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

#include "Cursor.hpp"
#include "Offset.hpp"
#include "Utilities/Utilities.hpp"
#include "Window/Window.hpp"
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
void processKeypress(int const keyPressed, Cursor& cursor, terminal::Window const& window) noexcept;

/// @brief Process the results from readKey
/// @details This function is responsible for mapping keypresses to editor
/// operations
/// @throws std::system_error if an error occured during read
void processKeypress();

/// @brief Clear the screen and reposition the cursor to the top-left corner
void refreshScreen();

/**
 * @brief Perform a screen refresh
 *
 * @details Fit the cursor within the visible window and draw each row of the buffer of text being edited together with
 * the tildes
 * @param buffer The screen buffer
 * @param cursor The cursor
 * @param offset The offset from the window to the open document
 */
void refreshScreen(ScreenBuffer& buffer, Cursor const& cursor, Offset const& offset);

/// @brief Draw each row of the buffer of text being edited, plus a tilde at the
/// beginning
void drawRows(ScreenBuffer& buffer) noexcept;

/**
 * @brief Draw each row of the buffer of text being edited, plus a tilde at the beginning
 *
 * @param window The terminal window
 * @param offset The offset from the terminal window to the document
 * @param doc The document being edited
 * @param buffer The screen buffer
 * @param renderedDoc The version of the document being edited that is actually rendered
 */
void drawRows(terminal::Window const& window,
              Offset const& offset,
              std::vector<std::string> const& doc,
              ScreenBuffer& buffer,
              std::vector<std::string> const& renderedDoc);

/**
 * @brief Move the cursor in accordance with the key pressed
 *
 * @param key The character representing the direction to move the cursor in
 */
void moveCursor(utilities::EditorKey key) noexcept;

/**
 * @brief Open a file for reading and write its contents to the EditorConfig
 * instance's row member variable
 *
 * @param[in] path The path to the file to be opened for reading
 * @return True if the file was opened successfully and false otherwise
 */
bool open(std::filesystem::path const& path);

/**
 * @brief Position the cursor within the visible window
 *
 */
void scroll() noexcept;

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
 * @param cursor The cursor object
 * @param rows The rows of text of the document in memory
 * @return std::optional<std::string> The current row at which the cursor is located
 */
constexpr std::optional<std::string> getCurrentRow(Cursor const& cursor, std::vector<std::string> const& rows) noexcept
{
  assert(cursor.x >= 0 and cursor.y >= 0);

  if (std::cmp_greater_equal(cursor.y, rows.size())) {
    return std::nullopt;
  }
  else {
    return std::make_optional(rows[cursor.y]);
  }
}

/**
 * @brief Move cursor in document according to the key pressed
 *
 * @param cursor The cursor to be moved
 * @param keyPressed The key press determining how the cursor is to be moved
 * @param document The document within which the cursor is located
 */
constexpr void moveCursorHelper(Cursor& cursor,
                                utilities::EditorKey const& keyPressed,
                                std::vector<std::string> const& document) noexcept
{
  switch (keyPressed) {
    using enum utilities::EditorKey;

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
      auto currentRow = detail::getCurrentRow(cursor, document);

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
void processKeypressHelper(utilities::EditorKey keyPressed, Cursor& cursor, terminal::Window const& window) noexcept;

/**
 * @brief Print the welcome message or a tilde to the window
 *
 * @param documentIsEmpty A boolean value representing whether the document is empty or not
 * @param currentRow The row we are currently scrolled to in the open document
 * @param buffer The ScreenBuffer
 * @param window The terminal window
 */
void printWelcomeMessageOrTilde(bool documentIsEmpty,
                                int currentRow,
                                ScreenBuffer& buffer,
                                terminal::Window const& window);

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
