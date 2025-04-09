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

#include "Window.hpp"

#include <array>
#include <stdexcept>
#include <sys/ioctl.h>
#include <system_error>
#include <unistd.h>
#include <cerrno>
#include "File/File.hpp"
#include <string>
#include <cstdlib>

namespace Kilo::Terminal {

Window::Window() : m_winsize(detail::getWindowSize())
{
}

namespace detail {

/// Get the size of the open terminal window
/// \throws std::system_error on failure
/// \returns The size of the terminal window as a WindowSize instance on success
auto getWindowSize() -> WindowSize
{
  ::winsize ws;
  IO::File file;

  if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 or ws.ws_col == 0) {
    errno = 0;

    if (file.write(STDOUT_FILENO, std::string("\x1b[999c\x1b[999B")) != 12) {
      throw std::system_error(errno, std::system_category(),
                              "Could not move the cursor to the bottom-right of the screen");
    }
    else {
      return detail::getCursorPosition(file);
    }
  }

  return WindowSize { .cols = ws.ws_col, .rows = ws.ws_row};
}

/// Get the position of the cursor in the terminal window
/// \throws std::system_error on failure
/// \returns The position of the cursor as a WindowSize instance
auto getCursorPosition(IO::FileInterface& file) -> WindowSize
{
  // Get the position of the cursor
  if (file.write(STDOUT_FILENO, std::string("\x1b[6n")) != 4) {
    throw std::system_error(errno, std::system_category(), "Could not get cursor position");
  }

  // Read the reply from stdin and store it in a buffer
  // Do this until we encounter a 'R' character

  std::array<char, 32> buf;

  for (std::size_t i = 0; i < buf.size() - 1; ++i) {
    if (::read(STDIN_FILENO, &buf[0], 1) != 1 or buf[i] == 'R') {
      break;
    }
  }

  // Assign the null-termination character to the final byte of buf
  buf.back() = '\0';

  // First make sure read() responded with an escape sequence
  if (buf[0] != '\x1b' or buf[1] != '[') {
    throw std::invalid_argument("An invalid byte sequence was encountered where an escape sequence was expected");
  }

  WindowSize result { .cols = 0, .rows = 0 };

  // At this point, we are passing a string of the form "35;76" to sscanf
  // We tell it to parse the 2 integers separated by a ';' and write the value
  // into the rows and cols variables
  if (std::sscanf(&buf[2], "%d;%d", &result.rows, &result.cols) != 2) {
    throw std::runtime_error("Failed to parse cursor position");
  }

  return result;
}

}   // namespace detail

}   // namespace Kilo::Terminal
