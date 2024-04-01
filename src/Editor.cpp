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
#include "Utilities.hpp"
#include "Terminal.hpp"

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
    void refreshScreen()
    {
        Utilities::clearScreenAndRepositionCursor();
        drawRows();
        ::write(STDOUT_FILENO, "\x1b[H", 3);
    }

    /// @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
    void drawRows() noexcept
    {
        for (int y = 0; y < editorConfig.m_screenRows; ++y) {
            ::write(STDOUT_FILENO, "~", 1);

            if (y < editorConfig.m_screenRows - 1) {
                ::write(STDOUT_FILENO, "\r\n", 2);
            }
        }
    }
}