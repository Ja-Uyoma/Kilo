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

#ifndef KILO_TERMINAL_TERMINAL_MODE_HPP
#define KILO_TERMINAL_TERMINAL_MODE_HPP

#include <cstdint>
#include <termios.h>

namespace kilo::terminal::inline v1 {

/**
 * @class terminal_mode
 *
 * @brief Handles setting the underlying terminal driver to raw or canonical mode as appropriate
 */
class terminal_mode final
{
public:
  /**
   * @enum tty_mode
   * @brief Describes the possible modes the terminal driver can be in
   */
  enum class tty_mode : std::uint8_t
  {
    raw,
    canonical
  };

  /**
   * @brief Create a default terminal mode instance
   *
   * @details Queries the underlying terminal driver's settings and writes them to a class' member variable
   */
  explicit terminal_mode() noexcept(false);

  /**
   * @brief Destructor.
   * Sets the terminal driver back in canonical mode
   *
   * @details Attempts to set the terminal driver's mode back to canonical mode by calling set_canonical_mode
   * If this fails, the thrown exception is caught, and std::terminate() is called
   */
  ~terminal_mode() noexcept;

  terminal_mode(terminal_mode const&) = delete;
  auto operator=(terminal_mode const&) -> terminal_mode& = delete;
  terminal_mode(terminal_mode&&) = delete;
  auto operator=(terminal_mode&&) -> terminal_mode& = delete;

  /**
   * @brief Set the terminal driver to raw (or non-canonical) mode
   * @pre The terminal driver should be in canonical mode
   * @post The terminal dirver will be in raw mode on success, or reset to canonical mode on failure
   * @throws std::system_error if the operation failed
   */
  void set_raw_mode() & noexcept(false);

  /**
   * @brief Set the terminal driver's mode to canonical mode
   * @pre The terminal driver should be in raw mode
   * @post The terminal driver will be in canonical mode
   */
  void set_canonical_mode() & noexcept(false);

  /**
   * @brief Get the current mode of the terminal driver
   * @returns tty_mode::raw if it's in raw mode, and tty_mode::canonical if it's in canonical mode
   */
  [[nodiscard]] constexpr auto get_mode() const& noexcept -> tty_mode
  {
    return m_mode;
  }

private:
  termios m_termios {};
  termios m_copy {};
  tty_mode m_mode {tty_mode::canonical};
};

namespace detail {

/**
 * @brief Query file_descriptor and write its settings to buf
 * @param[in] file_descriptor The file descriptor to be queried
 * @param[in] buf Where the settings are written to
 * @throws std::system_error on failure
 */
void get_terminal_driver_settings(int file_descriptor, termios& buf) noexcept(false);

/**
 * @brief Set the terminal driver's mode to raw mode
 * @param[in] file_descriptor The terminal driver's file descriptor
 * @param[in] buf The buffer to which the terminal driver's settings are to be written
 * @param[in] copy A copy of the settings stored in buf in case we need to roll back
 * @throws std::system_error if the operation failed
 */
void tty_raw(int file_descriptor, termios const& buf, termios& copy) noexcept(false);

/**
 * @brief Set the terminal driver's mode to canonical mode
 * @param[in] file_descriptor The terminal driver's file descriptor
 * @param[in] buf The buffer from which the desired settings are to be read from
 * @throws std::system_error if the operation failed
 */
void tty_canonical_mode(int file_descriptor, termios const& buf) noexcept(false);

}   // namespace detail

}   // namespace kilo::terminal::inline v1

#endif
