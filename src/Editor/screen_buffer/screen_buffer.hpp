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

#ifndef WRITE_BUFFER_HPP
#define WRITE_BUFFER_HPP

#include "Terminal/File.hpp"
#include <string>
#include <string_view>

namespace Kilo::editor {
// In order to avoid making multiple ::write() calls anytime we need to refresh
// the screen, we will do one big ::write() at the end to make sure the entire
// screen updates at once. This is accomplished by the use of a buffer to which
// strings will be appended, and then this buffer will be written out at the
// end.

class ScreenBuffer
{
public:
  explicit constexpr ScreenBuffer() noexcept = default;

  /// @brief Append the given C-string to the buffer
  /// @param[in] str The string to be appended to the buffer
  /// @param[in] length The length of the string
  constexpr ScreenBuffer& write(char const* str, std::size_t length)
  {
    m_buffer.append(str, length);
    return *this;
  }

  /**
   * @brief Append the given string to the string buffer
   *
   * @param[in] str The string to be appended to the string buffer
   */
  constexpr ScreenBuffer& write(std::string_view str)
  {
    m_buffer.append(str);
    return *this;
  }

  /// @brief Get the size of the buffer
  /// @returns The size of the buffer
  constexpr std::size_t size() const noexcept
  {
    return m_buffer.length();
  }

  /// @brief Get a  C-string representation of the buffer
  /// @returns A constant C-string representation of the buffer
  constexpr char const* c_str() const noexcept
  {
    return m_buffer.c_str();
  }

  /// \brief Flush the buffer by writing its contents to a file
  /// \param[in] file The file being written to
  /// \returns The number of bytes written
  /// \throws `std::system_error` if the operation failed
  std::size_t flush(terminal::File& file) const;

private:
  std::string m_buffer;
};
}   // namespace Kilo::editor

#endif
