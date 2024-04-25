#ifndef APPEND_BUFFER_HPP
#define APPEND_BUFFER_HPP

#include <string>

namespace Kilo::AppendBuffer
{
    // In order to avoid making multiple ::write() calls anytime we need to refresh the screen,
    // we will do one big ::write() at the end to make sure the entire screen updates at once.
    // This is accomplished by the use of a buffer to which strings will be appended, and then
    // this buffer will be written out at the end.
    
    class AppendBuffer
    {
    public:
        /// @brief Append the given C-string to the buffer
        /// @param[in] str The string to be appended to the buffer
        /// @param[in] length The length of the string
        constexpr void append(char const* str, std::size_t length)
        {
            m_buffer.append(str, length);
        }

    private:
        std::string m_buffer;
    };

    /// @brief Append the given C-string to the buffer
    /// @param[in] buffer The buffer to be appended to
    /// @param[in] str The string to be appended to the buffer
    /// @param[in] length The length of the string
    constexpr void abAppend(AppendBuffer& buffer, char const* str, std::size_t length)
    {
        buffer.append(str, length);
    }
}

#endif