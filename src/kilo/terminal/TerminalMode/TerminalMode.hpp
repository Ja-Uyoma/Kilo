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

#include <cstdint>
#include <termios.h>

namespace kilo::terminal {

class terminal_mode final
{
public:
  enum class tty_mode : std::uint8_t
  {
    raw,
    canonical
  };

  ///
  /// \brief Default constructor
  ///
  explicit terminal_mode();

  ///
  /// \brief Destructor
  ///
  ~terminal_mode();

  terminal_mode(terminal_mode const&) = delete;
  auto operator=(terminal_mode const&) -> terminal_mode& = delete;
  terminal_mode(terminal_mode&&) = delete;
  auto operator=(terminal_mode&&) -> terminal_mode& = delete;

  ///
  /// \brief Set the terminal driver to raw (or non-canonical) mode
  /// \throws std::system_error on failure
  ///
  void set_raw_mode() &;

  ///
  /// \brief Set the terminal driver to canonical mode
  ///
  void set_canonical_mode() &;

  ///
  /// \brief Get the current mode of the terminal
  /// \returns tty_mode::raw if the terminal is in raw mode, tty_mode::canonical otherwise
  ///
  [[nodiscard]]
  constexpr auto get_mode() const noexcept -> tty_mode
  {
    return m_mode;
  }

private:
  termios m_termios {};
  termios m_copy {};
  tty_mode m_mode {tty_mode::canonical};
};

namespace detail {

///
/// \brief Query file_descriptor and write its settings to buf
/// \param[in] file_descriptor The file descriptor to be queried
/// \param[in] buf Where the settings are written to
/// \throws std::system_error on failure
///
void get_terminal_driver_settings(int file_descriptor, termios& buf);

///
/// \brief Set the terminal driver in raw mode
/// \param[in] file_descriptor The terminal driver's file descriptor
/// \param[in] buf The buffer to which the terminal driver's settings are to be written
/// \param[in] copy A copy of the settings stored in buf in case we need to roll back
///
void tty_raw(int file_descriptor, termios const& buf, termios& copy);

///
/// \brief Set the terminal driver in canonical mode
/// \param[in] file_descriptor The terminal driver's file descriptor
/// \param[in] buf The buffer from which the desired settings are to be read from
///
void tty_canonical_mode(int file_descriptor, termios const& buf);

}   // namespace detail

}   // namespace kilo::terminal

#endif
