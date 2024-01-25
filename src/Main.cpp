#include "TerminalSettings.hpp"

#include <termios.h>
#include <unistd.h>
#include <cerrno>
#include <system_error>
#include <iostream>
#include <cctype>

namespace Kilo
{
    /// @brief Read a character from the terminal and respond in kind
    /// @throws std::system_error if we could not read a character from STDIN
    void Main()
    {
        termios canonicalSettings {};
        enableRawMode(canonicalSettings);

        for ( ; ; ) {
            char c {};

            if (::read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) {
                throw std::system_error(errno, std::generic_category());
            }

        #ifdef DEBUG
            if (std::iscntrl(c)) {
                std::clog << static_cast<int>(c) << "\r\n";
            }
            else {
                std::clog << static_cast<int>(c) << " (" << c << ")\r\n";
            }
        #endif

            if (c == 'q') {
                break;
            }
        }

        disableRawMode(canonicalSettings);
    }
} // namespace Kilo
