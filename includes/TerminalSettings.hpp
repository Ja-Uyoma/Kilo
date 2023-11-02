#ifndef TERMINAL_SETTINGS_HPP
#define TERMINAL_SETTINGS_HPP

#include <termios.h>

namespace Kilo
{
    /**
     * @brief Enable raw mode by turning off some terminal flags
     * 
     * @param[in] canonicalSettings The original settings of the terminal driver
     * @pre canonicalSettings must be a valid reference
    */
    void enableRawMode(termios& canonicalSettings);
}

#endif