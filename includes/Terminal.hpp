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
 * This file contains the declarations of functions used to manipulate the terminal
 * driver settings with regards to entering and exiting raw and/or canonical mode
*/

#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <termios.h>

namespace Kilo::Terminal
{
    /**
     * @brief Enable raw mode by turning off some terminal flags
     * 
     * @param[in] canonicalSettings The original settings of the terminal driver
     * @pre canonicalSettings must be a valid reference
     * @throws std::system_error if either querying the terminal driver for its current settings fails
     * or if writing the modified settings fails.
    */
    void enableRawMode(termios& canonicalSettings);

    /**
     * @brief Disable raw mode by restoring the original canonical settings
     * 
     * @param[in] canonicalSettings The original settings of the terminal driver
     * @pre canonicalSettings must be a valid reference
     * @throws std::system_error if restoring the terminal driver to normal mode fails
    */
    void disableRawMode(termios const& canonicalSettings);

    /** 
     * @brief Read key input from stdin
     * @return The character read
     * @throws std::system_error if an error occured during read
    */ 
    char readKey();

    /**
     * @brief Get the size of the terminal window
     * @param[inout] rows The number of rows of the terminal window
     * @param[inout] cols The number of columns of the terminal window
     * @throws std::system_error if the terminal window size could not be retrieved
     * @returns 0 on success
    */
    int getWindowSize(int* const rows, int* const cols);
}

#endif