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

#include "Editor/Cursor.hpp"
#include "Editor/Offset.hpp"
#include "ScreenBuffer/ScreenBuffer.hpp"
#include "Utilities/Utilities.hpp"
#include "Window/Window.hpp"
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace Kilo::editor {

TEST(processKeypress, TerminatesTheProgramIfQIsPressed)
{
  auto key = utilities::ctrlKey('q');
  ASSERT_EXIT(detail::processKeypressHelper(key), ::testing::ExitedWithCode(0), ::testing::Eq(""));
}

TEST(processKeypress, MovesCursorToStartOfLineIfHomeButtonIsPressed)
{
  using utilities::EditorKey;
  using window::Window;

  EditorKey const key = EditorKey::Home;
  Cursor cursor {100, 100};
  Window const window {50, 50};

  detail::processKeypressHelper(key, cursor, window);

  ASSERT_THAT(cursor.x, ::testing::Eq(0));
}

TEST(processKeypress, MovesCursorToEndOfLineIfEndButtonIsPressed)
{
  using utilities::EditorKey;
  using window::Window;

  EditorKey const key = EditorKey::End;
  Cursor cursor {100, 100};
  Window const window {50, 50};

  detail::processKeypressHelper(key, cursor, window);

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

TEST(getCurrentRow, ReturnsTheCurrentRowOnWhichTheCursorIsLocated)
{
  Cursor cursor {.x = 0, .y = 0};
  std::vector<std::string> rows({"The quick brown fox", "jumped over the lazy dog"});

  auto currentRow = detail::getCurrentRow(cursor, rows);

  ASSERT_THAT(currentRow.has_value(), ::testing::Eq(true));
}

TEST(getCurrentRow, ReturnsAnEmptyOptionalIfTheCursorIsOutOfBounds)
{
  Cursor cursor {.x = 0, .y = 4};
  std::vector<std::string> rows({"The quick brown fox", "jumped over the lazy dog"});

  auto currentRow = detail::getCurrentRow(cursor, rows);

  ASSERT_THAT(currentRow.has_value(), ::testing::Eq(false));
}

TEST(getCurrentRow, ReturnsAnEmptyOptionalIfTheRowsObjectIsEmpty)
{
  Cursor cursor {.x = 0, .y = 4};
  std::vector<std::string> rows {};

  auto currentRow = detail::getCurrentRow(cursor, rows);

  ASSERT_THAT(currentRow.has_value(), ::testing::Eq(false));
}

TEST(moveCursorHelper, ArrowLeftMovesTheCursorLeftOneByPosition)
{
  using utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 1, .y = 0};
  EditorKey key {ArrowLeft};
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  detail::moveCursorHelper(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(0));
}

TEST(moveCursorHelper, ArrowLeftMovesTheCursorToTheEndOfTheLine)
{
  using utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 0, .y = 1};
  EditorKey key = ArrowLeft;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  detail::moveCursorHelper(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(std::strlen("The quick brown fox")));
  ASSERT_THAT(cursor.y, ::testing::Eq(0));
}

TEST(moveCursorHelper, ArrowRightMovesTheCursorRightByOnePosition)
{
  using utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 1};
  EditorKey key = ArrowRight;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  detail::moveCursorHelper(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(3));
  ASSERT_THAT(cursor.y, ::testing::Eq(1));
}

TEST(moveCursorHelper, ArrowRightMovesTheCursorToTheNextLine)
{
  using utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor;
  cursor.x = std::strlen("jumped over the lazy dog");
  cursor.y = 1;
  EditorKey key = ArrowRight;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  detail::moveCursorHelper(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(0));
  ASSERT_THAT(cursor.y, ::testing::Eq(2));
}

TEST(moveCursorHelper, ArrowUpMovesTheCursorUpByOnePosition)
{
  using utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 4};
  EditorKey key = ArrowUp;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  detail::moveCursorHelper(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(2));
  ASSERT_THAT(cursor.y, ::testing::Eq(3));
}

TEST(moveCursorHelper, ArrowUpIsANoOpIfTheCursorIsAtTheTopOfTheDocument)
{
  using utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 0};
  EditorKey key = ArrowUp;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  detail::moveCursorHelper(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(2));
  ASSERT_THAT(cursor.y, ::testing::Eq(0));
}

TEST(moveCursorHelper, ArrowDownMovesTheCursorDownByOnePosition)
{
  using utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 1};
  EditorKey key = ArrowDown;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  detail::moveCursorHelper(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(2));
  ASSERT_THAT(cursor.y, ::testing::Eq(2));
}

TEST(moveCursorHelper, ArrowDownIsANoOpIfTheCursorIsAtTheBottomOfTheDocument)
{
  using utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 2};
  EditorKey key = ArrowDown;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  detail::moveCursorHelper(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(2));
  ASSERT_THAT(cursor.y, ::testing::Eq(2));
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

}   // namespace detail

}   // namespace Kilo::editor
