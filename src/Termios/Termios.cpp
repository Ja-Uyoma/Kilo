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

#include "Termios.hpp"

#include <cerrno>
#include <system_error>

namespace Kilo::Terminal {

/**
 * @brief Query fd and write its settings to buf
 *
 * @param fd The file descriptor to be queried
 * @param buf Where the settings are written to
 * @details Actually a wrapper around tcgetattr
 * @throw std::system_error In case querying the file descriptor failed
 */
void getTerminalDriverSettings(int fd, termios& buf)
{
  errno = 0;

  if (::tcgetattr(fd, &buf) == -1) {
    throw std::system_error(errno, std::system_category(), "Could not retrieve terminal driver settings");
  }
}

/**
 * @brief Set the terminal driver in raw mode
 *
 * @param fd The terminal driver's file descriptor
 * @param buf The buffer to which the terminal driver's settings are to be written
 * @param copy A copy of the settings stored in buf in case we need to roll back
 */
void ttyRaw(int fd, termios const& buf, termios& copy)
{
  copy = buf;

  /*
   * No SIGINT on BREAK, CR-to-NL off, input parity check off, don't strip 8th bit on input, output
   * flow control off
   */
  copy.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  /* Output processing off */
  copy.c_oflag &= ~OPOST;

  /* Set 8 bits per char */
  copy.c_cflag |= CS8;

  /* Echo off, canonical mode off, extended input processing off, signal chars off */
  copy.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

  /* Read 1 byte at a time */
  copy.c_cc[VMIN] = 0;

  /* No timer */
  copy.c_cc[VTIME] = 1;

  errno = 0;

  if (::tcsetattr(fd, TCSAFLUSH, &copy) == -1) {
    throw std::system_error(errno, std::system_category(), "Failed to set terminal driver to raw mode");
  }

  /*
   * Verify that the changes stuck since tcsetattr can return 0 on partial success
   */

  errno = 0;

  if (::tcgetattr(fd, &copy) == -1) {
    ::tcsetattr(fd, TCSAFLUSH, &buf);
    throw std::system_error(errno, std::system_category(), "Error while writing terminal driver settings to buffer");
  }

  auto const verify = [&copy] {
    return (copy.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON)) || (copy.c_oflag & OPOST)
        || ((copy.c_cflag & CS8) != CS8) || (copy.c_lflag & (ECHO | ICANON | IEXTEN | ISIG)) || (copy.c_cc[VMIN] != 0)
        || (copy.c_cc[VTIME] != 1);
  };

  /*
   * Only some of the changes stuck. Restore the original settings
   */

  if (verify()) {
    ::tcsetattr(fd, TCSAFLUSH, &buf);
    throw std::system_error(EINVAL, std::system_category(), "Setting driver to raw mode only partially successful");
  }
}

/**
 * @brief Set the terminal driver in canonical mode
 *
 * @param fd The terminal driver's file descriptor
 * @param buf The buffer from which the desired settings are to be read from
 */
void ttyReset(int fd, termios const& buf)
{
  errno = 0;

  if (::tcsetattr(fd, TCSAFLUSH, &buf) == -1) {
    throw std::system_error(errno, std::system_category(), "Failed to reset terminal driver to canonical mode");
  }
}

}   // namespace Kilo::Terminal

