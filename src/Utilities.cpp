#include "Utilities.hpp"
#include <unistd.h>

namespace Kilo
{
    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void clearScreenAndRepositionCursor() noexcept
    {
        ::write(STDOUT_FILENO, "\x1b[2J", 4);
        ::write(STDOUT_FILENO, "\x1b[H", 3);
    }
} // namespace Kilo
