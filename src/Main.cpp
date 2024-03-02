#include "TerminalSettings.hpp"

#include <termios.h>
#include <unistd.h>
#include <cerrno>
#include <system_error>
#include <iostream>
#include <cctype>

namespace Kilo
{
    /// @brief Map characters to control keys
    /// @param key The ASCII character to be mapped to a control key
    /// @return A control key
    constexpr unsigned ctrlKey(unsigned char key) noexcept
    {
        return key &= 0x1f;
    }

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

            if (c == ctrlKey('q')) {
                break;
            }
        }

        disableRawMode(canonicalSettings);
    }

    namespace Editor 
    {
        char readKey()
        {
            char c {};

            for (long nread = 0; nread != 1; nread = ::read(STDIN_FILENO, &c, 1)) {
                if (nread == -1 && errno != EAGAIN) {
                    throw std::system_error(errno, std::generic_category());
                }

                errno = 0;
            }

            return c;
        }

        void processKeypress() noexcept(noexcept(readKey()))
        {
            char c = readKey();

            switch (c) {
                case ctrlKey('q'):
                    std::exit(0);
                    break;
                default:
                    return;
            }
        }
    }
} // namespace Kilo
