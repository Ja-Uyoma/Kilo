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
#include "Offset/Offset.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"
#include "kilo/io/File.hpp"
#include "kilo/terminal/Window/Window.hpp"
#include "kilo/utilities/Constants.hpp"
#include "kilo/utilities/Utilities.hpp"
#include <fmt/format.h>
#include <string_view>

#include <algorithm>
#include <cassert>
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

namespace kilo::editor {

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

  using enum utilities::EditorKey;

  if (auto const key = static_cast<utilities::EditorKey>(keyPressed); key == Home) {
    editorConfig.cursor.x = 0;
  }
  else if (key == End) {
    editorConfig.cursor.x = editorConfig.window.cols() - 1;
  }
  else if (key == PageUp or key == PageDown) {
    for (int32_t i = editorConfig.window.rows(); i > 0; --i) {
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
  editor.screenBuffer.write(utilities::EscapeSequences::HideCursorWhenRepainting)
    .write(utilities::EscapeSequences::MoveCursorToHomePosition);

  // Draw the welcome message, or each row of the currently open document with a tilde at the beginning
  drawRows(editor);

  // We want to show the cursor immediately after writing the contents of the open document or the welcome message.
  // To do this, we must first get the cursor position, and then write it to the screen buffer before flushing it

  // We get the cursor position in the terminal by adding 1 to cursor.x and cursor.y
  // (less the corresponding offset values) to convert from 0-indexed values to the 1-indexed values
  // that the terminal uses
  auto const cursorPos =
    fmt::format("\x1b[{};{}H", (editor.cursor.y - editor.offset.row) + 1, (editor.cursor.x - editor.offset.col) + 1);

  // The file to which the screen buffer writes its contents when flushed (typically STDOUT)
  io::File outputFile {};

  editor.screenBuffer.write(cursorPos).write(utilities::EscapeSequences::ShowTheCursor).flush(outputFile);
}

/*
 * \brief Draw each row of the buffer of text being edited, plus a tilde at the beginning, or the welcome message
 * \param[in] editor The editor configuration
 */
void drawRows(EditorConfig& editor)
{
  for (int32_t currentRow = 0; currentRow < editor.window.rows(); ++currentRow) {
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

    editor.screenBuffer.write(utilities::EscapeSequences::ErasePartOfLineToTheRightOfCursor);

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
void moveCursor(utilities::EditorKey const key, EditorConfig& editor)
{
  using enum utilities::EditorKey;

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
    } break;

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

  editor.cursor.x = std::min(editor.cursor.x, rowLength);
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

  editor.offset.row = std::min(editor.cursor.y, editor.offset.row);

  if (editor.cursor.y >= editor.offset.row + editor.window.rows()) {
    editor.offset.row = editor.cursor.y - editor.window.rows() + 1;
  }

  editor.offset.col = std::min(editor.cursor.x, editor.offset.col);

  if (editor.cursor.x >= editor.offset.col + editor.window.cols()) {
    editor.offset.col = editor.cursor.x - editor.window.cols() + 1;
  }
}

/*
 * \brief Open a file and write its contents to memory
 *
 * \param[in] path The path to the file
 * \param[in] document The buffer containing the file in memory
 * \param[in] rendered The document that is actually rendered to the window
 * \return true If the operation was successful, and false otherwise
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

void updateRow(std::string_view row, std::string& render)
{
  using utilities::KiloTabStop;

  [[maybe_unused]] auto tabs = std::ranges::count_if(row, [](unsigned char c) { return c == '\t'; });

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
}   // namespace kilo::editor

namespace kilo::editor::detail {

/*
 * \brief Write the welcome message to the screen buffer
 *
 * \param[in] windowWidth The width of the window in which the message is to be displayed
 * \param[in] buffer The buffer to which the message is written before being displayed
 */
void printWelcomeMessage(int32_t const windowWidth, ScreenBuffer& buffer)
{
  auto msg = fmt::format("Kilo editor -- version {}", utilities::KiloVersion);

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

/*
 * \brief Print a line of text from the open document to the screen
 *
 * \param[in] line The line to be printed
 * \param[in] buffer The screen buffer
 * \param[in] windowWidth The width of the terminal window
 * \param[in] columnOffset The column offset between the terminal window width and the document width
 * \pre The column offset must be non-negative
 */
void printLineOfDocument(std::string const& line, ScreenBuffer& buffer, int32_t const windowWidth,
                         int64_t const columnOffset)
{
  assert(columnOffset >= 0 and "Column offset must be non-negative");

  auto lineLen = std::ssize(line) - columnOffset;

  lineLen = std::max<int64_t>(lineLen, 0);

  lineLen = std::min<int64_t>(lineLen, windowWidth);

  buffer.write(&line[columnOffset], lineLen);
}

}   // namespace kilo::editor::detail
