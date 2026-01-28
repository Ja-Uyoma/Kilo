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

#include "kilo/editor/Cursor/Cursor.hpp"
#include "kilo/editor/Editor.hpp"
#include "kilo/editor/EditorConfig/EditorConfig.hpp"
#include "kilo/editor/ScreenBuffer/ScreenBuffer.hpp"
#include "kilo/terminal/Window/Window.hpp"
#include "kilo/utilities/Utilities.hpp"
#include "kilo/utilities/Constants.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstring>
#include <string>
#include <vector>

namespace kilo::editor {

TEST(processKeypress, TerminatesTheProgramIfQIsPressed)
{
  using utilities::editor_key;

  editor_config editor_config;
  constexpr auto key = utilities::ctrl_key('q');

  ASSERT_EXIT(process_keypress(key, editor_config), ::testing::ExitedWithCode(0), ::testing::Eq(""));
}

TEST(processKeypress, MovesCursorToStartOfLineIfHomeButtonIsPressed)
{
  using utilities::editor_key;

  constexpr auto key = editor_key::home;
  editor_config editor_config;

  process_keypress(static_cast<int>(key), editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
}

TEST(processKeypress, MovesCursorToEndOfLineIfEndButtonIsPressed)
{
  using utilities::editor_key;

  constexpr auto key = editor_key::end;
  editor_config editor_config;

  process_keypress(static_cast<int>(key), editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(editor_config.window.cols() - 1));
}

namespace detail {

TEST(printWelcomeMessage, PrintsTheCorrectMessageCentred)
{
  constexpr int width = 50;
  screen_buffer buf {};

  print_welcome_message(width, buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  auto const padding = (width - msg.length()) / 2;
  std::string const output = "~" + std::string(padding - 1, ' ') + msg;

  ASSERT_THAT(buf.c_str(), ::testing::Eq(output));
}

TEST(printWelcomeMessage, TruncatesTheMessageIfItsTooLong)
{
  constexpr int width = 25;
  screen_buffer buf {};

  print_welcome_message(width, buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  std::string const truncated_msg = msg.substr(0, width);

  ASSERT_THAT(buf.c_str(), ::testing::Eq(truncated_msg));
}

TEST(printLineOfDocument, PrintsNothingWhenTheLineLengthIsLessThanTheColumnOffset)
{
  std::string const line {"The quick brown fox jumped over the lazy doggo"};
  constexpr int window_width = 20;
  auto const col_off = std::ssize(line) + 5;
  screen_buffer buf;

  print_line_of_document(line, buf, window_width, col_off);

  ASSERT_THAT(buf.size(), ::testing::Eq(0));
}

TEST(printLineOfDocument, TruncatesTheLineIfItsLongerThanWindowWidth)
{
  std::string const line {"The quick brown fox jumped over the lazy doggo"};
  constexpr int window_width = 20;
  constexpr int col_off = 5;
  screen_buffer buf;

  print_line_of_document(line, buf, window_width, col_off);

  ASSERT_THAT(buf.size(), ::testing::Eq(window_width));
}

}   // namespace detail

}   // namespace kilo::editor
