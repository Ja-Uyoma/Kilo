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

#include "Editor/Editor.hpp"

#include "Editor/Cursor/Cursor.hpp"
#include "Editor/Offset/Offset.hpp"
#include "Editor/ScreenBuffer/ScreenBuffer.hpp"
#include "Terminal/Window/Window.hpp"
#include "Utilities/Utilities.hpp"
#include "gmock/gmock.h"
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace Kilo::editor {

TEST(processKeypress, TerminatesTheProgramIfQIsPressed)
{
  using editor::EditorKey;
  using Terminal::Window;

  Cursor cursor {100, 100};
  Window const window;
  std::vector<std::string> const& doc {};

  auto key = utilities::ctrlKey('q');
  ASSERT_EXIT(processKeypress(key, cursor, window, doc), ::testing::ExitedWithCode(0), ::testing::Eq(""));
}

TEST(processKeypress, MovesCursorToStartOfLineIfHomeButtonIsPressed)
{
  using editor::EditorKey;
  using Terminal::Window;

  EditorKey const key = EditorKey::Home;
  Cursor cursor {100, 100};
  Window const window;
  std::vector<std::string> const& doc {};

  processKeypress(static_cast<int>(key), cursor, window, doc);

  ASSERT_THAT(cursor.x, ::testing::Eq(0));
}

TEST(processKeypress, MovesCursorToEndOfLineIfEndButtonIsPressed)
{
  using editor::EditorKey;
  using Terminal::Window;

  EditorKey const key = EditorKey::End;
  Cursor cursor {100, 100};
  Window const window;
  std::vector<std::string> const& doc {};

  processKeypress(static_cast<int>(key), cursor, window, doc);

  ASSERT_THAT(cursor.x, ::testing::Eq(window.cols() - 1));
}

namespace detail {

TEST(FixCursorToVisibleWindow, ModifiesTheOffsetIfCursorPositionIsLessThanOffsetValue)
{
  int cursorY {10};
  int offRow {20};
  int windowHeight {0};

  fixCursorToVisibleWindow(cursorY, offRow, windowHeight);

  ASSERT_THAT(offRow, ::testing::Eq(cursorY));
}

TEST(FixCursorToVisibleWindow, ModifiesTheOffsetIfCursorPositionIsGreaterOrEqualToSumOfOffsetValueAndWindowDimension)
{
  int cursorY {30};
  int offRow {20};
  int windowHeight {0};

  fixCursorToVisibleWindow(cursorY, offRow, windowHeight);

  ASSERT_THAT(offRow, ::testing::Eq(cursorY - windowHeight + 1));
}

TEST(setExactPositionToMoveCursorTo, ReturnsAStringContainingThePositionToMoveTheCursorTo)
{
  Cursor c {15, 15};
  Offset off {10, 10};

  auto pos = setExactPositionToMoveCursorTo(c, off);

  ASSERT_THAT(pos, ::testing::Eq("\x1b[6;6H"));
}

TEST(createWelcomeMessage, CreatesAWelcomeMessageContainingTheVersionString)
{
  std::string_view version {"0.0.1"};

  auto msg = createWelcomeMessage(version);

  ASSERT_THAT(msg, ::testing::Eq("Kilo editor -- version 0.0.1"));
}

TEST(resizeWelcomeMessage, ResizesTheWelcomeMessageToFitInTheWindow)
{
  std::string msg {"One must imagine Sisyphus happy"};
  int width = 5;

  resizeWelcomeMessage(msg, width);

  ASSERT_THAT(msg.length(), ::testing::Eq(width));
  ASSERT_THAT(msg, ::testing::Eq("One m"));
}

TEST(getPadding, GetsThePaddingBetweenTheEdgesOfTheWindowAndTheWelcomeMessage)
{
  int const windowWidth = 100;
  std::size_t const msgLength = 25;

  auto const padding = getPadding(windowWidth, msgLength);

  ASSERT_THAT(padding, ::testing::Eq(37));
}

TEST(writePaddingToScreenBuffer, WritesPadCharactersToTheScreenBuffer)
{
  std::size_t const padding = 10;
  ScreenBuffer buf {};

  writePaddingToScreenBuffer(padding, buf);

  ASSERT_THAT(buf.size(), ::testing::Eq(padding));
}

TEST(printWelcomeMessage, PrintsTheCorrectMessageCentred)
{
  int const width {50};
  ScreenBuffer buf {};

  printWelcomeMessage(width, buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  auto padding = getPadding(width, msg.length());
  std::string output = "~" + std::string(padding - 1, ' ') + msg;

  ASSERT_THAT(buf.c_str(), ::testing::Eq(output));
}

TEST(printWelcomeMessage, TruncatesTheMessageIfItsTooLong)
{
  int const width {25};
  ScreenBuffer buf {};

  printWelcomeMessage(width, buf);

  std::string const msg {"Kilo editor -- version 0.0.1"};
  std::string const truncatedMsg = msg.substr(0, width);

  ASSERT_THAT(buf.c_str(), ::testing::Eq(truncatedMsg));
}

TEST(printWelcomeMessageOrTilde, PrintsATildeIfTheDocumentIsNotEmpty)
{
  bool documentIsEmpty = false;
  ScreenBuffer buf;
  Terminal::Window window;
  int currentRow = window.rows() / 3;

  printWelcomeMessageOrTilde(documentIsEmpty, currentRow, buf, window);

  ASSERT_THAT(buf.size(), ::testing::Eq(1));
}

TEST(printWelcomeMessageOrTilde, PrintsATildeIfTheCurrentRowIsNotAThirdOfTheWindowHeight)
{
  bool documentIsEmpty = true;
  ScreenBuffer buf;
  Terminal::Window window;
  int currentRow = 0;

  printWelcomeMessageOrTilde(documentIsEmpty, currentRow, buf, window);

  ASSERT_THAT(buf.size(), ::testing::Eq(1));
}

TEST(printWelcomeMessageOrTilde, PrintsTheWelcomeMessage)
{
  // This is not a good test
  // Try rewriting this using EXPECT_CALL

  bool documentIsEmpty = true;
  ScreenBuffer buffer;
  Terminal::Window window;
  auto currentRow = window.rows() / 3;

  printWelcomeMessageOrTilde(documentIsEmpty, currentRow, buffer, window);

  ASSERT_THAT(buffer.size(), ::testing::Gt(1));
}

TEST(printLineOfDocument, PrintsNothingWhenTheLineLengthIsLessThanTheColumnOffset)
{
  std::string const line {"The quick brown fox jumped over the lazy doggo"};
  int const windowWidth {20};
  int const colOffset = std::ssize(line) + 5;
  ScreenBuffer buf;

  printLineOfDocument(line, buf, windowWidth, colOffset);

  ASSERT_THAT(buf.size(), ::testing::Eq(0));
}

TEST(printLineOfDocument, TruncatesTheLineIfItsLongerThanWindowWidth)
{
  std::string const line {"The quick brown fox jumped over the lazy doggo"};
  int const windowWidth {20};
  int const colOffset = 5;
  ScreenBuffer buf;

  printLineOfDocument(line, buf, windowWidth, colOffset);

  ASSERT_THAT(buf.size(), ::testing::Eq(windowWidth));
}

}   // namespace detail

}   // namespace Kilo::editor
