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

// NOLINTBEGIN(*-magic-numbers)

TEST(Editor, ProcessKeypressTerminatesTheProgramIfQIsPressed)
{
  using utilities::editor_key;

  editor_config editor_config;
  constexpr auto key = utilities::ctrl_key('q');

  ASSERT_EXIT(process_keypress(key, editor_config), ::testing::ExitedWithCode(EXIT_SUCCESS), ::testing::Eq(""));
}

TEST(Editor, ProcessKeypressMovesCursorToStartOfLineIfHomeButtonIsPressed)
{
  using utilities::editor_key;

  static constexpr int32_t cols = 64;
  static constexpr int32_t rows = 32;

  constexpr auto key = editor_key::home;
  editor_config editor_config {
    .winsize = {.cols = cols, .rows = rows},
      .curs = {},
      .off = {},
      .abuf = append_buffer(), .row = {}
  };

  process_keypress(static_cast<int>(key), editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
}

TEST(Editor, ProcessKeypressMovesCursorToEndOfLineIfEndButtonIsPressed)
{
  using utilities::editor_key;

  static constexpr int32_t cols = 64;
  static constexpr int32_t rows = 32;

  constexpr auto key = editor_key::end;
  editor_config editor_config {
    .winsize = {.cols = cols, .rows = rows},
      .curs = {},
      .off = {},
      .abuf = append_buffer(), .row = {}
  };

  process_keypress(static_cast<int>(key), editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(editor_config.winsize.cols - 1));
}

TEST(Editor, MoveCursorArrowLeftDecrementsXWhenNotAtStart)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 5, .y = 0},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello world", "Hello world"}}
  };

  move_cursor(utilities::editor_key::arrow_left, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(4));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(Editor, MoveCursorArrowLeftMovesToEndOfPreviousLineWhenAtStartOfLine)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 0, .y = 1},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello", "Hello"}, {"world", "world"}}
  };

  move_cursor(utilities::editor_key::arrow_left, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(5));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(Editor, MoveCursorArrowLeftStaysAtStartWhenAlreadyAtBeginningOfDocument)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 0, .y = 0},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello world", "Hello world"}}
  };

  move_cursor(utilities::editor_key::arrow_left, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(Editor, MoveCursorArrowRightIncrementsXWhenNotAtEndOfLine)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 3, .y = 0},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello world", "Hello world"}}
  };

  move_cursor(utilities::editor_key::arrow_right, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(4));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(Editor, MoveCursorArrowRightMovesToStartOfNextLineWhenAtEndOfLine)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 5, .y = 0},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello", "Hello"}, {"world", "world"}}
  };

  move_cursor(utilities::editor_key::arrow_right, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
}

TEST(Editor, MoveCursorArrowRightStaysAtEndWhenAtEndOfDocument)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 11, .y = 1},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello world", "Hello world"}}
  };

  move_cursor(utilities::editor_key::arrow_right, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
}

TEST(Editor, MoveCursorArrowUpDecrementsYWhenNotAtTopOfDocument)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 3, .y = 2},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello", "Hello"}, {"world", "world"}, {"foo", "foo"}}
  };

  move_cursor(utilities::editor_key::arrow_up, editor_config);

  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(3));
}

TEST(Editor, MoveCursorArrowUpStaysAtTopWhenAlreadyAtFirstLine)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 3, .y = 0},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello", "Hello"}, {"world", "world"}, {"foo", "foo"}}
  };

  move_cursor(utilities::editor_key::arrow_up, editor_config);

  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(Editor, MoveCursorArrowDownIncrementsYWhenNotAtBottomOfDocument)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 3, .y = 0},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello", "Hello"}, {"world", "world"}, {"foo", "foo"}}
  };

  move_cursor(utilities::editor_key::arrow_down, editor_config);

  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(3));
}

TEST(Editor, MoveCursorArrowDownStaysAtBottomWhenAlreadyAtLastLine)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 3, .y = 3},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello", "Hello"}, {"world", "world"}, {"foo", "foo"}}
  };

  move_cursor(utilities::editor_key::arrow_down, editor_config);

  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(3));
}

TEST(Editor, MoveCursorClampsXWhenMovingToShorterLine)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 10, .y = 0},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello world this is longer", "Hello world this is longer"}, {"hi", "hi"}}
  };

  move_cursor(utilities::editor_key::arrow_down, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(2));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
}

TEST(Editor, MoveCursorClampsXWhenMovingBeyondOpenDocBounds)
{
  editor_config editor_config {
    .winsize = {.cols = 80, .rows = 24},
    .curs = {.x = 5, .y = 2},
    .off = {},
    .abuf = append_buffer(),
    .row = {{"Hello", "Hello"}}
  };

  move_cursor(utilities::editor_key::arrow_down, editor_config);

  // After vertical move beyond document, x should be clamped to line length
  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
}

namespace detail {

TEST(Editor, PrintWelcomeMessagePrintsTheCorrectMessageCentred)
{
  constexpr int width = 50;
  append_buffer buf {};

  print_welcome_message(width, buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  auto const padding = (width - msg.length()) / 2;
  std::string const output = "~" + std::string(padding - 1, ' ') + msg;

  ASSERT_THAT(buf.c_str(), ::testing::Eq(output));
}

TEST(Editor, PrintWelcomeMessageTruncatesTheMessageIfItsTooLong)
{
  constexpr int width = 25;
  append_buffer buf {};

  print_welcome_message(width, buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  std::string const truncated_msg = msg.substr(0, width);

  ASSERT_THAT(buf.c_str(), ::testing::Eq(truncated_msg));
}

TEST(Editor, PrintLineOfDocumentTruncatesTheLineIfItsLongerThanWindowWidth)
{
  std::string const line {"The quick brown fox jumped over the lazy doggo"};
  constexpr int window_width = 20;
  constexpr int col_off = 5;
  append_buffer buf;

  print_line_of_document(line, buf, window_width, col_off);

  ASSERT_THAT(buf.size(), ::testing::Eq(window_width));
}

TEST(Editor, PrintLineOfDocumentPrintsFullLineWhenWindowIsWiderThanLine)
{
  std::string const line {"Hello world"};
  constexpr int window_width = 20;
  constexpr int col_off = 0;
  append_buffer buf;

  print_line_of_document(line, buf, window_width, col_off);

  ASSERT_THAT(buf.size(), ::testing::Eq(static_cast<int>(std::ssize(line))));
  ASSERT_THAT(buf.c_str(), ::testing::Eq(line));
}

TEST(Editor, PrintLineOfDocumentStartsAtColumnOffset)
{
  std::string const line {"0123456789abcdefghijklmnopqrstuvwxyz"};
  constexpr int window_width = 10;
  constexpr int col_off = 4;
  append_buffer buf;

  print_line_of_document(line, buf, window_width, col_off);

  std::string const expected = line.substr(static_cast<size_t>(col_off), static_cast<size_t>(window_width));

  ASSERT_THAT(buf.size(), ::testing::Eq(static_cast<int>(std::ssize(expected))));
  ASSERT_THAT(buf.c_str(), ::testing::Eq(expected));
}

TEST(Editor, PrintLineOfDocumentPrintsRemainingCharsWhenShorterThanWindow)
{
  std::string const line {"abcdef"};
  constexpr int window_width = 10;
  constexpr int col_off = 2;
  append_buffer buf;

  print_line_of_document(line, buf, window_width, col_off);

  std::string const expected = line.substr(static_cast<size_t>(col_off));

  ASSERT_THAT(buf.size(), ::testing::Eq(static_cast<int>(std::ssize(expected))));
  ASSERT_THAT(buf.c_str(), ::testing::Eq(expected));
}

}   // namespace detail

// NOLINTEND(*-magic-numbers)

}   // namespace kilo::editor
