namespace Kilo
{
    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void clearScreenAndRepositionCursor() noexcept;

    /// @brief Map characters to control keys
    /// @param key The ASCII character to be mapped to a control key
    /// @return A control key
    constexpr unsigned ctrlKey(unsigned char key) noexcept
    {
        return key &= 0x1f;
    }
} // namespace Kilo
