#include "TerminalSettings.hpp"

#include <unistd.h>

namespace Kilo
{
    /**
     * @brief Enable raw mode by turning off some terminal flags
     * 
     * @param[in] canonicalSettings The original settings of the terminal driver
     * @pre canonicalSettings must be a valid reference
    */
    void enableRawMode(termios& canonicalSettings)
    {
        ::tcgetattr(STDIN_FILENO, &canonicalSettings);

        // Copy the current terminal settings into another variable
        termios temp = canonicalSettings;

        // Modify the necessary settings
        temp.c_lflag &= ~ECHO;

        // Write the new settings to the terminal driver
        ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &temp);
    }
}