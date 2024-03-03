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
        while (true) {
            Editor::refreshScreen();
            Editor::processKeypress();
        }
    }
} // namespace Kilo
