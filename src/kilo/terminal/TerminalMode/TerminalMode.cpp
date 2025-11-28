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

#include <system_error>

#include <cassert>
#include <cerrno>
#include <functional>
#include <iostream>
#include <termios.h>
#include <unistd.h>

namespace kilo::terminal {

///
/// \brief Default constructor
///
terminal_mode::terminal_mode()
{
  detail::get_terminal_driver_settings(STDIN_FILENO, m_termios);
}

///
/// \brief Destructor
///
terminal_mode::~terminal_mode()
{
  set_canonical_mode();
}

///
/// \brief Set the terminal driver to raw (or non-canonical) mode
/// \throws std::system_error on failure
///
void terminal_mode::set_raw_mode() &
{
  if (m_mode == tty_mode::raw) {
    return;
  }

  assert(m_mode == tty_mode::canonical && "Terminal driver currently in canonical mode");

  try {
    detail::tty_raw(STDIN_FILENO, m_termios, m_copy);
    m_mode = tty_mode::raw;
  }
  catch (std::system_error const& err) {
    std::cerr << err.code().message() << ": " << err.what() << '\n';
    m_mode = tty_mode::canonical;
    throw;
  }
}

///
/// \brief Set the terminal driver to canonical mode
///
void terminal_mode::set_canonical_mode() &
{
  if (m_mode == tty_mode::canonical) {
    return;
  }

  assert(m_mode == tty_mode::raw && "Terminal driver currently in raw mode");

  try {
    detail::tty_canonical_mode(STDIN_FILENO, m_termios);
    m_mode = tty_mode::canonical;
  }
  catch (std::system_error const& err) {
    std::cerr << err.code().message() << ": " << err.what() << '\n';
    m_mode = tty_mode::raw;
  }
}

namespace detail {

///
/// \brief Query file_descriptor and write its settings to buf
/// \param[in] file_descriptor The file descriptor to be queried
/// \param[in] buf Where the settings are written to
/// \throws std::system_error on failure
///
void get_terminal_driver_settings(int file_descriptor, termios& buf)
{
  assert(file_descriptor == STDIN_FILENO and "File descriptor must be STDIN_FILENO");

  errno = 0;

  if (::tcgetattr(file_descriptor, &buf) == -1) [[unlikely]] {
    throw std::system_error(errno, std::system_category(), "Could not retrieve terminal driver settings");
  }
}

///
/// \brief Set the terminal driver in raw mode
/// \param[in] file_descriptor The terminal driver's file descriptor
/// \param[in] buf The buffer to which the terminal driver's settings are to be written
/// \param[in] copy A copy of the settings stored in buf in case we need to roll back
///
void tty_raw(int file_descriptor, termios const& buf, termios& copy)
{
  assert(file_descriptor == STDIN_FILENO and "File descriptor must be STDIN_FILENO");

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
    throw std::system_error(errno, std::system_category(), "Failed to set terminal driver to raw mode");
  }

  /*
   * Verify that the changes stuck since tcsetattr can return 0 on partial success
   */

  errno = 0;

  if (::tcgetattr(file_descriptor, &copy) == -1) [[unlikely]] {
    ::tcsetattr(file_descriptor, TCSAFLUSH, &buf);
    throw std::system_error(errno, std::system_category(), "Error while writing terminal driver settings to buffer");
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
    ::tcsetattr(file_descriptor, TCSAFLUSH, &buf);
    throw std::system_error(EINVAL, std::system_category(), "Setting driver to raw mode only partially successful");
  }
}

///
/// \brief Set the terminal driver in canonical mode
/// \param[in] file_descriptor The terminal driver's file descriptor
/// \param[in] buf The buffer from which the desired settings are to be read from
///
void tty_canonical_mode(int file_descriptor, termios const& buf)
{
  assert(file_descriptor == STDIN_FILENO and "File descriptor must be STDIN_FILENO");

  errno = 0;

  if (::tcsetattr(file_descriptor, TCSAFLUSH, &buf) == -1) [[unlikely]] {
    throw std::system_error(errno, std::system_category(), "Failed to reset terminal driver to canonical mode");
  }
}

}   // namespace detail

}   // namespace kilo::terminal
