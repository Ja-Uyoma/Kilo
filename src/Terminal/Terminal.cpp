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

#include "Utilities/Utilities.hpp"
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <sys/ioctl.h>
#include <system_error>
#include <unistd.h>

namespace Kilo::terminal {

int readKey()
{
  char c {};

  for (long nread = 0; nread != 1; nread = ::read(STDIN_FILENO, &c, 1)) {
    if (nread == -1 && errno != EAGAIN) {
      throw std::system_error(errno, std::system_category(), "Could not read key input from stdin");
    }

    errno = 0;
  }

  // Pressing an arrow key sends multiple bytes as input to our program.
  // These bytes are in the form of an escape sequence that starts with '\x1b',
  // '[', followed by an 'A', 'B', 'C', or 'D', depending on which of the 4
  // arrow keys was pressed. We want to read escape sequences of this form as a
  // single key press.

  if (c == '\x1b') {
    return detail::handleEscapeSequences();
  }
  else {
    return c;
  }
}

void getWindowSize(int* const rows, int* const cols)
{
  winsize ws;

  if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    // Move the cursor to the bottom-right of the screen
    if (::write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
      throw std::system_error(
        errno, std::system_category(), "Could not move the cursor to the bottom-right of the screen");
    }

    detail::getCursorPosition(rows, cols);
  }

  *cols = ws.ws_col;
  *rows = ws.ws_row;
}

namespace detail {

int handleEscapeSequences() noexcept
{
  std::array<char, 3> seq {};

  /*
   * If we read an escape character, we immediately read 2 more bytes into the
   * seq buffer. If either of these reads times out, then we assume the user
   * just pressed the Escape key and return that.
   */

  if (::read(STDIN_FILENO, &seq[0], 1) != 1) {
    return '\x1b';
  }

  if (::read(STDIN_FILENO, &seq[1], 1) != 1) {
    return '\x1b';
  }

  if (seq[0] == '[') {
    using enum Kilo::utilities::EditorKey;

    /*
     * If the byte after [ is a digit, we read another byte expecting it to be
     * a ~. Then we test the digit byte to see if it's a 1, 4, 5, 6, 7, or 8.
     * Page Up is sent as \x1b[5~, and Page Down is sent as \x1b[6~.
     * Delete is sent as \x1b[3~.
     * Home could be sent as \x1b[1~, \x1b[7~, \x1b[H, or \x1b[OH
     * End could be sent as \x1b[4~, \x1b[8~, \x1b[F, or \x1b[OF.
     */

    if (seq[1] >= '0' && seq[1] <= '9') {
      if (::read(STDIN_FILENO, &seq[2], 1) != 1) {
        return '\x1b';
      }

      if (seq[2] == '~') {
        switch (seq[1]) {
          case '1':
            return static_cast<int>(Home);
          case '3':
            return static_cast<int>(Delete);
          case '4':
            return static_cast<int>(End);
          case '5':
            return static_cast<int>(PageUp);
          case '6':
            return static_cast<int>(PageDown);
          case '7':
            return static_cast<int>(Home);
          case '8':
            return static_cast<int>(End);
        }
      }
    }

    /*
     * Otherwise we look to see if the escape sequence is an arrow key or Home
     * or End escape sequence. If it is, we just return the corresponding [w,
     * a, s, d] character for now. If it isn't, we just return the escape
     * character
     */

    else {
      switch (seq[1]) {
        case 'A':
          return static_cast<int>(ArrowUp);
        case 'B':
          return static_cast<int>(ArrowDown);
        case 'C':
          return static_cast<int>(ArrowRight);
        case 'D':
          return static_cast<int>(ArrowLeft);
        case 'H':
          return static_cast<int>(Home);
        case 'F':
          return static_cast<int>(End);
      }
    }
  }
  else if (seq[0] == 'O') {
    using enum Kilo::utilities::EditorKey;

    switch (seq[1]) {
      case 'H':
        return static_cast<int>(Home);
      case 'F':
        return static_cast<int>(End);
    }
  }

  return '\x1b';
}

void writeCursorPositionToBuffer(std::array<char, 32>& buf) noexcept
{
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
}

void getCursorPosition(int* const rows, int* const cols)
{
  // Get the position of the cursor
  if (::write(STDOUT_FILENO, "\x1b[6n", 4) != 4) {
    throw std::system_error(errno, std::system_category(), "Could not get cursor position");
  }

  std::array<char, 32> buf {};

  detail::writeCursorPositionToBuffer(buf);

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

}   // namespace detail

}   // namespace Kilo::terminal
