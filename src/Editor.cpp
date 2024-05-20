/** 
 * MIT License
 * Copyright (c) 2023 Jimmy Givans
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "Editor.hpp"
#include "EditorConfig.hpp"
#include "AppendBuffer.hpp"
#include "Utilities.hpp"
#include "Terminal.hpp"

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

namespace Kilo::Editor 
{
    static EditorConfig editorConfig;

    /// @brief Process the results from readKey
    /// @details This function is responsible for mapping keypresses to editor operations
    /// @throws std::system_error if an error occured during read
    void processKeypress()
    {
        int c = Terminal::readKey();

        switch (c) {
            using enum Kilo::Utilities::EditorKey;

            case Utilities::ctrlKey('q'):
                Utilities::clearScreenAndRepositionCursor();
                std::exit(EXIT_SUCCESS);
                break;
            case static_cast<int>(Home):
                editorConfig.cursorX = 0;
                break;
            case static_cast<int>(End):
                editorConfig.cursorX = editorConfig.screenCols - 1;
                break;
            case static_cast<int>(PageUp):
            case static_cast<int>(PageDown):
                {
                    for (int i = editorConfig.screenRows; i > 0; i--) {
                        moveCursor(c == static_cast<int>(PageUp) ? static_cast<int>(ArrowUp) : static_cast<int>(ArrowDown));
                    }
                }
                break;
            case static_cast<int>(ArrowUp):
            case static_cast<int>(ArrowDown):
            case static_cast<int>(ArrowLeft):
            case static_cast<int>(ArrowRight):
                moveCursor(c);
            default:
                break;
        }
    }

    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void refreshScreen() noexcept
    {
        AppendBuffer::AppendBuffer buffer {};

        AppendBuffer::abAppend(buffer, "\x1b[?25l", 6);    // hide the cursor when repainting
        AppendBuffer::abAppend(buffer, "\x1b[H", 3);    // reposition the cursor
        
        drawRows(buffer);

        char buf[32];

        // Specify the exact position we want the cursor to move to
        // We add 1 to cursorX and cursorY to convert from 0-indexed values to the 1-indexed values that the terminal uses
        std::snprintf(buf, sizeof buf, "\x1b[%d;%dH", editorConfig.cursorY + 1, editorConfig.cursorX + 1);
        
        AppendBuffer::abAppend(buffer, buf, std::strlen(buf));
        AppendBuffer::abAppend(buffer, "\x1b[?25h", 6);    // show the cursor

        ::write(STDOUT_FILENO, buffer.c_str(), buffer.length());
    }

    /// @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
    void drawRows(AppendBuffer::AppendBuffer& buffer) noexcept
    {
        using AppendBuffer::abAppend;
        using Utilities::KILO_VERSION;

        for (int y = 0; y < editorConfig.screenRows; ++y) {
            if (y == editorConfig.screenRows / 3) {
                char welcome[80] {};

                // Interpolate KILO_VERSION into the welcome message
                int welcomeLen = std::snprintf(welcome, sizeof(welcome), "Kilo editor -- version %s", KILO_VERSION);

                // Truncate the length of the string in case the terminal is too small to fit the welcome message
                if (welcomeLen > editorConfig.screenCols) {
                    welcomeLen = editorConfig.screenCols;
                }

                // Center the string
                // Divide the screen width by 2 and then subtract half the string's length from this value.
                // This tells us how far from the left edge of the screen we should start printing the string.
                // So, we fill that space with space characters, except for the first character, which should be a tilde

                int padding = (editorConfig.screenCols - welcomeLen) / 2;

                if (padding > 0) {
                    abAppend(buffer, "~", 1);
                    --padding;
                }

                while (padding > 0) {
                    abAppend(buffer, " ", 1);
                    --padding;
                }

                abAppend(buffer, welcome, welcomeLen);
            }
            else {
                abAppend(buffer, "~", 1);
            }

            abAppend(buffer, "\x1b[K", 3);

            if (y < editorConfig.screenRows - 1) {
                abAppend(buffer, "\r\n", 2);
            }
        }
    }

    /**
     * @brief Move the cursor in accordance with the key pressed
     *
     * @param key The character representing the direction to move the cursor in
     */
    void moveCursor(int key)
    {
        using enum Kilo::Utilities::EditorKey;

        switch (key) {
            case static_cast<int>(ArrowLeft):
                if (editorConfig.cursorX != 0) {
                    editorConfig.cursorX--;
                }

                break;
            case static_cast<int>(ArrowRight):
                if (editorConfig.cursorX != editorConfig.screenCols - 1) {
                    editorConfig.cursorX++;
                }

                break;
            case static_cast<int>(ArrowUp):
                if (editorConfig.cursorY != 0) {
                    editorConfig.cursorY--;
                }

                break;
            case static_cast<int>(ArrowDown):
                if (editorConfig.cursorY != editorConfig.screenRows - 1) {
                    editorConfig.cursorY++;
                }
                
                break;
            default:
                break;
        }
    }
}