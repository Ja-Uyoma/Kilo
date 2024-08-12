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

}   // namespace Kilo::Editor
