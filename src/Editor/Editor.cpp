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

#include "Editor.hpp"

#include "Constants/Constants.hpp"
#include "Cursor/Cursor.hpp"
#include "File/File.hpp"
#include "Offset/Offset.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"
#include "Terminal/Window/Window.hpp"
#include "Utilities/Utilities.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unistd.h>
#include <utility>
#include <vector>

namespace Kilo::editor {

/**
 * @brief Performs an action depending on the key pressed
 *
 * @param[in] keyPressed The key pressed by the user
 * @param[in] cursor The position of the cursor in the terminal window
 * @param[in] window The terminal window
 */
void processKeypress(int const keyPressed, Cursor& cursor, Terminal::Window const& window,
                     std::vector<std::string> const& document) noexcept
{
  using editor::EditorKey;
  using utilities::clearScreenAndRepositionCursor;
  using utilities::ctrlKey;
  using enum editor::EditorKey;

  if (keyPressed == ctrlKey('q')) {
    clearScreenAndRepositionCursor();
    std::exit(EXIT_SUCCESS);
  }

  auto key = static_cast<EditorKey>(keyPressed);

  if (key == Home) {
    cursor.x = 0;
  }
  else if (key == End) {
    cursor.x = window.cols() - 1;
  }
  else if (key == PageUp or key == PageDown) {
    for (auto i = window.rows(); i > 0; i--) {
      moveCursor(key == PageUp ? ArrowUp : ArrowDown, cursor, document);
    }
  }
  else if (key == ArrowLeft or key == ArrowRight or key == ArrowUp or key == ArrowDown) {
    moveCursor(key, cursor, document);
  }
}

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
                   std::vector<std::string> const& document, std::vector<std::string> const& renderedDoc)
{
  /*
   * Hide the cursor when painting and then move it to the home position
   */

  buffer.write(EscapeSequences::HideCursorWhenRepainting).write(EscapeSequences::MoveCursorToHomePosition);

  IO::File output;

  drawRows(window, offset, document, buffer, renderedDoc);

  // We add 1 to cursor.x and cursor.y to convert from 0-indexed values to the
  // 1-indexed values that the terminal uses
  auto const cursorPos = fmt::format("\x1b[{};{}H", (cursor.y - offset.row) + 1, (cursor.x - offset.col) + 1);

  buffer.write(cursorPos).write(EscapeSequences::ShowTheCursor).flush(output);
}

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
              ScreenBuffer& buffer, std::vector<std::string> const& renderedDoc)
{
  for (std::size_t currentRow = 0; std::cmp_less(currentRow, window.rows()); currentRow++) {
    if (auto fileRow = currentRow + offset.row; std::cmp_greater_equal(fileRow, doc.size())) {
      detail::printWelcomeMessageOrTilde(doc.empty(), currentRow, buffer, window);
    }
    else {
      detail::printLineOfDocument(renderedDoc[fileRow], buffer, window.cols(), offset.col);
    }

    buffer.write(EscapeSequences::ErasePartOfLineToTheRightOfCursor);

    if (std::cmp_less(currentRow, window.rows() - 1)) {
      buffer.write("\r\n");
    }
  }
}

/**
 * @brief Move the cursor in the direction of the key pressed
 *
 * @param key The key pressed
 * @param cursor The editor cursor
 * @param document The document which is currently open
 */
void moveCursor(editor::EditorKey key, Cursor& cursor, std::vector<std::string> const& document)
{
  using enum editor::EditorKey;

  switch (key) {
    case ArrowLeft:
      if (cursor.x != 0) {
        cursor.x--;
      }
      else if (cursor.y > 0) {
        cursor.y--;
        cursor.x = std::ssize(document[cursor.y]);
      }
      break;
    case ArrowRight: {
      auto currentRow = std::invoke([cy = cursor.y, &document]() -> std::optional<std::string> {
        if (cy >= std::ssize(document)) {
          return std::nullopt;
        }

        return std::make_optional(document[cy]);
      });

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

  auto currRow = std::invoke([&cursor, &document]() -> std::optional<std::string> {
    if (cursor.y >= std::ssize(document)) {
      return std::nullopt;
    }

    return std::make_optional(document[cursor.y]);
  });

  auto rowlen = currRow ? currRow->length() : 0;

  if (std::cmp_greater_equal(cursor.x, rowlen)) {
    cursor.x = rowlen;
  }
}

/**
 * @brief Open a file and write its contents to memory
 *
 * @param[in] path The path to the file
 * @param[in] document The buffer containing the file in memory
 * @param[in] rendered The document that is actually rendered to the window
 * @return true If the operation was successful
 * @return false If the operation failed
 */
bool open(std::filesystem::path const& path, std::vector<std::string>& document, std::vector<std::string>& rendered)
{
  if (!std::filesystem::is_regular_file(path)) {
    return false;
  }

  std::ifstream infile(path);

  if (!infile) {
    return false;
  }

  std::string line;

  while (std::getline(infile, line)) {
    document.push_back(line);
  }

  rendered = document;

  return true;
}

/**
 * @brief Fit the cursor in the visible window
 *
 * @param[in] cursor The cursor
 * @param[in] offset The position the user is currently scrolled to in the document
 * @param[in] window The terminal window
 */
void scroll(Cursor const& cursor, Offset& offset, Terminal::Window const& window) noexcept
{
  /*
   * Check if the cursor has moved outside of the visible window.
   * If so, adjust editorConfig.off.row and/or editorConfig.off.col so that the
   * cursor is just inside the visible window
   */

  if (cursor.y < offset.row) {
    offset.row = cursor.y;
  }

  if (cursor.y >= offset.row + window.rows()) {
    offset.row = cursor.y - window.rows() + 1;
  }

  if (cursor.x < offset.col) {
    offset.col = cursor.x;
  }

  if (cursor.x >= offset.col + window.cols()) {
    offset.col = cursor.x - window.cols() + 1;
  }
}

void updateRow(std::string_view row, std::string& render)
{
  using editor::KiloTabStop;

  [[maybe_unused]]
  auto tabs = std::ranges::count_if(row, [](unsigned char c) { return c == '\t'; });

  int idx {};

  for (std::size_t j {}; j < row.length(); j++) {
    if (row[j] == '\t') {
      render[idx] = ' ';
      idx++;

      while (idx % KiloTabStop != 0) {
        render[idx] = ' ';
        idx++;
      }
    }
    else {
      render[idx] = row[j];
      idx++;
    }
  }
}
}   // namespace Kilo::editor

namespace Kilo::editor::detail {

/**
 * @brief Write the welcome message to the screen buffer
 *
 * @param windowWidth The width of the window in which the message is to be displayed
 * @param buffer The buffer to which the message is written before being displayed
 */
void printWelcomeMessage(int windowWidth, ScreenBuffer& buffer)
{
  auto msg = fmt::format("Kilo editor -- version {}", editor::KiloVersion);

  // If the message is longer than the window's width, resize it to fit
  if (std::cmp_greater(msg.length(), windowWidth)) {
    msg.resize(windowWidth);
  }

  /*
   * Center the string
   * Divide the screen width by 2 and then subtract half the string's length
   * from this value. This tells us how far from the left edge of the screen we
   * should start printing the string. So, we fill that space with space
   * characters, except for the first character, which should be a tilde
   */

  auto padding = (windowWidth - std::ssize(msg)) / 2;

  if (padding > 0) {
    buffer.write("~");
    padding--;
  }

  while (padding > 0) {
    buffer.write(" ");
    padding--;
  }

  buffer.write(msg);
}

/**
 * @brief Print the welcome message or a tilde to the window
 *
 * @param documentIsEmpty A boolean value representing whether the document is empty or not
 * @param currentRow The row we are currently scrolled to in the open document
 * @param buffer The ScreenBuffer
 * @param window The terminal window
 */
void printWelcomeMessageOrTilde(bool documentIsEmpty, int currentRow, ScreenBuffer& buffer,
                                Terminal::Window const& window)
{
  if (documentIsEmpty && currentRow == window.rows() / 3) {
    detail::printWelcomeMessage(window.cols(), buffer);
  }
  else {
    buffer.write("~");
  }
}

/**
 * @brief Print a line of text from the open document to the screen
 *
 * @param line The line to be printed
 * @param buffer The screen buffer
 * @param windowWidth The width of the terminal window
 * @param columnOffset The column offset between the terminal window width and the document width
 * @pre The column offset must be non-negative
 */
void printLineOfDocument(std::string const& line, ScreenBuffer& buffer, int const windowWidth, int const columnOffset)
{
  assert(columnOffset >= 0 and "Column offset must be non-negative");

  auto lineLen = std::ssize(line) - columnOffset;

  if (lineLen < 0) {
    lineLen = 0;
  }

  if (lineLen > windowWidth) {
    lineLen = windowWidth;
  }

  buffer.write(&line[columnOffset], lineLen);
}

}   // namespace Kilo::editor::detail
