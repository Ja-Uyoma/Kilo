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

#include "Cursor/Cursor.hpp"
#include "Offset/Offset.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"
#include "Terminal/Window/Window.hpp"
#include "Utilities/Constants.hpp"
#include <string_view>

#include <filesystem>
#include <vector>

namespace Kilo::editor {

struct EditorConfig
{
  Terminal::Window window;
  Cursor cursor;
  Offset offset;
  ScreenBuffer screenBuffer;
  std::vector<std::string> openDoc;
  std::vector<std::string> renderedDoc;
};

/*
 * \brief Move the cursor in the open document depending on the key pressed
 * \param[in] keyPressed The key pressed by the user
 * \param[in] editor The current state of the editor
 */
void processKeypress(int keyPressed, EditorConfig& editorConfig);

/*
 * \brief Perform a screen refresh
 * \param[in] editor The current editor configuration
 */
void refreshScreen(EditorConfig& editor);

/*
 * \brief Draw each row of the buffer of text being edited, plus a tilde at the beginning, or the welcome message
 * \param[in] editor The editor configuration
 */
void drawRows(EditorConfig& editor);

/*
 * \brief Move the cursor in the direction of the key pressed
 * \param[in] key The key pressed by the user
 * \param[in] editor The current state of the editor
 */
void moveCursor(EditorKey key, EditorConfig& editor);

/*
 * \brief Fix the cursor in the visible window while scrolling
 * \param[in] editor The current state of the editor
 */
void scroll(EditorConfig& editor);

/*
 * \brief Open a file and write its contents to memory
 *
 * \param[in] path The path to the file
 * \param[in] document The buffer containing the file in memory
 * \param[in] rendered The document that is actually rendered to the window
 * \return true If the operation was successful, and false otherwise
 */
auto open(std::filesystem::path const& path, std::vector<std::string>& document, std::vector<std::string>& rendered)
  -> bool;

/*
 * \brief Copies the contents of the source string into the destination string
 * \param[in] row The source string
 * \param[in] render The destination string
 */
void updateRow(std::string_view row, std::string& render);
} // namespace Kilo::editor

namespace Kilo::editor::detail {

/*
 * \brief Write the welcome message to the screen buffer
 *
 * \param[in] windowWidth The width of the window in which the message is to be displayed
 * \param[in] buffer The buffer to which the message is written before being displayed
 */
void printWelcomeMessage(int32_t windowWidth, ScreenBuffer& buffer);

/*
 * \brief Print a line of text from the open document to the screen
 *
 * \param[in] line The line to be printed
 * \param[in] buffer The screen buffer
 * \param[in] windowWidth The width of the terminal window
 * \param[in] columnOffset The column offset between the terminal window width and the document width
 * \pre The column offset must be non-negative
 */
void printLineOfDocument(std::string const& line, ScreenBuffer& buffer, int32_t windowWidth, int64_t columnOffset);

} // namespace Kilo::editor::detail

#endif
