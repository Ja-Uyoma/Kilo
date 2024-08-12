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

#include "Cursor.hpp"
#include "Utilities.hpp"
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace Kilo::Editor {

TEST(getCurrentRow, ReturnsTheCurrentRowOnWhichTheCursorIsLocated)
{
  Cursor cursor {.x = 0, .y = 0};
  std::vector<std::string> rows({"The quick brown fox", "jumped over the lazy dog"});

  auto currentRow = getCurrentRow(cursor, rows);

  ASSERT_THAT(currentRow.has_value(), ::testing::Eq(true));
}

TEST(getCurrentRow, ReturnsAnEmptyOptionalIfTheCursorIsOutOfBounds)
{
  Cursor cursor {.x = 0, .y = 4};
  std::vector<std::string> rows({"The quick brown fox", "jumped over the lazy dog"});

  auto currentRow = getCurrentRow(cursor, rows);

  ASSERT_THAT(currentRow.has_value(), ::testing::Eq(false));
}

TEST(getCurrentRow, ReturnsAnEmptyOptionalIfTheRowsObjectIsEmpty)
{
  Cursor cursor {.x = 0, .y = 4};
  std::vector<std::string> rows {};

  auto currentRow = getCurrentRow(cursor, rows);

  ASSERT_THAT(currentRow.has_value(), ::testing::Eq(false));
}

TEST(moveCursor, ArrowLeftMovesTheCursorLeftOneByPosition)
{
  using Utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 1, .y = 0};
  EditorKey key {ArrowLeft};
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  moveCursor(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(0));
}

TEST(moveCursor, ArrowLeftMovesTheCursorToTheEndOfTheLine)
{
  using Utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 0, .y = 1};
  EditorKey key = ArrowLeft;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  moveCursor(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(std::strlen("The quick brown fox")));
  ASSERT_THAT(cursor.y, ::testing::Eq(0));
}

TEST(moveCursor, ArrowRightMovesTheCursorRightByOnePosition)
{
  using Utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 1};
  EditorKey key = ArrowRight;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  moveCursor(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(3));
  ASSERT_THAT(cursor.y, ::testing::Eq(1));
}

TEST(moveCursor, ArrowRightMovesTheCursorToTheNextLine)
{
  using Utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor;
  cursor.x = std::strlen("jumped over the lazy dog");
  cursor.y = 1;
  EditorKey key = ArrowRight;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  moveCursor(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(0));
  ASSERT_THAT(cursor.y, ::testing::Eq(2));
}

TEST(moveCursor, ArrowUpMovesTheCursorUpByOnePosition)
{
  using Utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 4};
  EditorKey key = ArrowUp;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  moveCursor(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(2));
  ASSERT_THAT(cursor.y, ::testing::Eq(3));
}

TEST(moveCursor, ArrowUpIsANoOpIfTheCursorIsAtTheTopOfTheDocument)
{
  using Utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 0};
  EditorKey key = ArrowUp;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  moveCursor(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(2));
  ASSERT_THAT(cursor.y, ::testing::Eq(0));
}

TEST(moveCursor, ArrowDownMovesTheCursorDownByOnePosition)
{
  using Utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 1};
  EditorKey key = ArrowDown;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  moveCursor(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(2));
  ASSERT_THAT(cursor.y, ::testing::Eq(2));
}

TEST(moveCursor, ArrowDownIsANoOpIfTheCursorIsAtTheBottomOfTheDocument)
{
  using Utilities::EditorKey;
  using enum EditorKey;

  Cursor cursor {.x = 2, .y = 2};
  EditorKey key = ArrowDown;
  std::vector<std::string> document({"The quick brown fox", "jumped over the lazy dog"});

  moveCursor(cursor, key, document);

  ASSERT_THAT(cursor.x, ::testing::Eq(2));
  ASSERT_THAT(cursor.y, ::testing::Eq(2));
}

}   // namespace Kilo::Editor
