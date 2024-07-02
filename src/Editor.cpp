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

#include "EditorConfig.hpp"
#include "Terminal.hpp"
#include "Utilities.hpp"
#include "WriteBuffer.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unistd.h>
#include <utility>

namespace Kilo::Editor {
static EditorConfig editorConfig;

namespace {
void displayWelcomeMessage(WriteBuffer& buffer)
{
  using namespace std::string_literals;
  using namespace Kilo::Utilities;

  char welcome[80] {};

  // Interpolate KILO_VERSION into the welcome message
  int welcomeLen = std::snprintf(welcome, sizeof welcome, "Kilo editor -- version %s", KILO_VERSION);

  // Truncate the length of the string in case the terminal is too small to fit
  // the welcome message
  if (welcomeLen > editorConfig.screenCols) {
    welcomeLen = editorConfig.screenCols;
  }

  // Center the string
  // Divide the screen width by 2 and then subtract half the string's length
  // from this value. This tells us how far from the left edge of the screen we
  // should start printing the string. So, we fill that space with space
  // characters, except for the first character, which should be a tilde

  int padding = (editorConfig.screenCols - welcomeLen) / 2;

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
}   // namespace

void processKeypress()
{
  using namespace Kilo::Utilities;
  using enum Kilo::Utilities::EditorKey;

  int c = Terminal::readKey();

  switch (c) {
  case ctrlKey('q'):
    clearScreenAndRepositionCursor();
    std::exit(EXIT_SUCCESS);
    break;
  case static_cast<int>(Home):     editorConfig.cursorX = 0; break;
  case static_cast<int>(End):      editorConfig.cursorX = editorConfig.screenCols - 1; break;
  case static_cast<int>(PageUp):
  case static_cast<int>(PageDown): {
    for (int i = editorConfig.screenRows; i > 0; i--) {
      moveCursor(c == static_cast<int>(PageUp) ? static_cast<int>(ArrowUp) : static_cast<int>(ArrowDown));
    }
  } break;
  case static_cast<int>(ArrowUp):
  case static_cast<int>(ArrowDown):
  case static_cast<int>(ArrowLeft):
  case static_cast<int>(ArrowRight): moveCursor(c);
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
  // We add 1 to cursorX and cursorY to convert from 0-indexed values to the
  // 1-indexed values that the terminal uses
  std::snprintf(buf,
                sizeof buf,
                "\x1b[%d;%dH",
                (editorConfig.cursorY - editorConfig.rowoff) + 1,
                (editorConfig.cursorX - editorConfig.coloff) + 1);

  buffer.write(buf, std::strlen(buf));
  buffer.write("\x1b[?25h"s);   // show the cursor

  ::write(STDOUT_FILENO, buffer.c_str(), buffer.size());
}

void drawRows(WriteBuffer& buffer) noexcept
{
  using namespace std::string_literals;

  for (int y = 0; y < editorConfig.screenRows; y++) {
    if (int filerow = y + editorConfig.rowoff; filerow >= editorConfig.numrows) {
      if (editorConfig.numrows == 0 && y == editorConfig.screenRows / 3) {
        displayWelcomeMessage(buffer);
      }
      else {
        buffer.write("~"s);
      }
    }
    else {
      auto len = std::ssize(editorConfig.render[filerow]) - editorConfig.coloff;

      if (len < 0) {
        len = 0;
      }

      if (len > editorConfig.screenCols) {
        len = editorConfig.screenCols;
      }

      // TODO: use coloff as an index into the chars of each row of text
      buffer.write(editorConfig.render[filerow]);
    }

    buffer.write("\x1b[K"s);

    if (y < editorConfig.screenRows - 1) {
      buffer.write("\r\n"s);
    }
  }
}

void moveCursor(int key)
{
  using enum Kilo::Utilities::EditorKey;

  std::optional<std::string> currRow = std::invoke([]() -> std::optional<std::string> {
    if (editorConfig.cursorY >= editorConfig.numrows) {
      return std::nullopt;
    }

    return std::make_optional(editorConfig.row[editorConfig.cursorY]);
  });

  switch (key) {
  case static_cast<int>(ArrowLeft):
    if (editorConfig.cursorX != 0) {
      editorConfig.cursorX--;
    }
    else if (editorConfig.cursorY > 0) {
      editorConfig.cursorY--;
      editorConfig.cursorX = std::ssize(editorConfig.row[editorConfig.cursorY]);
    }

    break;
  case static_cast<int>(ArrowRight):
    if (currRow && std::cmp_less(editorConfig.cursorX, currRow->size())) {
      editorConfig.cursorX++;
    }
    else if (currRow && std::cmp_equal(editorConfig.cursorX, currRow->size())) {
      editorConfig.cursorY++;
      editorConfig.cursorX = 0;
    }
    break;
  case static_cast<int>(ArrowUp):
    if (editorConfig.cursorY != 0) {
      editorConfig.cursorY--;
    }

    break;
  case static_cast<int>(ArrowDown):
    if (editorConfig.cursorY < editorConfig.numrows) {
      editorConfig.cursorY++;
    }

    break;
  default: break;
  }

  currRow = std::invoke([]() -> std::optional<std::string> {
    if (editorConfig.cursorY >= editorConfig.numrows) {
      return std::nullopt;
    }

    return std::make_optional(editorConfig.row[editorConfig.cursorY]);
  });

  int rowlen = currRow ? currRow->length() : 0;

  if (editorConfig.cursorX > rowlen) {
    editorConfig.cursorX = rowlen;
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
  /*
   * Check if the cursor has moved outside of the visible window.
   * If so, adjust editorConfig.rowoff and/or editorConfig.coloff so that the
   * cursor is just inside the visible window
   */

  if (editorConfig.cursorY < editorConfig.rowoff) {
    editorConfig.rowoff = editorConfig.cursorY;
  }

  if (editorConfig.cursorY >= editorConfig.rowoff + editorConfig.screenRows) {
    editorConfig.rowoff = editorConfig.cursorY - editorConfig.screenRows + 1;
  }

  if (editorConfig.cursorX < editorConfig.coloff) {
    editorConfig.coloff = editorConfig.cursorX;
  }

  if (editorConfig.cursorX >= editorConfig.coloff + editorConfig.screenCols) {
    editorConfig.coloff = editorConfig.cursorX - editorConfig.screenCols + 1;
  }
}

void updateRow(std::string_view row, std::string& render)
{
  render.assign(row);
}
}   // namespace Kilo::Editor
