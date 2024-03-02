#include "TerminalSettings.hpp"
#include "Editor.hpp"

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
    [[noreturn]] void Main()
    {
        // We need to call disableRawMode at program exit to reset the terminal to its canonical settings
        // We cannot register an atexit handler for this because atexit doesn't accept functions
        // that take parameters.
        // Therefore, the solution is to use RAII with a static object whose resources must be cleaned up
        // at program exit
        class TerminalSettings
        {
        public:
            termios m_settings {};

            TerminalSettings()
            {
                enableRawMode(m_settings);
            }

            ~TerminalSettings()
            {
                disableRawMode(m_settings);
            }

            TerminalSettings(TerminalSettings const&) = delete;
            TerminalSettings(TerminalSettings&&) = delete;

            TerminalSettings& operator=(TerminalSettings const&) = delete;
            TerminalSettings& operator=(TerminalSettings&&) = delete;
        };

        static TerminalSettings settings {};
        
        while (true) {
            Editor::processKeypress();
        }
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
