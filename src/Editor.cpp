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
#include "AppendBuffer.hpp"
#include "Utilities.hpp"
#include "Terminal.hpp"

#include <cstdio>
#include <system_error>
#include <unistd.h>
#include <cstdlib>
#include <iostream>

namespace Kilo::Editor 
{
    EditorConfig::EditorConfig()
    {
        try {
            Terminal::enableRawMode(m_origTermios);
            Terminal::getWindowSize(&m_screenRows, &m_screenCols);
        }
        catch (std::system_error const& err) {
            std::cerr << err.code() << ": " << err.what() << '\n';
        }
    }

    EditorConfig::~EditorConfig()
    {
        // We need to call disableRawMode at program exit to reset the terminal to its canonical settings
        // We cannot register an atexit handler for this because atexit doesn't accept functions
        // that take parameters.
        // Therefore, the solution is to use RAII with a static object whose resources must be cleaned up
        // at program exit
        
        Terminal::disableRawMode(m_origTermios);
    }

    static const Editor::EditorConfig editorConfig;

    /// @brief Process the results from readKey
    /// @details This function is responsible for mapping keypresses to editor operations
    /// @throws std::system_error if an error occured during read
    void processKeypress()
    {
        char c = Terminal::readKey();

        switch (c) {
            case Utilities::ctrlKey('q'):
                Utilities::clearScreenAndRepositionCursor();
                std::exit(EXIT_SUCCESS);
                break;
            default:
                return;
        }
    }

    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void refreshScreen() noexcept
    {
        AppendBuffer::AppendBuffer buffer {};

        AppendBuffer::abAppend(buffer, "\x1b[?25l", 6);    // hide the cursor when repainting
        AppendBuffer::abAppend(buffer, "\x1b[H", 3);    // reposition the cursor
        
        drawRows(buffer);

        AppendBuffer::abAppend(buffer, "\x1b[H", 3);    // reposition the cursor
        AppendBuffer::abAppend(buffer, "\x1b[?25h", 6);    // show the cursor

        ::write(STDOUT_FILENO, buffer.c_str(), buffer.length());
    }

    /// @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
    void drawRows(AppendBuffer::AppendBuffer& buffer) noexcept
    {
        using AppendBuffer::abAppend;
        using Utilities::KILO_VERSION;

        for (int y = 0; y < editorConfig.m_screenRows; ++y) {
            if (y == editorConfig.m_screenRows / 3) {
                char welcome[80] {};

                // Interpolate KILO_VERSION into the welcome message
                int welcomeLen = std::snprintf(welcome, sizeof(welcome), "Kilo editor -- version %s", KILO_VERSION);

                // Truncate the length of the string in case the terminal is too small to fit the welcome message
                if (welcomeLen > editorConfig.m_screenCols) {
                    welcomeLen = editorConfig.m_screenCols;
                }

                // Center the string
                // Divide the screen width by 2 and then subtract half the string's length from this value.
                // This tells us how far from the left edge of the screen we should start printing the string.
                // So, we fill that space with space characters, except for the first character, which should be a tilde

                int padding = (editorConfig.m_screenCols - welcomeLen) / 2;

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

            if (y < editorConfig.m_screenRows - 1) {
                abAppend(buffer, "\r\n", 2);
            }
        }
    }
}