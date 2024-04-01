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

#include "Terminal.hpp"

#include <unistd.h>
#include <cerrno>
#include <system_error>
#include <sys/ioctl.h>
#include <cstring>

namespace Kilo::Terminal
{
    namespace
    {
        /**
         * @brief Get the original terminal driver settings and write them to a struct
         * @param[inout] settings Where the terminal driver settings are written to
         * @throws std::system_error if we could not query the terminal driver
        */
        void getOriginalTerminalSettings(termios& settings)
        {
            if (errno = 0; ::tcgetattr(STDIN_FILENO, &settings) == -1) {
                throw std::system_error(errno, std::system_category(), "Could not retrieve original terminal driver settings");
            }
        }

        /**
         * @brief Write new settings to the terminal driver
         * @param[in] settings The struct containing the settings to be written
         * @throws std::system_error if we could not write the new settings
        */
        void setNewTerminalSettings(termios const& settings)
        {
            if (errno = 0; ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &settings) == -1) {
                throw std::system_error(errno, std::system_category(), "Could not set terminal driver to raw mode");
            }
        }
    }

    /**
     * @brief Enable raw mode by turning off some terminal flags
     * 
     * @param[in] canonicalSettings The original settings of the terminal driver
     * @pre canonicalSettings must be a valid reference
     * @throws std::system_error if either querying the terminal driver for its current settings fails
     * or if writing the modified settings fails.
    */
    void enableRawMode(termios& canonicalSettings)
    {
        getOriginalTerminalSettings(canonicalSettings);  

        // Copy the current terminal settings into another variable
        termios temp = canonicalSettings;

        // Modify the necessary settings
        temp.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        temp.c_oflag &= ~OPOST;
        temp.c_cflag |= CS8;
        temp.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        temp.c_cc[VMIN] = 0;
        temp.c_cc[VTIME] = 1;

        setNewTerminalSettings(temp);

        // Verify that all of the changes were successfully made because
        // ::tcsetattr returns successfully if at least one change was successful
        auto verifyTerminalSettings = [&temp] {
            return (temp.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON))
                || (temp.c_oflag & OPOST)
                || ((temp.c_cflag & CS8) != CS8)
                || (temp.c_lflag & (ECHO | ICANON | IEXTEN | ISIG))
                || (temp.c_cc[VMIN] != 0)
                || (temp.c_cc[VTIME] != 1);
        };

        if (verifyTerminalSettings()) {
            disableRawMode(canonicalSettings);
        }
    }

    /**
     * @brief Disable raw mode by restoring the original canonical settings
     * 
     * @param[in] canonicalSettings The original settings of the terminal driver
     * @pre canonicalSettings must be a valid reference
     * @throws std::system_error if restoring the terminal driver to normal mode fails
    */
    void disableRawMode(termios const& canonicalSettings)
    {
        if (errno = 0; ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &canonicalSettings) == -1) {
            throw std::system_error(errno, std::system_category(), "Could not restore terminal driver to normal mode");
        }
    }

    /** 
     * @brief Read key input from stdin
     * @return The character read
     * @throws std::system_error if an error occured during read
    */ 
    char readKey()
    {
        char c {};

        for (long nread = 0; nread != 1; nread = ::read(STDIN_FILENO, &c, 1)) {
            if (nread == -1 && errno != EAGAIN) {
                throw std::system_error(errno, std::system_category(), "Could not read key input from stdin");
            }

            errno = 0;
        }

        return c;
    }

    /**
     * @brief Get the size of the terminal window and write them to @param rows and @param cols
     * @param[inout] rows The number of rows of the terminal window
     * @param[inout] cols The number of columns of the terminal window
     * @throws std::system_error if the terminal window size could not be retrieved
    */
    void getWindowSize(int* const rows, int* const cols)
    {
        winsize ws;

        if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
            // Move the cursor to the bottom-right of the screen
            if (::write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
                throw std::system_error(errno, std::system_category(), "Could not move the cursor to the bottom-right of the screen");
            }

            getCursorPosition(rows, cols);
        }
        
        *cols = ws.ws_col;
        *rows = ws.ws_row;
    }

    /**
     * @brief Get the position of the cursor and write them to @param rows and @param cols
     * @param[inout] rows The number of rows of the terminal window
     * @param[inout] cols The number of columns of the terminal window
    */
    void getCursorPosition(int* const rows, int* const cols)
    {
        // Get the position of the cursor
        if (::write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
            throw std::system_error(errno, std::system_category(), "Could not get cursor position");
        }

        char buf[32] {};
        unsigned i {};

        while (i < sizeof(buf) - 1) {
            // Read the reply from stdin and store it in a buffer
            // We do this until we encounter the 'R' character
            
            if (::read(STDIN_FILENO, &buf[i], 1) != 1) {
                break;
            }

            if (buf[i] == 'R') {
                break;
            }
            
            ++i;
        }

        // Assign the null-termination character to the the final byte of buf because
        // C-strings should end with a zero byte
        buf[i] = '\0';

        // First make sure ::read responded with an escape sequence
        if (buf[0] != '\x1b' || buf[1] != '[') {
            throw std::system_error(std::make_error_code(std::errc::invalid_argument), "The buffer contains an invalid argument where aan espace sequence was expected.");
        }

        // At this point, we are passing a string of the form "35;76" to sscanf
        // We tell it to parse the 2 integers separated by a ';' and write the value into the rows and cols variables
        if (std::sscanf(&buf[2], "%d;%d", rows, cols) != 2) {
            throw std::system_error(errno, std::system_category(), "Failed to write buffer data into rows and cols variables");
        }
    }
}