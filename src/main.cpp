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
#include <system_error>
#include <unistd.h>

// Forward declaration to function Main()
namespace Kilo
{
    void Main();
}

int main()
{
    // Clear the screen and reposition the cursor to the top-left corner at exit
    // This is added as a fallback in case an error occurs in the middle of rendering the screen
    // We would otherwise have garbage and/or errors printed wherever the cursor happens to be at that point
    auto const clearScreenAndRepositionCursor = [] { 
        ::write(STDOUT_FILENO, "\x1b[2J", 4); 
        ::write(STDOUT_FILENO, "\x1b[H", 3); 
    };

    try {
        Kilo::Main();
    }
    catch (std::system_error const& err) {
        clearScreenAndRepositionCursor();
        std::cerr << err.code() << ": " << err.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
