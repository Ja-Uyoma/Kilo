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
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace Kilo::Editor 
{
    static EditorConfig editorConfig;

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

    void refreshScreen() noexcept
    {
        scroll();

        AppendBuffer::AppendBuffer buffer {};

        buffer.write("\x1b[?25l", 6);    // hide the cursor when repainting
        buffer.write("\x1b[H", 3);    // reposition the cursor
        
        drawRows(buffer);

        char buf[32];

        // Specify the exact position we want the cursor to move to
        // We add 1 to cursorX and cursorY to convert from 0-indexed values to the 1-indexed values that the terminal uses
        std::snprintf(buf, sizeof buf, "\x1b[%d;%dH", (editorConfig.cursorY - editorConfig.rowoff) + 1, (editorConfig.cursorX + editorConfig.coloff) + 1);
        
        buffer.write(buf, std::strlen(buf));
        buffer.write("\x1b[?25h", 6);    // show the cursor

        ::write(STDOUT_FILENO, buffer.c_str(), buffer.length());
    }

    void drawRows(AppendBuffer::AppendBuffer& buffer) noexcept
    {
        using AppendBuffer::abAppend;
        using Utilities::KILO_VERSION;

        for (int y = 0; y < editorConfig.screenRows; ++y) {
            if (int filerow = y + editorConfig.rowoff; filerow >= editorConfig.numrows) {
                if (editorConfig.numrows == 0 && y == editorConfig.screenRows / 3) {
                    char welcome[80] {};

                    // Interpolate KILO_VERSION into the welcome message
                    int welcomeLen = std::snprintf(
                        welcome,
                        sizeof welcome,
                        "Kilo editor -- version %s",
                        KILO_VERSION
                    );

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
                        buffer.write("~", 1);
                        padding--;
                    }

                    while (padding > 0) {
                        buffer.write(" ", 1);
                        padding--;
                    }

                    buffer.write(welcome, welcomeLen);
                }
                else {
                    buffer.write("~", 1);
                }
            }
            else {
                auto len = std::ssize(editorConfig.row[filerow]) - editorConfig.coloff;

                if (len < 0) {
                    len = 0;
                }
                
                if (len > editorConfig.screenCols) {
                    len = editorConfig.screenCols;
                }

                buffer.write(editorConfig.row[filerow]);
            }

            buffer.write("\x1b[K", 3);

            if (y < editorConfig.screenRows - 1) {
                buffer.write("\r\n", 2);
            }
        }
    }

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
                editorConfig.cursorX++;
                break;
            case static_cast<int>(ArrowUp):
                if (editorConfig.cursorY != 0) {
                    editorConfig.cursorY--;
                }

                break;
            case static_cast<int>(ArrowDown):
                if (editorConfig.cursorY != editorConfig.numrows) {
                    editorConfig.cursorY++;
                }
                
                break;
            default:
                break;
        }
    }

    bool open(std::filesystem::path const& path)
    {
        // TODO: rewrite the error handling logic to match the approach used in rest of the application

        std::ifstream infile(path);

        if (!infile) {
            return false;
        }

        std::string line;

        while (std::getline(infile, line)){
            editorConfig.row.push_back(line);
            editorConfig.numrows++;
        }

        return true;
    }

    void scroll()
    {
        /*
         * Check if the cursor has moved outside of the visible window.
         * If so, adjust editorConfig.rowoff and/or editorConfig.coloff so that the cursor is just inside the visible window
        */

        if (editorConfig.cursorY < editorConfig.rowoff) {
            editorConfig.rowoff = editorConfig.cursorY;
        }

        if (editorConfig.cursorY >= editorConfig.rowoff + editorConfig.screenRows) {
            editorConfig.rowoff = editorConfig.cursorY - editorConfig.screenRows + 1;
        }

        if (editorConfig.cursorX < editorConfig.coloff) {
            editorConfig.coloff = editorConfig.cursorX;
        }

        if (editorConfig.cursorX >= editorConfig.coloff + editorConfig.screenCols) {
            editorConfig.coloff = editorConfig.cursorX - editorConfig.screenCols + 1;
        }
    }
}