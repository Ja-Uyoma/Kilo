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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "Constants/Constants.hpp"
#include "Cursor/Cursor.hpp"
#include "Offset/Offset.hpp"
#include "Terminal/Window/Window.hpp"
#include <string_view>

#include <cassert>
#include <filesystem>
#include <vector>

namespace Kilo::editor {

/*
 * Forward declaration to the ScreenBuffer class
 */
class ScreenBuffer;

/**
 * @brief Performs an action depending on the key pressed
 *
 * @param[in] keyPressed The key pressed by the user
 * @param[in] cursor The position of the cursor in the terminal window
 * @param[in] window The terminal window
 */
void processKeypress(int keyPressed, Cursor& cursor, Terminal::Window const& window,
                     std::vector<std::string> const& document) noexcept;

/**
 * @brief Perform a screen refresh
 *
 * @details Fit the cursor within the visible window and draw each row of the buffer of text being edited together with
 * the tildes
 * @param buffer The screen buffer
 * @param cursor The cursor
 * @param offset The offset from the window to the open document
 */
void refreshScreen(ScreenBuffer& buffer, Cursor const& cursor, Offset const& offset, Terminal::Window const& window,
                   std::vector<std::string> const& document, std::vector<std::string> const& renderedDoc);

/**
 * @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
 *
 * @param window The terminal window
 * @param offset The offset from the terminal window to the document
 * @param doc The document being edited
 * @param buffer The screen buffer
 * @param renderedDoc The version of the document being edited that is actually rendered
 */
void drawRows(Terminal::Window const& window, Offset const& offset, std::vector<std::string> const& doc,
              ScreenBuffer& buffer, std::vector<std::string> const& renderedDoc);

/**
 * @brief Move the cursor in the direction of the key pressed
 *
 * @param key The key pressed
 * @param cursor The editor cursor
 * @param row The document which is currently open
 */
void moveCursor(editor::EditorKey key, Cursor& cursor, std::vector<std::string> const& row);

/**
 * @brief Open a file and write its contents to memory
 *
 * @param[in] path The path to the file
 * @param[in] document The buffer containing the file in memory
 * @param[in] rendered The document that is actually rendered to the window
 * @return true If the operation was successful
 * @return false If the operation failed
 */
auto open(std::filesystem::path const& path, std::vector<std::string>& document, std::vector<std::string>& rendered)
  -> bool;

/**
 * @brief Fit the cursor in the visible window
 *
 * @param[in] cursor The cursor
 * @param[in] offset The position the user is currently scrolled to in the document
 * @param[in] window The terminal window
 */
void scroll(Cursor const& cursor, Offset& offset, Terminal::Window const& window) noexcept;

/**
 * @brief Copies the contents of the source string into the destination string
 * @param[in] row The source string
 * @param[in] render The destination string
 */
void updateRow(std::string_view row, std::string& render);
}   // namespace Kilo::editor

namespace Kilo::editor::detail {

/**
 * @brief Write the welcome message to the screen buffer
 *
 * @param windowWidth The width of the window in which the message is to be displayed
 * @param buffer The buffer to which the message is written before being displayed
 */
void printWelcomeMessage(int windowWidth, ScreenBuffer& buffer);

/**
 * @brief Print a line of text from the open document to the screen
 *
 * @param line The line to be printed
 * @param buffer The screen buffer
 * @param windowWidth The width of the terminal window
 * @param columnOffset The column offset between the terminal window width and the document width
 * @pre The column offset must be non-negative
 */
void printLineOfDocument(std::string const& line, ScreenBuffer& buffer, int windowWidth, int columnOffset);

}   // namespace Kilo::editor::detail

#endif
