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

TEST(ScreenBufferTest, IsEmptyWhenCreated)
{
  ScreenBuffer buffer;
  ASSERT_EQ(buffer.size(), 0);
}

TEST(ScreenBufferTest, ItsSizeIncreasesByTheLengthOfTheAppendedString)
{
  ScreenBuffer buffer;
  char const* str = "Hello, World!";

  buffer.write(str, std::strlen(str));

  ASSERT_EQ(buffer.size(), 13);
}

class MockFileInterface : public IO::FileInterface
{
public:
  MOCK_METHOD(std::size_t, write, (int, std::string const&), (noexcept, override));
  MOCK_METHOD(std::size_t, read, (int, std::string&), (noexcept, override));
};

TEST(ScreenBufferTest, flushReturnsTheNumberOfBytesWrittenOnSuccess)
{
  using namespace ::testing;

  MockFileInterface file;
  ScreenBuffer buffer;
  buffer.write("Hello, world!");

  EXPECT_CALL(file, write(STDOUT_FILENO, std::string("Hello, world!"))).WillOnce(Return(13));
  auto rv = buffer.flush(file);

  ASSERT_THAT(rv, Eq(13));
}

TEST(ScreenBufferTest, flushThrowsAnExceptionOnFailure)
{
  using namespace ::testing;

  ScreenBuffer buffer;
  MockFileInterface file;

  EXPECT_CALL(file, write(_, _)).Times(1).WillOnce(Throw(std::system_error {}));

  ASSERT_THROW(buffer.flush(file), std::system_error);
}

}   // namespace Kilo::editor
