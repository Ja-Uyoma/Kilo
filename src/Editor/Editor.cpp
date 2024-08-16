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

#include "EditorConfig/EditorConfig.hpp"
#include "Terminal/Terminal.hpp"
#include "Utilities/Utilities.hpp"
#include "WriteBuffer/WriteBuffer.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unistd.h>

namespace Kilo::editor {
static EditorConfig editorConfig;

namespace detail {

void displayWelcomeMessage(WriteBuffer& buffer)
{
  using namespace std::string_literals;
  using namespace Kilo::Utilities;

  char welcome[80] {};

  // Interpolate KILO_VERSION into the welcome message
  int welcomeLen =
    std::snprintf(welcome, sizeof welcome, "Kilo editor -- version %s", KILO_VERSION);

  // Truncate the length of the string in case the terminal is too small to fit
  // the welcome message
  if (welcomeLen > editorConfig.window.cols) {
    welcomeLen = editorConfig.window.cols;
  }

  // Center the string
  // Divide the screen width by 2 and then subtract half the string's length
  // from this value. This tells us how far from the left edge of the screen we
  // should start printing the string. So, we fill that space with space
  // characters, except for the first character, which should be a tilde

  int padding = (editorConfig.window.cols - welcomeLen) / 2;

  if (padding > 0) {
    buffer.write("~"s);
    padding--;
  }

  while (padding > 0) {
    buffer.write(" "s);
    padding--;
  }

  buffer.write(welcome, welcomeLen);
}

}   // namespace detail

void processKeypress()
{
  using namespace Kilo::Utilities;
  using enum Kilo::Utilities::EditorKey;

  int c = terminal::readKey();

  switch (c) {
    case ctrlKey('q'):
      clearScreenAndRepositionCursor();
      std::exit(EXIT_SUCCESS);
      break;
    case static_cast<int>(Home):     editorConfig.cursor.x = 0; break;
    case static_cast<int>(End):      editorConfig.cursor.x = editorConfig.window.cols - 1; break;
    case static_cast<int>(PageUp):
    case static_cast<int>(PageDown): {
      for (int i = editorConfig.window.rows; i > 0; i--) {
        moveCursor(c == static_cast<int>(PageUp) ? ArrowUp : ArrowDown);
      }
    } break;
    case static_cast<int>(ArrowUp):
    case static_cast<int>(ArrowDown):
    case static_cast<int>(ArrowLeft):
    case static_cast<int>(ArrowRight): moveCursor(static_cast<EditorKey>(c));
    default:                           break;
  }
}

void refreshScreen() noexcept
{
  using namespace std::string_literals;

  scroll();

  WriteBuffer buffer;

  buffer.write("\x1b[?25l"s);   // hide the cursor when repainting
  buffer.write("\x1b[H"s);      // reposition the cursor

  drawRows(buffer);

  char buf[32];

  // Specify the exact position we want the cursor to move to
  // We add 1 to cursor.x and cursor.y to convert from 0-indexed values to the
  // 1-indexed values that the terminal uses
  std::snprintf(buf,
                sizeof buf,
                "\x1b[%d;%dH",
                (editorConfig.cursor.y - editorConfig.off.row) + 1,
                (editorConfig.cursor.x - editorConfig.off.col) + 1);

  buffer.write(buf, std::strlen(buf));
  buffer.write("\x1b[?25h"s);   // show the cursor

  ::write(STDOUT_FILENO, buffer.c_str(), buffer.size());
}

void drawRows(WriteBuffer& buffer) noexcept
{
  using namespace std::string_literals;

  for (int y = 0; y < editorConfig.window.rows; y++) {
    if (int filerow = y + editorConfig.off.row; filerow >= editorConfig.numrows) {
      if (editorConfig.numrows == 0 && y == editorConfig.window.rows / 3) {
        detail::displayWelcomeMessage(buffer);
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

      if (len > editorConfig.window.cols) {
        len = editorConfig.window.cols;
      }

      buffer.write(&editorConfig.render[filerow][editorConfig.off.col], len);
    }

    buffer.write("\x1b[K"s);

    if (y < editorConfig.window.rows - 1) {
      buffer.write("\r\n"s);
    }
  }
}

void moveCursor(Utilities::EditorKey key) noexcept
{
  moveCursor(editorConfig.cursor, key, editorConfig.row);

  auto currRow = getCurrentRow(editorConfig.cursor, editorConfig.row);

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

  if (editorConfig.cursor.y >= editorConfig.off.row + editorConfig.window.rows) {
    editorConfig.off.row = editorConfig.cursor.y - editorConfig.window.rows + 1;
  }

  if (editorConfig.cursor.x < editorConfig.off.col) {
    editorConfig.off.col = editorConfig.cursor.x;
  }

  if (editorConfig.cursor.x >= editorConfig.off.col + editorConfig.window.cols) {
    editorConfig.off.col = editorConfig.cursor.x - editorConfig.window.cols + 1;
  }
}

void updateRow(std::string_view row, std::string& render)
{
  using Utilities::KILO_TAB_STOP;

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
