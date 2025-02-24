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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Terminal.hpp"

#include "Window/window_size.hpp"
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <sys/ioctl.h>
#include <system_error>
#include <unistd.h>

namespace Kilo::Terminal {

void getWindowSize(int* const rows, int* const cols)
{
  winsize ws;

  if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    // Move the cursor to the bottom-right of the screen
    if (::write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
      throw std::system_error(errno, std::system_category(),
                              "Could not move the cursor to the bottom-right of the screen");
    }

    detail::getCursorPosition(rows, cols);
  }

  *cols = ws.ws_col;
  *rows = ws.ws_row;
}

/**
 * @brief Get the size of the terminal window
 * @throws std::system_error if the terminal window size could not be retrieved
 * @returns The size of the terminal window
 */
WindowSize getWindowSize(::winsize& winsize)
{
  if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize) == -1 or winsize.ws_col == 0) {
    errno = 0;

    if (::write(STDOUT_FILENO, "\x1b[999c\x1b[999B", 12) != 12) {
      throw std::system_error(errno, std::system_category(),
                              "Could not move the cursor to the bottom-right of the screen");
    }
    else {
      auto [c, r] = detail::getCursorPosition();
      return WindowSize {.cols = c, .rows = r};
    }
  }

  return WindowSize {.cols = winsize.ws_col, .rows = winsize.ws_row};
}

namespace detail {

void getCursorPosition(int* const rows, int* const cols)
{
  // Get the position of the cursor
  if (::write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
    throw std::system_error(errno, std::system_category(), "Could not get cursor position");
  }

  std::array<char, 32> buf {};

  /*
   * Read the reply from stdin and store it in a buffer
   * We do this until we encounter a 'R' character
   */

  for (std::size_t i {}; i < buf.size() - 1; i++) {
    if (::read(STDIN_FILENO, &buf[i], 1) != 1 or buf[i] == 'R') {
      break;
    }
  }

  /*
   * Assign the null-termination character to the the final byte of buf because
   * C-strings should end with a zero byte
   */

  buf.back() = '\0';

  // First make sure ::read responded with an escape sequence
  if (buf[0] != '\x1b' || buf[1] != '[') {
    throw std::system_error(std::make_error_code(std::errc::invalid_argument),
                            "An invalid argument was encountered where an "
                            "escape sequence was expected.");
  }

  // At this point, we are passing a string of the form "35;76" to sscanf
  // We tell it to parse the 2 integers separated by a ';' and write the value
  // into the rows and cols variables
  if (std::sscanf(&buf[2], "%d;%d", rows, cols) != 2) {
    throw std::system_error(errno, std::system_category(), "Failed to write buffer data into rows and cols variables");
  }
}

/**
 * @brief Get the position of the cursor
 *
 * @throws std::system_error If we could not determine the position of the cursor
 * @returns The position of the cursor
 */
WindowSize getCursorPosition()
{
  // Get the position of the cursor
  errno = 0;

  if (::write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
    throw std::system_error(errno, std::system_category(), "Could not get cursor position");
  }

  std::array<char, 32> buf {};

  /*
   * Read the reply from stdin and store it in a buffer
   * We do this until we encounter a 'R' character
   */

  for (std::size_t i {}; i < buf.size() - 1; i++) {
    if (::read(STDIN_FILENO, &buf[i], 1) != 1 or buf[i] == 'R') {
      break;
    }
  }

  /*
   * Assign the null-termination character to the the final byte of buf because
   * C-strings should end with a zero byte
   */

  buf.back() = '\0';

  // First make sure ::read responded with an escape sequence
  if (buf[0] != '\x1b' || buf[1] != '[') {
    throw std::system_error(std::make_error_code(std::errc::invalid_argument),
                            "An invalid argument was encountered where an "
                            "escape sequence was expected.");
  }

  WindowSize cursorPos;

  // At this point, we are passing a string of the form "35;76" to sscanf
  // We tell it to parse the 2 integers separated by a ';' and write the value
  // into the rows and cols variables
  if (std::sscanf(&buf[2], "%d;%d", &cursorPos.rows, &cursorPos.cols) != 2) {
    throw std::system_error(errno, std::system_category(), "Failed to write buffer data into rows and cols variables");
  }

  return cursorPos;
}

}   // namespace detail

}   // namespace Kilo::Terminal
