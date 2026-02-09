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

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "kilo/io/File.hpp"
#include <sys/ioctl.h>

#include <cstdint>
#include <span>

namespace kilo::terminal {

/**
 * @struct window_size
 * @brief Encodes the dimensions of a window
 */
struct window_size
{
  std::int32_t cols;
  std::int32_t rows;
};

/**
 * @class window
 * @brief Manages the creation of a window either by reading the dimensions of the terminal window or by explicitly
 * setting your desired dimensions
 */
class window final
{
public:
  /**
   * @brief Create a default window instance whose dimensions equal those of the currently-open terminal window
   */
  explicit window() noexcept(false);

  /**
   * @brief Create a window with the given dimensions
   * @param[in] windowSize The dimensions of the new Window object
   */
  explicit constexpr window(window_size const& window_size) noexcept : m_winsize(window_size)
  {
  }

  /**
   * @brief Get the columns of the terminal window
   * @returns The columns of the terminal window
   */
  [[nodiscard]] constexpr auto cols() const& noexcept -> std::int32_t
  {
    return m_winsize.cols;
  }

  /**
   * @brief Get the rows of the terminal window
   * @returns The rows of the terminal window
   */
  [[nodiscard]] constexpr auto rows() const& noexcept -> std::int32_t
  {
    return m_winsize.rows;
  }

private:
  window_size m_winsize {};
};

namespace detail {

/**
 * @brief Get the dimensions of the terminal window
 * @details Delegates to get_cursor_position if the window size could not be determined by a call to ioctl
 *
 * @param[in] file The "file" we're performing IO operations on; usually stdin and stdout
 * @param[in] winsz The internal data structure to which the sizes will be written
 * @returns The size of the terminal window
 * @throws std::system_error if the cursor could not be moved to the bottom-right of the terminal window
 * @throws std::system_error if the position of the cursor in the terminal window could not be determined
 * @throws std::runtime_error if the obtained cursor position was ill-formed and could not be parsed
 */
auto get_window_size(io::file_interface& file, winsize& winsz) noexcept(false) -> window_size;

/**
 * @brief Get the position of the cursor in the terminal window
 *
 * @param[in] file The file we're performing IO operations on (usually STDIN and STDOUT)
 * @param[in] buffer The buffer to which the cursor position is written
 * @returns The position of the cursor in the terminal window
 * @throws std::system_error if the position of the cursor in the terminal window could not be determined
 * @throws std::runtime_error if the obtained cursor position was ill-formed and could not be parsed
 */
auto get_cursor_position(io::file_interface& file, std::span<char> buffer) noexcept(false) -> window_size;

}   // namespace detail

}   // namespace kilo::terminal

#endif
