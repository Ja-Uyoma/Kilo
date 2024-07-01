/** 
 * MIT License
 * Copyright (c) 2023 Jimmy Givans
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "AppendBuffer.hpp"

#include <filesystem>

namespace Kilo::Editor
{
    /// @brief Process the results from readKey
    /// @details This function is responsible for mapping keypresses to editor operations
    /// @throws std::system_error if an error occured during read
    void processKeypress();

    /// @brief Clear the screen and reposition the cursor to the top-left corner
    void refreshScreen() noexcept;

    /// @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
    void drawRows(AppendBuffer& buffer) noexcept;

    /**
     * @brief Move the cursor in accordance with the key pressed
     * 
     * @param key The character representing the direction to move the cursor in
     */
    void moveCursor(int key);

    /**
     * @brief Open a file for reading and write its contents to the EditorConfig instance's row member variable
     * 
     * @param[in] path The path to the file to be opened for reading
     * @return True if the file was opened successfully and false otherwise
     */
    bool open(std::filesystem::path const& path);

    /**
     * @brief Position the cursor within the visible window
     * 
     */
    void scroll();
}