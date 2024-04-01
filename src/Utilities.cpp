#include "Utilities.hpp"

#include <unistd.h>
#include <cerrno>
#include <system_error>
#include <cstring>

namespace Kilo::Utilities
{
    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void clearScreenAndRepositionCursor() noexcept
    {
        ::write(STDOUT_FILENO, "\x1b[2J", 4);
        ::write(STDOUT_FILENO, "\x1b[H", 3);
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
    long writeAll(int fd, void const* buf, long count)
    {
        long totalWritten {};
        auto const* ptr = static_cast<char const*>(buf);

        while (totalWritten < count) {
            long written = ::write(fd, ptr + totalWritten, count - totalWritten);

            if (written == -1) {
                if (errno == EINTR || errno == EAGAIN) {
                    continue;
                }
                else {
                    throw std::system_error(errno, std::generic_category(), std::strerror(errno));
                }
            }

            if (written == 0) {
                break;
            }

            totalWritten += written;
        }

        return totalWritten;
    }
} // namespace Kilo
