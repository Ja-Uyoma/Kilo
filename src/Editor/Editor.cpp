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

#include "Terminal/File.hpp"
#include "Terminal/window/window.hpp"
#include "Utilities/Utilities.hpp"
#include "constants/constants.hpp"
#include "cursor/cursor.hpp"
#include "offset/offset.hpp"
#include "screen_buffer/screen_buffer.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fmt/format.h>
#include <fstream>
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
void processKeypress(int const keyPressed, Cursor& cursor, terminal::Window const& window,
                     std::vector<std::string> const& document) noexcept
{
  detail::processKeypressHelper(keyPressed);

  using utilities::EditorKey;
  auto key = static_cast<EditorKey>(keyPressed);

  detail::processKeypressHelper(key, cursor, window, document);
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
void refreshScreen(ScreenBuffer& buffer, Cursor const& cursor, Offset const& offset, terminal::Window const& window,
                   std::vector<std::string> const& document, std::vector<std::string> const& renderedDoc)
{
  /*
   * Hide the cursor when painting and then move it to the home position
   */

  buffer.write(EscapeSequences::HideCursorWhenRepainting).write(EscapeSequences::MoveCursorToHomePosition);

  terminal::File output;

  drawRows(window, offset, document, buffer, renderedDoc);
  auto const cursorPos = detail::setExactPositionToMoveCursorTo(cursor, offset);
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
void drawRows(terminal::Window const& window, Offset const& offset, std::vector<std::string> const& doc,
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
 * @param row The document which is currently open
 */
void moveCursor(utilities::EditorKey key, Cursor& cursor, std::vector<std::string> const& row)
{
  detail::moveCursorHelper(cursor, key, row);

  auto currRow = detail::getCurrentRow(cursor.y, row);
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
void scroll(Cursor const& cursor, Offset& offset, terminal::Window const& window) noexcept
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
  using utilities::KILO_TAB_STOP;

  [[maybe_unused]]
  auto tabs = std::ranges::count_if(row, [](unsigned char c) { return c == '\t'; });

  int idx {};

  for (std::size_t j {}; j < row.length(); j++) {
    if (row[j] == '\t') {
      render[idx] = ' ';
      idx++;

      while (idx % KILO_TAB_STOP != 0) {
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
 * @brief Specify the exact position we want the cursor to move to
 *
 * @param cursor The current position of the cursor
 * @param offset The offset from the terminal window to the currently-open document
 * @return std::string
 */
std::string setExactPositionToMoveCursorTo(Cursor const& cursor, Offset const& offset)
{
  /*
   * We add 1 to cursor.x and cursor.y to convert from 0-indexed values to the
   * 1-indexed values that the terminal uses
   */

  return fmt::format("\x1b[{};{}H", (cursor.y - offset.row) + 1, (cursor.x - offset.col) + 1);
}

/**
 * @brief Create a welcome message by interpolating two strings
 *
 * @param versionString The version of the application
 * @return std::string The welcome message
 */
std::string createWelcomeMessage(std::string_view versionString) noexcept
{
  return fmt::format("Kilo editor -- version {}", versionString);
}

/**
 * @brief Resizes the message string to be equal to the window width if it exceeds it
 *
 * @param message The message string to be resized
 * @param windowWidth The unit determining how much the string should be resized
 */
void resizeWelcomeMessage(std::string& message, int windowWidth) noexcept
{
  assert(windowWidth >= 0 and "Window width cannot be less than zero");

  if (std::cmp_greater(message.length(), windowWidth)) {
    message.resize(windowWidth);
  }
}

/**
 * @brief Write padding characters to the screen buffer
 *
 * @param padding The number of characters to be written to the screen buffer
 * @param buf The screen buffer being written to
 */
void writePaddingToScreenBuffer(long padding, ScreenBuffer& buf)
{
  if (padding > 0) {
    buf.write("~");
    padding--;
  }

  while (padding > 0) {
    buf.write(" ");
    padding--;
  }
}

/**
 * @brief Write the welcome message to the screen buffer
 *
 * @param windowWidth The width of the window in which the message is to be displayed
 * @param buffer The buffer to which the message is written before being displayed
 */
void printWelcomeMessage(int windowWidth, ScreenBuffer& buffer)
{
  auto msg = createWelcomeMessage(utilities::KILO_VERSION);
  resizeWelcomeMessage(msg, windowWidth);

  /*
   * Center the string
   * Divide the screen width by 2 and then subtract half the string's length
   * from this value. This tells us how far from the left edge of the screen we
   * should start printing the string. So, we fill that space with space
   * characters, except for the first character, which should be a tilde
   */

  auto padding = getPadding(windowWidth, msg.length());

  writePaddingToScreenBuffer(padding, buffer);

  buffer.write(msg);
}

/**
 * @brief Perform an editor operation depending on the key pressed
 *
 * @param[in] keyPressed The key pressed by the user
 */
void processKeypressHelper(unsigned const keyPressed) noexcept
{
  using utilities::clearScreenAndRepositionCursor;
  using utilities::ctrlKey;

  if (keyPressed == ctrlKey('q')) {
    clearScreenAndRepositionCursor();
    std::exit(EXIT_SUCCESS);
  }
}

/**
 * @brief Perform an editor operation depending on the key pressed
 *
 * @param[in] keyPressed The key pressed by the user
 * @param[in] cursor The position of the cursor in the terminal window
 * @param[in] window The terminal window
 */
void processKeypressHelper(utilities::EditorKey keyPressed, Cursor& cursor, terminal::Window const& window,
                           std::vector<std::string> const& document) noexcept
{
  using enum utilities::EditorKey;

  if (keyPressed == Home) {
    cursor.x = 0;
  }
  else if (keyPressed == End) {
    cursor.x = window.cols() - 1;
  }
  else if (keyPressed == PageUp or keyPressed == PageDown) {
    for (auto i = window.rows(); i > 0; i--) {
      moveCursor(keyPressed == PageUp ? ArrowUp : ArrowDown, cursor, document);
    }
  }
  else if (keyPressed == ArrowLeft or keyPressed == ArrowRight or keyPressed == ArrowUp or keyPressed == ArrowDown) {
    moveCursor(keyPressed, cursor, document);
  }
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
                                terminal::Window const& window)
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
