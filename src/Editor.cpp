#include "Editor.hpp"
#include "Utilities.hpp"
#include "TerminalSettings.hpp"

#include <system_error>
#include <unistd.h>
#include <cstdlib>

namespace Kilo::Editor 
{
    EditorConfig::EditorConfig()
    {
        enableRawMode();
    }

    EditorConfig::~EditorConfig()
    {
        disableRawMode();
    }

    /// @brief Set the terminal in raw mode
    void EditorConfig::enableRawMode() &
    {
        Terminal::enableRawMode(m_origTermios);
    }

    /// @brief Set the terminal in canonical mode
    void EditorConfig::disableRawMode() const& 
    {
        Terminal::disableRawMode(m_origTermios);
    }
    
    /// @brief Read key input from stdin
    /// @return The character read
    /// @throws std::system_error if an error occured during read
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

    /// @brief Process the results from readKey
    /// @details This function is responsible for mapping keypresses to editor operations
    /// @throws std::system_error if an error occured during read
    void processKeypress() noexcept(noexcept(readKey()))
    {
        char c = readKey();

        switch (c) {
            case ctrlKey('q'):
                clearScreenAndRepositionCursor();
                std::exit(EXIT_SUCCESS);
                break;
            default:
                return;
        }
    }

    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void refreshScreen()
    {
        clearScreenAndRepositionCursor();
        drawRows();
        ::write(STDOUT_FILENO, "\x1b[H", 3);
    }

    /// @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
    void drawRows() noexcept
    {
        for (int y = 0; y < 24; ++y) {
            ::write(STDOUT_FILENO, "~\r\n", 3);
        }
    }
}