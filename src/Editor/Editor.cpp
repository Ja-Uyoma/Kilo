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

#include "Constants.hpp"
#include "Cursor.hpp"
#include "EditorConfig/EditorConfig.hpp"
#include "Offset.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"
#include "Terminal/Terminal.hpp"
#include "Utilities/Utilities.hpp"
#include "Window/Window.hpp"
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

namespace Kilo::editor {
static EditorConfig editorConfig;

/**
 * @brief Performs an action depending on the key pressed
 *
 * @param[in] keyPressed The key pressed by the user
 * @param[in] cursor The position of the cursor in the terminal window
 * @param[in] window The terminal window
 */
void processKeypress(int const keyPressed, Cursor& cursor, window::Window const& window) noexcept
{
  detail::processKeypressHelper(keyPressed);

  using utilities::EditorKey;
  auto key = static_cast<EditorKey>(keyPressed);

  detail::processKeypressHelper(key, cursor, window);
}

void processKeypress()
{
  using utilities::clearScreenAndRepositionCursor;
  using utilities::ctrlKey;

  int c = terminal::readKey();

  if (c == ctrlKey('q')) {
    clearScreenAndRepositionCursor();
    std::exit(EXIT_SUCCESS);
  }

  using enum utilities::EditorKey;
  using utilities::EditorKey;

  auto key = static_cast<EditorKey>(c);

  if (key == Home) {
    editorConfig.cursor.x = 0;
  }
  else if (key == End) {
    editorConfig.cursor.x = editorConfig.window.cols() - 1;
  }
  else if (key == PageUp or key == PageDown) {
    for (auto i = editorConfig.window.rows(); i > 0; i--) {
      moveCursor(key == PageUp ? ArrowUp : ArrowDown);
    }
  }
  else if (key == ArrowLeft or key == ArrowRight or key == ArrowUp or key == ArrowDown) {
    moveCursor(key);
  }
}

void refreshScreen() noexcept
{
  using namespace std::string_literals;

  scroll();

  ScreenBuffer buffer;

  /*
   * Hide the cursor when painting and then move it to the home position
   */

  buffer.write(EscapeSequences::HideCursorWhenRepainting).write(EscapeSequences::MoveCursorToHomePosition);

  drawRows(buffer);

  auto const cursorPos = detail::setExactPositionToMoveCursorTo(editorConfig.cursor, editorConfig.off);

  buffer.write(cursorPos).write(EscapeSequences::ShowTheCursor).flush();
}

void drawRows(ScreenBuffer& buffer) noexcept
{
  using namespace std::string_literals;

  for (int y = 0; y < editorConfig.window.rows(); y++) {
    if (int filerow = y + editorConfig.off.row; filerow >= editorConfig.numrows) {
      if (editorConfig.numrows == 0 && y == editorConfig.window.rows() / 3) {
        detail::printWelcomeMessage(editorConfig.window.cols(), buffer);
      }
      else {
        buffer.write("~"s);
      }
    }
    else {
      auto len = std::ssize(editorConfig.render[filerow]) - editorConfig.off.col;

      if (len < 0) {
        len = 0;
      }

      if (len > editorConfig.window.cols()) {
        len = editorConfig.window.cols();
      }

      buffer.write(&editorConfig.render[filerow][editorConfig.off.col], len);
    }

    buffer.write(EscapeSequences::ErasePartOfLineToTheRightOfCursor);

    if (y < editorConfig.window.rows() - 1) {
      buffer.write("\r\n"s);
    }
  }
}

void moveCursor(utilities::EditorKey key) noexcept
{
  detail::moveCursorHelper(editorConfig.cursor, key, editorConfig.row);

  auto currRow = detail::getCurrentRow(editorConfig.cursor, editorConfig.row);

  int rowlen = currRow ? currRow->length() : 0;

  if (editorConfig.cursor.x > rowlen) {
    editorConfig.cursor.x = rowlen;
  }
}

bool open(std::filesystem::path const& path)
{
  // TODO: rewrite the error handling logic to match the approach used in rest
  // of the application

  std::ifstream infile(path);

  if (!infile) {
    return false;
  }

  std::string line;

  while (std::getline(infile, line)) {
    editorConfig.row.push_back(line);
    editorConfig.numrows++;
  }

  editorConfig.render = editorConfig.row;

  return true;
}

void scroll() noexcept
{
  editorConfig.rx = editorConfig.cursor.x;

  /*
   * Check if the cursor has moved outside of the visible window.
   * If so, adjust editorConfig.off.row and/or editorConfig.off.col so that the
   * cursor is just inside the visible window
   */

  if (editorConfig.cursor.y < editorConfig.off.row) {
    editorConfig.off.row = editorConfig.cursor.y;
  }

  if (editorConfig.cursor.y >= editorConfig.off.row + editorConfig.window.rows()) {
    editorConfig.off.row = editorConfig.cursor.y - editorConfig.window.rows() + 1;
  }

  if (editorConfig.cursor.x < editorConfig.off.col) {
    editorConfig.off.col = editorConfig.cursor.x;
  }

  if (editorConfig.cursor.x >= editorConfig.off.col + editorConfig.window.cols()) {
    editorConfig.off.col = editorConfig.cursor.x - editorConfig.window.cols() + 1;
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
void processKeypressHelper(utilities::EditorKey keyPressed, Cursor& cursor, window::Window const& window) noexcept
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
      moveCursor(keyPressed == PageUp ? ArrowUp : ArrowDown);
    }
  }
  else if (keyPressed == ArrowLeft or keyPressed == ArrowRight or keyPressed == ArrowUp or keyPressed == ArrowDown) {
    moveCursor(keyPressed);
  }
}

}   // namespace Kilo::editor::detail
