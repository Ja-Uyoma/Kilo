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
#include "Terminal/window/window.hpp"
#include <cassert>
#include <filesystem>
#include <optional>
#include <string_view>
#include <utility>
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
void processKeypress(int const keyPressed, Cursor& cursor, Terminal::Window const& window,
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
bool open(std::filesystem::path const& path, std::vector<std::string>& document, std::vector<std::string>& rendered);

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
 * @brief Specify the exact position we want the cursor to move to
 *
 * @param cursor The current position of the cursor
 * @param offset The offset from the terminal window to the currently-open document
 * @return std::string
 */
std::string setExactPositionToMoveCursorTo(Cursor const& cursor, Offset const& offset);

/**
 * @brief Create a welcome message by interpolating two strings
 *
 * @param versionString The version of the application
 * @return std::string The welcome message
 */
std::string createWelcomeMessage(std::string_view versionString) noexcept;

/**
 * @brief Resizes the message string to be equal to the window width if it exceeds it
 *
 * @param message The message string to be resized
 * @param windowWidth The unit determining how much the string should be resized
 * @pre windowWidth must be >= 0
 */
void resizeWelcomeMessage(std::string& message, int windowWidth) noexcept;

/**
 * @brief Determine how far from the left edge of the screen we should start printing the welcome message
 *
 * @param windowWidth The width of the window
 * @param msgLength The length of the welcome message
 * @return constexpr std::size_t The padding between the left edge of the screen and the welcome message
 */
constexpr long getPadding(int windowWidth, std::size_t msgLength) noexcept
{
  assert(windowWidth >= 0 and "Window width cannot be negative");

  return (windowWidth - msgLength) / 2;
}

/**
 * @brief Write padding characters to the screen buffer
 *
 * @param padding The number of characters to be written to the screen buffer
 * @param buf The screen buffer being written to
 */
void writePaddingToScreenBuffer(long padding, ScreenBuffer& buf);

/**
 * @brief Write the welcome message to the screen buffer
 *
 * @param windowWidth The width of the window in which the message is to be displayed
 * @param buffer The buffer to which the message is written before being displayed
 */
void printWelcomeMessage(int windowWidth, ScreenBuffer& buffer);

/**
 * @brief Get the current row at which the cursor is located
 *
 * @param cursorY The position of the cursor along the y-axis
 * @param rows The rows of text of the document in memory
 * @return std::optional<std::string> The current row at which the cursor is located
 */
constexpr std::optional<std::string> getCurrentRow(int const cursorY, std::vector<std::string> const& rows) noexcept
{
  assert(cursorY >= 0 and "cursorY must be non-negative");

  if (std::cmp_greater_equal(cursorY, rows.size())) {
    return std::nullopt;
  }
  else {
    return std::make_optional(rows[cursorY]);
  }
}

/**
 * @brief Move cursor in document according to the key pressed
 *
 * @param cursor The cursor to be moved
 * @param keyPressed The key press determining how the cursor is to be moved
 * @param document The document within which the cursor is located
 */
constexpr void moveCursorHelper(Cursor& cursor, editor::EditorKey const& keyPressed,
                                std::vector<std::string> const& document) noexcept
{
  switch (keyPressed) {
    using enum editor::EditorKey;

    case ArrowLeft:
      if (cursor.x != 0) {
        cursor.x--;
      }
      else if (cursor.y > 0) {
        cursor.y--;
        cursor.x = (int)document[cursor.y].size();
      }
      break;
    case ArrowRight: {
      auto currentRow = detail::getCurrentRow(cursor.y, document);

      if (currentRow && std::cmp_less(cursor.x, currentRow->size())) {
        cursor.x++;
      }
      else if (currentRow && std::cmp_equal(cursor.x, currentRow->size())) {
        cursor.y++;
        cursor.x = 0;
      }
      break;
    }
    case ArrowUp:
      if (cursor.y != 0) {
        cursor.y--;
      }
      break;
    case ArrowDown:
      if (std::cmp_less(cursor.y, document.size())) {
        cursor.y++;
      }
      break;
    default:
      return;
  }
}

/**
 * @brief Check if the cursor has moved outside of the visible window and adjust the offset to fix it if so
 *
 * @param cursorPos The current position of the cursor
 * @param offOf An offset from the window into the open document
 * @param windowDimension A dimension of the terminal window
 */
constexpr void fixCursorToVisibleWindow(int const cursorPos, int& offOf, int const windowDimension) noexcept
{
  if (cursorPos < offOf) {
    offOf = cursorPos;
  }
  else if (cursorPos >= offOf + windowDimension) {
    offOf = cursorPos - windowDimension + 1;
  }

  assert(offOf == cursorPos or offOf == cursorPos - windowDimension + 1);
};

/**
 * @brief Perform an editor operation depending on the key pressed
 *
 * @param[in] keyPressed The key pressed by the user
 */
void processKeypressHelper(unsigned const keyPressed) noexcept;

/**
 * @brief Perform an editor operation depending on the key pressed
 *
 * @param[in] keyPressed The key pressed by the user
 * @param[in] cursor The position of the cursor in the terminal window
 * @param[in] window The terminal window
 */
void processKeypressHelper(editor::EditorKey keyPressed, Cursor& cursor, Terminal::Window const& window,
                           std::vector<std::string> const& document) noexcept;

/**
 * @brief Print the welcome message or a tilde to the window
 *
 * @param documentIsEmpty A boolean value representing whether the document is empty or not
 * @param currentRow The row we are currently scrolled to in the open document
 * @param buffer The ScreenBuffer
 * @param window The terminal window
 */
void printWelcomeMessageOrTilde(bool documentIsEmpty, int currentRow, ScreenBuffer& buffer,
                                Terminal::Window const& window);

/**
 * @brief Print a line of text from the open document to the screen
 *
 * @param line The line to be printed
 * @param buffer The screen buffer
 * @param windowWidth The width of the terminal window
 * @param columnOffset The column offset between the terminal window width and the document width
 * @pre The column offset must be non-negative
 */
void printLineOfDocument(std::string const& line, ScreenBuffer& buffer, int const windowWidth, int const columnOffset);

}   // namespace Kilo::editor::detail

#endif
