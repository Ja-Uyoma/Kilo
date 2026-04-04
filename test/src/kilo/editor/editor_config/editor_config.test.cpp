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

#include "kilo/editor/editor_config/editor_config.hpp"

#include "kilo/editor/append_buffer/append_buffer.hpp"
#include "kilo/terminal/window_size/window_size.hpp"
#include "kilo/utilities/Constants.hpp"
#include "kilo/utilities/Utilities.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstring>
#include <string>
#include <vector>

namespace kilo::editor::editor_config {

// NOLINTBEGIN(*-magic-numbers)

TEST(EditorConfig, ProcessKeypressTerminatesTheProgramIfQIsPressed)
{
  using terminal::window_size::window_size;
  using utilities::editor_key;

  editor_config editor_config;
  window_size winsz {};
  constexpr auto key = utilities::ctrl_key('q');

  ASSERT_EXIT(process_keypress(key, editor_config, winsz), ::testing::ExitedWithCode(EXIT_SUCCESS), ::testing::Eq(""));
}

TEST(EditorConfig, ProcessKeypressMovesCursorToStartOfLineIfHomeButtonIsPressed)
{
  using terminal::window_size::window_size;
  using utilities::editor_key;

  constexpr auto key = editor_key::home;
  window_size winsz {.cols = 64, .rows = 32};
  editor_config editor_config {};

  process_keypress(static_cast<int>(key), editor_config, winsz);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
}

TEST(EditorConfig, ProcessKeypressDoesNotMoveCursorIfEndButtonIsPressedAndDocumentIsEmpty)
{
  using terminal::window_size::window_size;
  using utilities::editor_key;

  constexpr auto key = editor_key::end;
  window_size winsz {.cols = 64, .rows = 32};
  editor_config editor_config {};

  process_keypress(static_cast<int>(key), editor_config, winsz);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
}

TEST(EditorConfig, ProcessKeypressMovesCursorToEndOfLineIfEndButtonIsPressed)
{
  using terminal::window_size::window_size;
  using utilities::editor_key;

  constexpr auto key = editor_key::end;
  window_size winsz {.cols = 64, .rows = 32};
  editor_config editor_config {
    .curs = {},
      .row = {erow::erow("Hello world"), erow::erow("This is a hotfix")}
  };

  process_keypress(static_cast<int>(key), editor_config, winsz);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(std::strlen("Hello world")));
}

TEST(EditorConfig, MoveCursorArrowLeftDecrementsXWhenNotAtStart)
{
  editor_config editor_config {
    .curs = {.x = 5, .y = 0},
      .off = {},
      .row = {erow::erow {"Hello world"}}
  };

  move_cursor(utilities::editor_key::arrow_left, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(4));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(EditorConfig, MoveCursorArrowLeftMovesToEndOfPreviousLineWhenAtStartOfLine)
{
  editor_config editor_config {
    .curs = {.x = 0, .y = 1},
      .off = {},
      .row = {erow::erow {"Hello"}, erow::erow {"world"}}
  };

  move_cursor(utilities::editor_key::arrow_left, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(5));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(EditorConfig, MoveCursorArrowLeftStaysAtStartWhenAlreadyAtBeginningOfDocument)
{
  editor_config editor_config {
    .curs = {.x = 0, .y = 0},
      .off = {},
      .row = {erow::erow {"Hello world"}}
  };

  move_cursor(utilities::editor_key::arrow_left, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(EditorConfig, MoveCursorArrowRightIncrementsXWhenNotAtEndOfLine)
{
  editor_config editor_config {
    .curs = {.x = 3, .y = 0},
      .off = {},
      .row = {erow::erow {"Hello world"}}
  };

  move_cursor(utilities::editor_key::arrow_right, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(4));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(EditorConfig, MoveCursorArrowRightMovesToStartOfNextLineWhenAtEndOfLine)
{
  editor_config editor_config {
    .curs = {.x = 5, .y = 0},
      .off = {},
      .row = {erow::erow {"Hello"}, erow::erow {"world"}}
  };

  move_cursor(utilities::editor_key::arrow_right, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
}

TEST(EditorConfig, MoveCursorArrowRightStaysAtEndWhenAtEndOfDocument)
{
  editor_config editor_config {
    .curs = {.x = 11, .y = 1},
      .off = {},
      .row = {erow::erow {"Hello world"}}
  };

  move_cursor(utilities::editor_key::arrow_right, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
}

TEST(EditorConfig, MoveCursorArrowUpDecrementsYWhenNotAtTopOfDocument)
{
  editor_config editor_config {
    .curs = {.x = 3, .y = 2},
      .off = {},
      .row = {erow::erow {"Hello"}, erow::erow {"world"}, erow::erow {"foo"}}
  };

  move_cursor(utilities::editor_key::arrow_up, editor_config);

  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(3));
}

TEST(EditorConfig, MoveCursorArrowUpStaysAtTopWhenAlreadyAtFirstLine)
{
  editor_config editor_config {
    .curs = {.x = 3, .y = 0},
      .off = {},
      .row = {erow::erow {"Hello"}, erow::erow {"world"}, erow::erow {"foo"}}
  };

  move_cursor(utilities::editor_key::arrow_up, editor_config);

  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(0));
}

TEST(EditorConfig, MoveCursorArrowDownIncrementsYWhenNotAtBottomOfDocument)
{
  editor_config editor_config {
    .curs = {.x = 3, .y = 0},
      .off = {},
      .row = {erow::erow {"Hello"}, erow::erow {"world"}, erow::erow {"foo"}}
  };

  move_cursor(utilities::editor_key::arrow_down, editor_config);

  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(3));
}

TEST(EditorConfig, MoveCursorArrowDownStaysAtBottomWhenAlreadyAtLastLine)
{
  editor_config editor_config {
    .curs = {.x = 3, .y = 3},
      .off = {},
      .row = {erow::erow {"Hello"}, erow::erow {"world"}, erow::erow {"foo"}}
  };

  move_cursor(utilities::editor_key::arrow_down, editor_config);

  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(3));
}

TEST(EditorConfig, MoveCursorClampsXWhenMovingToShorterLine)
{
  editor_config editor_config {
    .curs = {.x = 10, .y = 0},
      .off = {},
      .row = {erow::erow {"Hello world this is longer"}, erow::erow {"hi"}}
  };

  move_cursor(utilities::editor_key::arrow_down, editor_config);

  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(2));
  ASSERT_THAT(editor_config.curs.y, ::testing::Eq(1));
}

TEST(EditorConfig, MoveCursorClampsXWhenMovingBeyondOpenDocBounds)
{
  editor_config editor_config {
    .curs = {.x = 5, .y = 2},
      .off = {},
      .row = {erow::erow {"Hello"}}
  };

  move_cursor(utilities::editor_key::arrow_down, editor_config);

  // After vertical move beyond document, x should be clamped to line length
  ASSERT_THAT(editor_config.curs.x, ::testing::Eq(0));
}

namespace detail {

TEST(EditorConfig, PrintWelcomeMessagePrintsTheCorrectMessageCentred)
{
  constexpr int width = 50;
  append_buffer::append_buffer buf {};

  print_welcome_message(width, &buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  auto const padding = (width - msg.length()) / 2;
  std::string const output = "~" + std::string(padding - 1, ' ') + msg;

  ASSERT_THAT(buf.c_str(), ::testing::Eq(output));
}

TEST(EditorConfig, PrintWelcomeMessageTruncatesTheMessageIfItsTooLong)
{
  constexpr int width = 25;
  append_buffer::append_buffer buf {};

  print_welcome_message(width, &buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  std::string const truncated_msg = msg.substr(0, width);

  ASSERT_THAT(buf.c_str(), ::testing::Eq(truncated_msg));
}

TEST(EditorConfig, PrintLineOfDocumentTruncatesTheLineIfItsLongerThanWindowWidth)
{
  std::string const line {"The quick brown fox jumped over the lazy doggo"};
  constexpr int window_width = 20;
  constexpr int col_off = 5;
  append_buffer::append_buffer buf;

  print_line_of_document(line, &buf, window_width, col_off);

  ASSERT_THAT(buf.size(), ::testing::Eq(window_width));
}

TEST(EditorConfig, PrintLineOfDocumentPrintsFullLineWhenWindowIsWiderThanLine)
{
  std::string const line {"Hello world"};
  constexpr int window_width = 20;
  constexpr int col_off = 0;
  append_buffer::append_buffer buf;

  print_line_of_document(line, &buf, window_width, col_off);

  ASSERT_THAT(buf.size(), ::testing::Eq(static_cast<int>(std::ssize(line))));
  ASSERT_THAT(buf.c_str(), ::testing::Eq(line));
}

TEST(EditorConfig, PrintLineOfDocumentStartsAtColumnOffset)
{
  std::string const line {"0123456789abcdefghijklmnopqrstuvwxyz"};
  constexpr int window_width = 10;
  constexpr int col_off = 4;
  append_buffer::append_buffer buf;

  print_line_of_document(line, &buf, window_width, col_off);

  std::string const expected = line.substr(static_cast<size_t>(col_off), static_cast<size_t>(window_width));

  ASSERT_THAT(buf.size(), ::testing::Eq(static_cast<int>(std::ssize(expected))));
  ASSERT_THAT(buf.c_str(), ::testing::Eq(expected));
}

TEST(EditorConfig, PrintLineOfDocumentPrintsRemainingCharsWhenShorterThanWindow)
{
  std::string const line {"abcdef"};
  constexpr int window_width = 10;
  constexpr int col_off = 2;
  append_buffer::append_buffer buf;

  print_line_of_document(line, &buf, window_width, col_off);

  std::string const expected = line.substr(static_cast<size_t>(col_off));

  ASSERT_THAT(buf.size(), ::testing::Eq(static_cast<int>(std::ssize(expected))));
  ASSERT_THAT(buf.c_str(), ::testing::Eq(expected));
}

}   // namespace detail

// NOLINTEND(*-magic-numbers)

}   // namespace kilo::editor::editor_config
