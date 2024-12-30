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

#include "ScreenBuffer/ScreenBuffer.hpp"

#include "File/File.hpp"
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <system_error>

namespace Kilo::editor {

class MockFile : public IO::File
{
public:
  MOCK_METHOD(std::size_t, write, (int, std::string const&), (noexcept));
};

class ScreenBufferTest : public ::testing::Test
{
public:
  ScreenBuffer buf;
};

TEST_F(ScreenBufferTest, IsEmptyWhenCreated)
{
  ASSERT_EQ(buf.size(), 0);
}

TEST_F(ScreenBufferTest, ItsSizeIncreasesByTheLengthOfTheAppendedString)
{
  char const* str = "Hello, World!";
  buf.write(str, std::strlen(str));

  ASSERT_EQ(buf.size(), 13);
}

TEST_F(ScreenBufferTest, c_strReturnsACStringRepresentationOfTheContentsOfTheBuffer)
{
  char const* str = "The quick brown fox jumped over the lazy dog";
  buf.write(str, std::strlen(str));

  ASSERT_STREQ(buf.c_str(), str);
}

TEST_F(ScreenBufferTest, flushReturnsTheNumberOfBytesWrittenOnSuccess)
{
  using namespace ::testing;

  MockFile file;

  EXPECT_CALL(file, write(_, _)).Times(1).WillOnce(Return(5));

  auto rv = buf.flush(file);

  ASSERT_THAT(rv, Eq(5));
}

TEST_F(ScreenBufferTest, flushThrowsAnExceptionOnFailure)
{
  using namespace ::testing;

  MockFile file;
  EXPECT_CALL(file, write(_, _)).Times(1).WillOnce(Throw(std::system_error {}));

  ASSERT_THROW(buf.flush(file), std::system_error);
}

}   // namespace Kilo::editor
