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

#include "kilo/io/File.hpp"
#include <gsl/assert>
#include <sys/ioctl.h>
#include <system_error>

#include <array>
#include <cerrno>
#include <charconv>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace kilo::terminal {

window::window() noexcept(false)
{
  io::file file;
  ::winsize winsz {};

  m_winsize = detail::get_window_size(file, winsz);
}

namespace detail {

auto get_window_size(io::file_interface& file, winsize& winsz) noexcept(false) -> window_size
{
  if (file.ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsz) == -1 or winsz.ws_col == 0) {
    static constexpr std::string move_cursor_bottom_right("\x1b[999c\x1b[999B");

    if (file.write(STDOUT_FILENO, move_cursor_bottom_right) != std::ssize(move_cursor_bottom_right)) {
      throw std::system_error(errno, std::system_category(), "Could not move cursor to bottom-right of screen");
    }

    return detail::get_cursor_position(file);
  }

  return window_size {.cols = winsz.ws_col, .rows = winsz.ws_row};
}

auto get_cursor_position(io::file_interface& file) noexcept(false) -> window_size
{
  Expects(isatty(STDIN_FILENO) and "STDIN must be a terminal device");
  Expects(isatty(STDOUT_FILENO) and "STDOUT must be a terminal device");

  // Get the position of the cursor
  if (file.write(STDOUT_FILENO, std::string("\x1b[6n")) != 4) {
    throw std::system_error(errno, std::system_category(), "Could not get cursor position");
  }

  // Read the reply from stdin and store it in a buffer
  // Do this until we encounter a 'R' character

  static constexpr unsigned buffer_size = 32;
  std::array<char, buffer_size> buf = {};

  for (std::size_t i = 0; i < buf.size() - 1; ++i) {
    if (::read(STDIN_FILENO, &buf.at(i), 1) != 1 or buf.at(i) == 'R') {
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

  window_size result {.cols = 0, .rows = 0};

  // At this point, we are passing a string of the form "35;76" to std::from_chars
  // We tell it to parse the 2 integers separated by a ';' and write the value
  // into the rows and cols variables

  char const* parse_ptr = &buf[2];
  char const* end_ptr = &buf.back();

  // Parse rows
  auto [row_end_ptr, row_ec] = std::from_chars(parse_ptr, end_ptr, result.rows);

  // Check error if no characters consumed
  if (row_ec != std::errc() or row_end_ptr == parse_ptr) {
    return {};   // failed to parse rows or row-string empty
  }

  // Check for semicolon
  if (row_end_ptr == end_ptr or *row_end_ptr != ';') {
    return {};   // expected semicolon not found or end of string
  }

  // Skip semicolon
  parse_ptr = row_end_ptr + 1;   // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

  // Parse columns
  auto [col_end_ptr, col_ec] = std::from_chars(parse_ptr, end_ptr, result.cols);

  // Check error; no characters consumed, or not ending at 'R'
  if (col_ec != std::errc() or col_end_ptr == parse_ptr or col_end_ptr != end_ptr) {
    // Failed to parse columns, or cols string empty, or extra characters encountered before 'R'
    return {};
  }

  return result;
}

}   // namespace detail

}   // namespace kilo::terminal
