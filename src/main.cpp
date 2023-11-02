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

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <termios.h>

int main()
{
    termios canonicalSettings {};

    auto enableRawMode = [&canonicalSettings] {
        // Store the current canonical mode terminal settings
        ::tcgetattr(STDIN_FILENO, &canonicalSettings);

        // Copy the canonical settings to a variable which can be modified
        termios temp = canonicalSettings;

        // Disable echoing of input
        temp.c_lflag &= ~ECHO;

        // Write these settings to the terminal handler for STDIN
        ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &temp);
    };

    auto disableRawMode = [&canonicalSettings] {
        ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &canonicalSettings);
    };

    enableRawMode();

    for (char c {}; read(STDIN_FILENO, &c, 1) == 1 && c != 'q'; ) {
        continue;
    }

    disableRawMode();

    return EXIT_SUCCESS;
}