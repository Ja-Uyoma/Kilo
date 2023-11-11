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

#include "TerminalSettings.hpp"

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <cctype>
#include <cerrno>
#include <system_error>

int main()
{
    using Kilo::enableRawMode;
    using Kilo::disableRawMode;

    termios canonicalSettings {};

    enableRawMode(canonicalSettings);

    for ( ; ; ) {
        char c {'\0'};

        if (::read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) {
            throw std::system_error(errno, std::generic_category());
        }
    
    #ifdef DEBUG
        if (std::iscntrl(c)) {
            std::clog << static_cast<int>(c) << "\r\n";
        }
        else {
            std::clog << static_cast<int>(c) << " (" << c << ")\r\n";
        }
    #endif

        if (c == 'q') {
            break;
        }

        continue;
    }

    disableRawMode(canonicalSettings);

    return EXIT_SUCCESS;
}