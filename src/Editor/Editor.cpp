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

#include "Cursor/Cursor.hpp"
#include "File/File.hpp"
#include "Offset/Offset.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"
#include "Terminal/Window/Window.hpp"
#include "Utilities/Constants.hpp"
#include "Utilities/Utilities.hpp"
#include <fmt/format.h>
#include <string_view>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

namespace Kilo::editor {

/*
 * \brief Move the cursor in the open document depending on the key pressed
 * \param[in] keyPressed The key pressed by the user
 * \param[in] editor The current state of the editor
 */
void processKeypress(int keyPressed, EditorConfig& editorConfig)
{
  if (keyPressed == utilities::ctrlKey('q')) {
    utilities::clearScreenAndRepositionCursor();
    std::exit(EXIT_SUCCESS);
  }

  using enum EditorKey;

  if (auto const key = static_cast<EditorKey>(keyPressed); key == Home) {
    editorConfig.cursor.x = 0;
  }
  else if (key == End) {
    editorConfig.cursor.x = editorConfig.window.cols() - 1;
  }
  else if (key == PageUp or key == PageDown) {
    for (int i = editorConfig.window.rows(); i > 0; --i) {
      moveCursor(key == PageUp ? ArrowUp : ArrowDown, editorConfig);
    }
  }
  else if (key == ArrowLeft or key == ArrowRight or key == ArrowUp or key == ArrowDown) {
    moveCursor(key, editorConfig);
  }
}

/*
 * \brief Perform a screen refresh
 * \param[in] editor The current editor configuration
 */
void refreshScreen(EditorConfig& editor)
{
  // Hide the cursor when painting and then move it to the Home position
  editor.screenBuffer
    .write(EscapeSequences::HideCursorWhenRepainting)
    .write(EscapeSequences::MoveCursorToHomePosition);

  // Draw the welcome message, or each row of the currently open document with a tilde at the beginning
  drawRows(editor);

  // We want to show the cursor immediately after writing the contents of the open document or the welcome message.
  // To do this, we must first get the cursor position, and then write it to the screen buffer before flushing it

  // We get the cursor position in the terminal by adding 1 to cursor.x and cursor.y
  // (less the corresponding offset values) to convert from 0-indexed values to the 1-indexed values
  // that the terminal uses
  auto const cursorPos = fmt::format(
    "\x1b[{};{}H",
    (editor.cursor.y - editor.offset.row) + 1,
    (editor.cursor.x - editor.offset.col) + 1
    );

  // The file to which the screen buffer writes its contents when flushed (typically STDOUT)
  IO::File outputFile{};

  editor.screenBuffer
    .write(cursorPos)
    .write(EscapeSequences::ShowTheCursor)
    .flush(outputFile);
}

/*
 * \brief Draw each row of the buffer of text being edited, plus a tilde at the beginning, or the welcome message
 * \param[in] editor The editor configuration
 */
void drawRows(EditorConfig& editor)
{
  for (int currentRow = 0; currentRow < editor.window.rows(); ++currentRow) {
    if (auto const fileRow = currentRow + editor.offset.row; fileRow >= std::ssize(editor.openDoc)) {
      if (editor.openDoc.empty() and currentRow == editor.window.rows() / 3) {
        detail::printWelcomeMessage(editor.window.cols(), editor.screenBuffer);
      }
      else {
        editor.screenBuffer.write("~");
      }
    }
    else {
      detail::printLineOfDocument(editor.renderedDoc[fileRow], editor.screenBuffer, editor.window.cols(),
                                  editor.offset.col);
    }

    editor.screenBuffer.write(EscapeSequences::ErasePartOfLineToTheRightOfCursor);

    if (currentRow < editor.window.rows() - 1) {
      editor.screenBuffer.write("\r\n");
    }
  }
}

/*
 * \brief Move the cursor in the direction of the key pressed
 * \param[in] key The key pressed by the user
 * \param[in] editor The current state of the editor
 */
void moveCursor(EditorKey const key, EditorConfig& editor)
{
  using enum EditorKey;

  switch (key) {
    case ArrowLeft:
      if (editor.cursor.x != 0) {
        --editor.cursor.x;
      }
      else if (editor.cursor.y > 0) {
        --editor.cursor.y;
        editor.cursor.x = std::ssize(editor.openDoc[editor.cursor.y]);
      }
      break;
    case ArrowRight: {
      auto const currRow = std::invoke([&editor]() -> std::optional<std::string> {
        if (editor.cursor.y >= std::ssize(editor.openDoc)) {
          return std::nullopt;
        }
        else {
          return std::make_optional(editor.openDoc[editor.cursor.y]);
        }
      });

      if (currRow and editor.cursor.x < std::ssize(*currRow)) {
        ++editor.cursor.x;
      }
      else if (currRow and editor.cursor.x == std::ssize(*currRow)) {
        ++editor.cursor.y;
        editor.cursor.x = 0;
      }
    }
    break;

    case ArrowUp:
      if (editor.cursor.y != 0) {
        --editor.cursor.y;
      }
      break;

    case ArrowDown:
      if (editor.cursor.y < std::ssize(editor.openDoc)) {
        ++editor.cursor.y;
      }

    default:
      return;
  }

  auto const currRow = std::invoke([&editor]() -> std::optional<std::string> {
    if (editor.cursor.y >= std::ssize(editor.openDoc)) {
      return std::nullopt;
    }
    else {
      return std::make_optional(editor.openDoc[editor.cursor.y]);
    }
  });

  auto const rowLength = currRow ? std::ssize(*currRow) : 0;

  if (editor.cursor.x >= rowLength) {
    editor.cursor.x = rowLength;
  }
}

/*
 * \brief Fix the cursor in the visible window while scrolling
 * \param[in] editor The current state of the editor
 */
void scroll(EditorConfig& editor)
{
  // Check if the cursor has moved outside the visible window
  // If so, adjust the editor.offset.row and/or editor.offset.col variable(s) so that the
  // cursor is just inside the visible window

  if (editor.cursor.y < editor.offset.row) {
    editor.offset.row = editor.cursor.y;
  }

  if (editor.cursor.y >= editor.offset.row + editor.window.rows()) {
    editor.offset.row = editor.cursor.y - editor.window.rows() + 1;
  }

  if (editor.cursor.x < editor.offset.col) {
    editor.offset.col = editor.cursor.x;
  }

  if (editor.cursor.x >= editor.offset.col + editor.window.cols()) {
    editor.offset.col = editor.cursor.x - editor.window.cols() + 1;
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

  [[maybe_unused]] auto tabs = std::ranges::count_if(row, [](unsigned char c) {
    return c == '\t';
  });

  int idx{};

  for (std::size_t j{}; j < row.length(); j++) {
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
} // namespace Kilo::editor

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

} // namespace Kilo::editor::detail
