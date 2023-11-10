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

/************************************************************************************/

/**
 * This file contains the definitions of functions used to manipulate the terminal
 * driver settings with regards to entering and exiting raw and/or canonical mode
*/

#include "TerminalSettings.hpp"

#include <unistd.h>
#include <cerrno>
#include <system_error>

namespace Kilo
{
    /**
     * @brief Enable raw mode by turning off some terminal flags
     * 
     * @param[in] canonicalSettings The original settings of the terminal driver
     * @pre canonicalSettings must be a valid reference
    */
    void enableRawMode(termios& canonicalSettings)
    {
        // Query the terminal driver and write its current settings into canonicalSettings
        if (errno = 0; ::tcgetattr(STDIN_FILENO, &canonicalSettings) == -1) {
            throw std::system_error(errno, std::generic_category(), "Could not retrieve original terminal driver settings.");
        }  

        // Copy the current terminal settings into another variable
        termios temp = canonicalSettings;

        // Modify the necessary settings
        temp.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        temp.c_oflag &= ~OPOST;
        temp.c_cflag |= CS8;
        temp.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        temp.c_cc[VMIN] = 0;
        temp.c_cc[VTIME] = 1;

        // Write the new settings to the terminal driver
        if (errno = 0; ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &temp) == -1) {
            throw std::system_error(errno, std::generic_category(), "Could not set terminal driver to raw mode");
        }
    }

    /**
     * @brief Disable raw mode by restoring the original canonical settings
     * 
     * @param[in] canonicalSettings The original settings of the terminal driver
     * @pre canonicalSettings must be a valid reference
    */
    void disableRawMode(termios const& canonicalSettings)
    {
        if (errno = 0; ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &canonicalSettings) == -1) {
            throw std::system_error(errno, std::generic_category(), "Could not restore terminal driver to normal mode");
        }
    }
}