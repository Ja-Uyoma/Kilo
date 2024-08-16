#ifndef WRITE_BUFFER_HPP
#define WRITE_BUFFER_HPP

#include <string>

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
  constexpr void write(char const* str, std::size_t length)
  {
    m_buffer.append(str, length);
  }

  /**
   * @brief Append the given string to the string buffer
   *
   * @param[in] str The string to be appended to the string buffer
   */
  constexpr void write(std::string const& str)
  {
    m_buffer.append(str);
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

private:
  std::string m_buffer;
};
}   // namespace Kilo::editor

#endif
