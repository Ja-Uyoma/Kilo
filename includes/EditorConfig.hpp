#ifndef EDITOR_CONFIG_HPP
#define EDITOR_CONFIG_HPP

#include <termios.h>

#include <string>

namespace Kilo::Editor
{
    class EditorConfig
    {
    public:
        termios origTermios;
        int screenRows;
        int screenCols;
        int cursorX {};    // The horizontal coordinate of the cursor (the column)
        int cursorY {};    // The vertical coordinate of the cursor (the row)
        std::string row;
        int numrows {1};

        explicit EditorConfig();
        ~EditorConfig();

        EditorConfig(EditorConfig const&) = delete;
        EditorConfig& operator=(EditorConfig const&) = delete;

        EditorConfig(EditorConfig&&) noexcept = delete;
        EditorConfig operator=(EditorConfig&&) noexcept = delete;
    };
}

#endif