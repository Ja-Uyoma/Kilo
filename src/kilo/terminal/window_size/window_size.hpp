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

#ifndef KILO_TERMINAL_WINDOW_SIZE_WINDOW_SIZE_HPP
#define KILO_TERMINAL_WINDOW_SIZE_WINDOW_SIZE_HPP

#include "kilo/io/File.hpp"
#include "kilo/utilities/Error.hpp"
#include <sys/ioctl.h>

#include <cstdint>
#include <expected>
#include <span>

namespace kilo::terminal::window_size {

/**
 * @struct window_size
 * @brief Encodes the dimensions of a window
 */
struct window_size
{
  std::int32_t cols {};
  std::int32_t rows {};
};

/**
 * @brief Get the size of the open terminal window
 *
 * @returns The size of the terminal window, or an error if it cannot be determined
 * @returns An error if the window size cannot be determined
 * @see detail::get_window_size()
 * @see detail::get_cursor_position()
 */
auto get_terminal_window_size() -> std::expected<window_size, utilities::error_info>;

namespace detail {

/**
 * @brief Get the dimensions of the terminal window
 * @details Delegates to get_cursor_position if the window size could not be determined by a call to ioctl
 *
 * @param[in] file The "file" we're performing IO operations on; usually stdin and stdout
 * @param[in] winsz The internal data structure to which the sizes will be written
 * @returns The size of the terminal window, or an error if it cannot be determined
 * @returns An error if the window size cannot be determined
 */
auto get_window_size(io::file_interface& file, winsize& winsz) -> std::expected<window_size, utilities::error_info>;

/**
 * @brief Get the position of the cursor in the terminal window
 *
 * @param[in] file The file we're performing IO operations on (usually STDIN and STDOUT)
 * @param[in] buffer The buffer to which the cursor position is written
 * @returns The position of the cursor, or an error if it cannot be read or parsed
 * @returns An error if the position could not be read or parsed
 */
auto get_cursor_position(io::file_interface& file, std::span<char> buffer)
  -> std::expected<window_size, utilities::error_info>;

}   // namespace detail

}   // namespace kilo::terminal::window_size

#endif
