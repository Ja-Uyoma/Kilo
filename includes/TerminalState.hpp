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

#ifndef TERMINAL_STATE_HPP
#define TERMINAL_STATE_HPP

#include <termios.h>

namespace Kilo {

class TerminalState
{
public:
  /**
   * @brief Construct a new Terminal State object
   * @throws std::system_error An exception describing what went wrong
   *
   */
  explicit TerminalState();

  /**
   * @brief Set the terminal driver to raw (or non-canonical) mode
   *
   */
  void setRawMode() &;

  /**
   * @brief Set the terminal driver to canonical mode
   *
   */
  void reset() const&;

private:
  enum class ttystate
  {
    Raw,
    Reset
  };

  termios m_termios {};
  termios m_copy {};
  mutable ttystate m_state {ttystate::Reset};
};

/**
 * @brief Query fd and write its settings to buf
 *
 * @param fd The file descriptor to be queried
 * @param buf Where the settings are written to
 * @details Actually a wrapper around tcgetattr
 * @throw std::system_error In case querying the file descriptor failed
 */
void getTerminalDriverSettings(int fd, termios& buf);

/**
 * @brief Set the terminal driver in raw mode
 *
 * @param fd The terminal driver's file descriptor
 * @param buf The buffer to which the terminal driver's settings are to be written
 * @param copy A copy of the settings stored in buf in case we need to roll back
 */
void ttyRaw(int fd, termios const& buf, termios& copy);

/**
 * @brief Set the terminal driver in canonical mode
 *
 * @param fd The terminal driver's file descriptor
 * @param buf The buffer from which the desired settings are to be read from
 */
void ttyReset(int fd, termios const& buf);

}   // namespace Kilo

#endif

