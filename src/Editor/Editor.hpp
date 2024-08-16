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
#include "Utilities/Utilities.hpp"
#include "Window/Window.hpp"
#include "WriteBuffer/WriteBuffer.hpp"
#include <cassert>
#include <filesystem>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace Kilo::editor {

/// @brief Process the results from readKey
/// @details This function is responsible for mapping keypresses to editor
/// operations
/// @throws std::system_error if an error occured during read
void processKeypress();

/// @brief Clear the screen and reposition the cursor to the top-left corner
void refreshScreen() noexcept;

/// @brief Draw each row of the buffer of text being edited, plus a tilde at the
/// beginning
void drawRows(WriteBuffer& buffer) noexcept;

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
 * @brief Get the current row at which the cursor is located
 *
 * @param cursor The cursor object
 * @param rows The rows of text of the document in memory
 * @return std::optional<std::string> The current row at which the cursor is located
 */
constexpr std::optional<std::string> getCurrentRow(cursor::Cursor const& cursor,
                                                   std::vector<std::string> const& rows) noexcept
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
constexpr void moveCursor(cursor::Cursor& cursor,
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
      auto currentRow = editor::getCurrentRow(cursor, document);

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
    default: return;
  }
}

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
 * @brief Create a welcome message by interpolating two strings
 *
 * @param versionString The version of the application
 * @return std::string The welcome message
 */
std::string createWelcomeMessage(std::string_view versionString) noexcept;

void displayWelcomeMessage(WriteBuffer& buffer);

}   // namespace Kilo::editor::detail

#endif
