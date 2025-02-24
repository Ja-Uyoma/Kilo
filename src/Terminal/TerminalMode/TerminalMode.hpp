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

namespace Kilo::Terminal {

class TerminalMode
{
public:
  /**
   * @brief Create a default TerminalMode object. Sets the terminal driver in raw mode
   * @throws std::system_error An exception describing what went wrong
   *
   */
  explicit TerminalMode();

  /**
   * @brief Destructor. Sets the terminal driver back in canonical mode
   *
   */
  ~TerminalMode();

  TerminalMode(TerminalMode const&) = delete;
  TerminalMode& operator=(TerminalMode const&) = delete;
  TerminalMode(TerminalMode&&) = delete;
  TerminalMode& operator=(TerminalMode&&) = delete;

  /**
   * @brief Set the terminal driver to raw (or non-canonical) mode
   *
   */
  void setRawMode() &;

  /**
   * @brief Set the terminal driver to canonical mode
   *
   */
  void reset() &;

private:
  enum class ttystate
  {
    Raw,
    Reset
  };

  termios m_termios {};
  termios m_copy {};
  ttystate m_state {ttystate::Reset};
};

namespace detail {

/// Query fd and write its settings to buf
/// \param[in] fd The file descriptor to be queried
/// \param[in] buf Where the settings are written to
/// \throws std::system_error on failure
void getTerminalDriverSettings(int fd, termios& buf);

/// Set the terminal driver in raw mode
/// \param[in] fd The terminal driver's file descriptor
/// \param[in] buf The buffer to which the terminal driver's settings are to be written
/// \param[in] copy A copy of the settings stored in buf in case we need to roll back
void ttyRaw(int fd, termios const& buf, termios& copy);

/// Set the terminal driver in canonical mode
/// \param[in] fd The terminal driver's file descriptor
/// \param[in] buf The buffer from which the desired settings are to be read from
void ttyReset(int fd, termios const& buf);

}   // namespace detail

}   // namespace Kilo::Terminal

#endif

