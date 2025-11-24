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

#include "editor/Editor.hpp"

#include "editor/Cursor/Cursor.hpp"
#include "editor/ScreenBuffer/ScreenBuffer.hpp"
#include "terminal/Window/Window.hpp"
#include "utilities/Utilities.hpp"

#include "gmock/gmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstring>
#include <string>
#include <vector>

namespace kilo::editor {

TEST(processKeypress, TerminatesTheProgramIfQIsPressed)
{
  using utilities::EditorKey;

  EditorConfig editorConfig;
  constexpr auto key = utilities::ctrlKey('q');

  ASSERT_EXIT(processKeypress(key, editorConfig), ::testing::ExitedWithCode(0), ::testing::Eq(""));
}

TEST(processKeypress, MovesCursorToStartOfLineIfHomeButtonIsPressed)
{
  using utilities::EditorKey;

  constexpr auto key = EditorKey::Home;
  EditorConfig editorConfig;

  processKeypress(static_cast<int>(key), editorConfig);

  ASSERT_THAT(editorConfig.cursor.x, ::testing::Eq(0));
}

TEST(processKeypress, MovesCursorToEndOfLineIfEndButtonIsPressed)
{
  using utilities::EditorKey;

  constexpr auto key = EditorKey::End;
  EditorConfig editorConfig;

  processKeypress(static_cast<int>(key), editorConfig);

  ASSERT_THAT(editorConfig.cursor.x, ::testing::Eq(editorConfig.window.cols() - 1));
}

namespace detail {

TEST(printWelcomeMessage, PrintsTheCorrectMessageCentred)
{
  constexpr int width = 50;
  ScreenBuffer buf{};

  printWelcomeMessage(width, buf);

  std::string const msg{"Kilo editor -- version 0.0.1"};
  auto const padding = (width - msg.length()) / 2;
  std::string const output = "~" + std::string(padding - 1, ' ') + msg;

  ASSERT_THAT(buf.c_str(), ::testing::Eq(output));
}

TEST(printWelcomeMessage, TruncatesTheMessageIfItsTooLong)
{
  constexpr int width = 25;
  ScreenBuffer buf{};

  printWelcomeMessage(width, buf);

  std::string const msg{"Kilo editor -- version 0.0.1"};
  std::string const truncatedMsg = msg.substr(0, width);

  ASSERT_THAT(buf.c_str(), ::testing::Eq(truncatedMsg));
}

TEST(printLineOfDocument, PrintsNothingWhenTheLineLengthIsLessThanTheColumnOffset)
{
  std::string const line{"The quick brown fox jumped over the lazy doggo"};
  constexpr int windowWidth = 20;
  auto const colOffset = std::ssize(line) + 5;
  ScreenBuffer buf;

  printLineOfDocument(line, buf, windowWidth, colOffset);

  ASSERT_THAT(buf.size(), ::testing::Eq(0));
}

TEST(printLineOfDocument, TruncatesTheLineIfItsLongerThanWindowWidth)
{
  std::string const line{"The quick brown fox jumped over the lazy doggo"};
  constexpr int windowWidth = 20;
  constexpr int colOffset = 5;
  ScreenBuffer buf;

  printLineOfDocument(line, buf, windowWidth, colOffset);

  ASSERT_THAT(buf.size(), ::testing::Eq(windowWidth));
}

}   // namespace detail

}   // namespace kilo::editor
