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

#include "File/File.hpp"
#include <sys/ioctl.h>
#include <system_error>

#include <array>
#include <cerrno>
#include <charconv>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <unistd.h>

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
  ::winsize ws {};

  if (IO::File file; ::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 or ws.ws_col == 0) {
    errno = 0;

    if (file.write(STDOUT_FILENO, std::string("\x1b[999c\x1b[999B")) != 12) {
      throw std::system_error(errno, std::system_category(),
                              "Could not move the cursor to the bottom-right of the screen");
    }

    return detail::getCursorPosition(file);
  }

  return WindowSize {.cols = ws.ws_col, .rows = ws.ws_row};
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

  std::array<char, 32> buf = {};

  for (std::size_t i = 0; i < buf.size() - 1; ++i) {
    if (::read(STDIN_FILENO, &buf[i], 1) != 1 or buf[i] == 'R') {
      break;
    }
  }

  // Assign the null-termination character to the final byte of buf
  buf.back() = '\0';

  // First make sure read() responded with an escape sequence
  if (buf[0] != '\x1b' or buf[1] != '[') {
    throw std::invalid_argument("An invalid byte sequence was encountered "
                                "where an escape sequence was expected");
  }

  WindowSize result {.cols = 0, .rows = 0};

  // At this point, we are passing a string of the form "35;76" to std::from_chars
  // We tell it to parse the 2 integers separated by a ';' and write the value
  // into the rows and cols variables

  char const* parsePtr = &buf[2];
  char const* endPtr = &buf.back();

  // Parse rows
  auto [rowEndPtr, rowEc] = std::from_chars(parsePtr, endPtr, result.rows);

  // Check error if no characters consumed
  if (rowEc != std::errc() or rowEndPtr == parsePtr) {
    return {};    // failed to parse rows or row-string empty
  }

  // Check for semicolon
  if (rowEndPtr == endPtr or *rowEndPtr != ';') {
    return {};    // expected semicolon not found or end of string
  }

  // Skip semicolon
  parsePtr = rowEndPtr + 1;

  // Parse columns
  auto [colEndPtr, colEc] = std::from_chars(parsePtr, endPtr, result.cols);

  // Check error; no characters consumed, or not ending at 'R'
  if (colEc != std::errc() or colEndPtr == parsePtr or colEndPtr != endPtr) {
    // Failed to parse columns, or cols string empty, or extra characters encountered before 'R'
    return {};
  }

  return result;
}

}   // namespace detail

}   // namespace Kilo::Terminal
