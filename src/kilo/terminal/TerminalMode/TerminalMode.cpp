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

#include "TerminalMode.hpp"

#include <fmt/core.h>
#include <gsl/assert>
#include <system_error>

#include <cerrno>
#include <functional>
#include <termios.h>
#include <unistd.h>

namespace kilo::terminal {

terminal_mode::terminal_mode() noexcept(false)
{
  detail::get_terminal_driver_settings(STDIN_FILENO, m_termios);
}

terminal_mode::~terminal_mode() noexcept
{
  try {
    set_canonical_mode();
  }
  catch (std::system_error const& err) {
    fmt::print(stderr, "{}: {}\n", err.code().message(), err.what());
  }
}

void terminal_mode::set_raw_mode() & noexcept(false)
{
  if (m_mode == tty_mode::raw) {
    return;
  }

  Expects(m_mode == tty_mode::canonical && "Terminal driver is in canonical mode");

  detail::tty_raw(STDIN_FILENO, m_termios, m_copy);
  m_mode = tty_mode::raw;

  Ensures(m_mode == tty_mode::raw and "Terminal driver is in raw mode");
}

void terminal_mode::set_canonical_mode() & noexcept(false)
{
  if (m_mode == tty_mode::canonical) {
    return;
  }

  Expects(m_mode == tty_mode::raw and "Terminal driver is in raw mode");

  detail::tty_canonical_mode(STDIN_FILENO, m_termios);
  m_mode = tty_mode::canonical;

  Ensures(m_mode == tty_mode::canonical and "Terminal driver is in canonical mode");
}

namespace detail {

void get_terminal_driver_settings(int file_descriptor, termios& buf) noexcept(false)
{
  errno = 0;

  if (::tcgetattr(file_descriptor, &buf) == -1) [[unlikely]] {
    throw std::system_error(errno, std::system_category(), "Could not retrieve terminal driver settings");
  }
}

void tty_raw(int file_descriptor, termios const& buf, termios& copy) noexcept(false)
{
  copy = buf;

  /*
   * No SIGINT on BREAK, CR-to-NL off, input parity check off, don't strip 8th bit on input, output
   * flow control off
   */
  copy.c_iflag &= ~(static_cast<tcflag_t>(BRKINT | ICRNL | INPCK | ISTRIP | IXON));

  /* Output processing off */
  copy.c_oflag &= ~(static_cast<tcflag_t>(OPOST));

  /* Set 8 bits per char */
  copy.c_cflag |= CS8;

  /* Echo off, canonical mode off, extended input processing off, signal chars off */
  copy.c_lflag &= ~(static_cast<tcflag_t>(ECHO | ICANON | IEXTEN | ISIG));

  /* Read 1 byte at a time */
  copy.c_cc[VMIN] = 0;

  /* No timer */
  copy.c_cc[VTIME] = 1;

  errno = 0;

  if (::tcsetattr(file_descriptor, TCSAFLUSH, &copy) == -1) [[unlikely]] {
    throw std::system_error(errno, std::system_category(), "Error while setting terminal driver to raw mode");
  }

  /*
   * Verify that the changes stuck since tcsetattr can return 0 on partial success
   */

  errno = 0;

  if (::tcgetattr(file_descriptor, &copy) == -1) [[unlikely]] {
    throw std::system_error(errno, std::system_category(), "Error while quering terminal driver for success of initial operation");

    if (::tcsetattr(file_descriptor, TCSAFLUSH, &buf) == -1) [[unlikely]] {
      throw std::system_error(errno, std::system_category(), "Error while restoring original terminal driver settings");
    }
  }

  auto const changes_did_not_stick = std::invoke([&copy]() -> bool {
    return (copy.c_iflag & static_cast<tcflag_t>(BRKINT | ICRNL | INPCK | ISTRIP | IXON)) != 0 ||
           (copy.c_oflag & OPOST) != 0 || ((copy.c_cflag & CS8) != CS8) ||
           (copy.c_lflag & static_cast<tcflag_t>(ECHO | ICANON | IEXTEN | ISIG)) != 0 || (copy.c_cc[VMIN] != 0) ||
           (copy.c_cc[VTIME] != 1);
  });

  /*
   * Only some of the changes stuck. Restore the original settings
   */

  if (changes_did_not_stick) {
    if (::tcsetattr(file_descriptor, TCSAFLUSH, &buf) == -1) [[unlikely]] {
      throw std::system_error(errno, std::system_category(), "Error while restoring original terminal driver settings");
    }

    throw std::system_error(EINVAL, std::system_category(), "Error while setting terminal driver to raw mode. Restoring original settings...");
  }
}

void tty_canonical_mode(int file_descriptor, termios const& buf) noexcept(false)
{
  errno = 0;

  if (::tcsetattr(file_descriptor, TCSAFLUSH, &buf) == -1) [[unlikely]] {
    throw std::system_error(errno, std::system_category(), "Failed to reset terminal driver to canonical mode");
  }
}

}   // namespace detail

}   // namespace kilo::terminal
