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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "WriteBuffer/ScreenBuffer.hpp"
#include <cstring>
#include <gtest/gtest.h>

namespace Kilo::editor {

class WriteBufferTest : public ::testing::Test
{
public:
  WriteBuffer buf;
};

TEST_F(WriteBufferTest, IsEmptyWhenCreated)
{
  ASSERT_EQ(buf.size(), 0);
}

TEST_F(WriteBufferTest, ItsSizeIncreasesByTheLengthOfTheAppendedString)
{
  char const* str = "Hello, World!";
  buf.write(str, std::strlen(str));

  ASSERT_EQ(buf.size(), 13);
}

TEST_F(WriteBufferTest, c_strReturnsACStringRepresentationOfTheContentsOfTheBuffer)
{
  char const* str = "The quick brown fox jumped over the lazy dog";
  buf.write(str, std::strlen(str));

  ASSERT_STREQ(buf.c_str(), str);
}

}   // namespace Kilo::editor
