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
        Terminal::enableRawMode(m_origTermios);
        Terminal::getWindowSize(&m_screenRows, &m_screenCols);
    }

    EditorConfig::~EditorConfig()
    {
        Terminal::disableRawMode(m_origTermios);
    }

    /// @brief Process the results from readKey
    /// @details This function is responsible for mapping keypresses to editor operations
    /// @throws std::system_error if an error occured during read
    void processKeypress()
    {
        char c = Terminal::readKey();

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