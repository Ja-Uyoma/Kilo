namespace Kilo::Editor
{
    /// @brief Read key input from stdin
    /// @return The character read
    /// @throws std::system_error if an error occured during read
    char readKey();

    /// @brief Process the results from readKey
    /// @details This function is responsible for mapping keypresses to editor operations
    /// @throws std::system_error if an error occured during read
    void processKeypress() noexcept(noexcept(readKey()));

    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void refreshScreen();
}