#include <cstddef>

namespace Kilo::Utilities
{
    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void clearScreenAndRepositionCursor() noexcept;

    /// @brief Map characters to control keys
    /// @param key The ASCII character to be mapped to a control key
    /// @return A control key
    constexpr unsigned ctrlKey(unsigned char key) noexcept
    {
        return key &= 0x1f;
    }

    /**
     * @brief Write to a file descriptor, with retries in case of partial writes
     * @param[in] fd The file descriptor to write to
     * @param[in] buf The buffer being written from
     * @param[in] count The number of bytes to be written
     * @returns The total number of bytes written
     * @throws std::system_error in case of total write failure
    */
    [[nodiscard]]
    long writeAll(int fd, void const* buf, long count);
} // namespace Kilo
