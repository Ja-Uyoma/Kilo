#include <termios.h>

namespace Kilo::Editor
{
    class EditorConfig
    {
    public:
        explicit EditorConfig();
        ~EditorConfig();

        EditorConfig(EditorConfig const&) = delete;
        EditorConfig& operator=(EditorConfig const&) = delete;

        EditorConfig(EditorConfig&&) noexcept = delete;
        EditorConfig operator=(EditorConfig&&) noexcept = delete;

    private:
        termios m_origTermios;
        int m_screenRows;
        int m_screenCols;
    };

    /// @brief Process the results from readKey
    /// @details This function is responsible for mapping keypresses to editor operations
    /// @throws std::system_error if an error occured during read
    void processKeypress();

    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void refreshScreen();

    /// @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
    void drawRows() noexcept;
}