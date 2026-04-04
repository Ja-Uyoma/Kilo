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

#ifndef KILO_EDITOR_APPEND_BUFFER_HPP
#define KILO_EDITOR_APPEND_BUFFER_HPP

#include "kilo/io/File.hpp"
#include <string_view>

#include <cstddef>
#include <cstdint>
#include <string>

namespace kilo::editor::append_buffer {

/**
 * @class append_buffer
 * @brief A buffer to which writes to the screen are written before being flushed out at the end
 * @details In order to avoid making multiple ::write() calls any time we need to refresh the screen, we will do one big
 * ::write() at the end to make sure the entire screen updates at once. This is accomplished by the use of a buffer to
 * which strings will be appended, and then this buffer will be written out at the end.
 */
class append_buffer final
{
public:
  /**
   * @brief Create a default append_buffer instance
   */
  explicit constexpr append_buffer() noexcept = default;

  /**
   * @brief Append the given string to the string buffer
   * @param[in] str The string to be appended to the string buffer
   * @returns A reference to the append_buffer object
   */
  constexpr auto write(std::string_view str) & noexcept(false) -> append_buffer&
  {
    m_buffer.append(str);
    return *this;
  }

  /**
   * @brief Get the size of the buffer
   * @returns The size of the buffer
   */
  [[nodiscard]] constexpr auto size() const& noexcept -> std::size_t
  {
    return m_buffer.length();
  }

  /**
   * @brief Get the size of the buffer
   * @returns The size of the buffer as a signed value
   */
  [[nodiscard]] constexpr auto ssize() const& noexcept -> int64_t
  {
    return std::ssize(m_buffer);
  }

  /**
   * @brief Get a  C-string representation of the buffer
   * @returns A constant C-string representation of the buffer
   */
  [[nodiscard]] constexpr auto c_str() const& noexcept -> char const*
  {
    return m_buffer.c_str();
  }

  /**
   * @brief Flush the buffer by writing its contents to a file
   * @param[in] file The file being written to
   * @returns The number of bytes written
   * @throws `std::system_error` if the operation failed
   */
  auto flush(io::file_interface& file) const& noexcept(false) -> std::size_t;

private:
  std::string m_buffer;
};
}   // namespace kilo::editor::append_buffer

#endif
