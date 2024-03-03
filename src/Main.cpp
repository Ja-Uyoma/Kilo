#include "TerminalSettings.hpp"
#include "Editor.hpp"
#include "Utilities.hpp"

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
                Terminal::enableRawMode(m_settings);
            }

            ~TerminalSettings()
            {
                Terminal::disableRawMode(m_settings);
            }

            TerminalSettings(TerminalSettings const&) = delete;
            TerminalSettings(TerminalSettings&&) = delete;

            TerminalSettings& operator=(TerminalSettings const&) = delete;
            TerminalSettings& operator=(TerminalSettings&&) = delete;
        };

        static TerminalSettings settings {};
        
        while (true) {
            Editor::refreshScreen();
            Editor::processKeypress();
        }
    }
} // namespace Kilo
