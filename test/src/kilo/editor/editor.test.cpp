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

#include "kilo/editor/Editor.hpp"

#include "kilo/editor/EditorConfig/EditorConfig.hpp"
#include "kilo/editor/ScreenBuffer/ScreenBuffer.hpp"
#include "kilo/terminal/Window/Window.hpp"
#include "kilo/utilities/Constants.hpp"
#include "kilo/utilities/Utilities.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstring>
#include <string>
#include <vector>

namespace kilo::editor {

TEST(Editor, ProcessKeypressTerminatesTheProgramIfQIsPressed)
{
  using utilities::editor_key;

  editor_config editor_config;
  constexpr auto key = utilities::ctrl_key('q');

  ASSERT_EXIT(process_keypress(key, editor_config), ::testing::ExitedWithCode(EXIT_SUCCESS), ::testing::Eq(""));
}

TEST(Editor, ProcessKeypressMovesCursorToStartOfLineIfHomeButtonIsPressed)
{
  using terminal::window;
  using utilities::editor_key;

  static constexpr int32_t cols = 64;
  static constexpr int32_t rows = 32;

  constexpr auto key = editor_key::home;
  editor_config editor_config {.window = window({.cols = cols, .rows = rows}),
                               .curs = {},
                               .off = {},
                               .screen_buf = screen_buffer(),
                               .open_doc = std::vector<std::string>(),
                               .render = std::vector<std::string>()};

  process_keypress(static_cast<int>(key), editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
}

TEST(Editor, ProcessKeypressMovesCursorToEndOfLineIfEndButtonIsPressed)
{
  using terminal::window;
  using utilities::editor_key;

  static constexpr int32_t cols = 64;
  static constexpr int32_t rows = 32;

  constexpr auto key = editor_key::end;
  editor_config editor_config {.window = window({.cols = cols, .rows = rows}),
                               .curs = {},
                               .off = {},
                               .screen_buf = screen_buffer(),
                               .open_doc = std::vector<std::string>(),
                               .render = std::vector<std::string>()};

  process_keypress(static_cast<int>(key), editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(editor_config.window.cols() - 1));
}

namespace detail {

TEST(Editor, PrintWelcomeMessagePrintsTheCorrectMessageCentred)
{
  constexpr int width = 50;
  screen_buffer buf {};

  print_welcome_message(width, buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  auto const padding = (width - msg.length()) / 2;
  std::string const output = "~" + std::string(padding - 1, ' ') + msg;

  ASSERT_THAT(buf.c_str(), ::testing::Eq(output));
}

TEST(Editor, PrintWelcomeMessageTruncatesTheMessageIfItsTooLong)
{
  constexpr int width = 25;
  screen_buffer buf {};

  print_welcome_message(width, buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  std::string const truncated_msg = msg.substr(0, width);

  ASSERT_THAT(buf.c_str(), ::testing::Eq(truncated_msg));
}

TEST(Editor, PrintLineOfDocumentPrintsNothingWhenTheLineLengthIsLessThanTheColumnOffset)
{
  std::string const line {"The quick brown fox jumped over the lazy doggo"};
  constexpr int window_width = 20;
  auto const col_off = std::ssize(line) + 5;
  screen_buffer buf;

  print_line_of_document(line, buf, window_width, col_off);

  ASSERT_THAT(buf.size(), ::testing::Eq(0));
}

TEST(Editor, PrintLineOfDocumentTruncatesTheLineIfItsLongerThanWindowWidth)
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
