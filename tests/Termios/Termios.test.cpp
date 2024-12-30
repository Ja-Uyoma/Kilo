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

#include "Termios/Termios.hpp"

#include <gsl/util>
#include <gtest/gtest.h>

namespace Kilo::Terminal {

TEST(getTerminalDriverSettings, Throws_An_Exception_When_Passed_An_Invalid_File_Descriptor)
{
  ::termios buf;
  int fd = -STDOUT_FILENO;

  ASSERT_THROW(getTerminalDriverSettings(fd, buf), std::system_error);
}

TEST(getTerminalDriverSettings, Runs_Successfully_When_Passed_A_Valid_File_Descriptor)
{
  ::termios buf;
  int fd = STDIN_FILENO;

  ASSERT_NO_THROW(getTerminalDriverSettings(fd, buf));
}

TEST(ttyRaw, Throws_An_Exception_When_Passed_An_Invalid_File_Descriptor)
{
  ::termios buf;
  ::termios copy;
  int fd = STDOUT_FILENO;

  getTerminalDriverSettings(fd, buf);
  ASSERT_THROW(ttyRaw(-fd, buf, copy), std::system_error);
}

TEST(ttyRaw, Succeeds_When_Passed_A_Valid_File_Descriptor)
{
  ::termios buf;
  ::termios copy;
  int fd = STDIN_FILENO;
  auto cleanup = gsl::finally([&fd, &buf] { ttyReset(fd, buf); });

  getTerminalDriverSettings(fd, buf);

  ASSERT_NO_THROW(ttyRaw(fd, buf, copy));
}

TEST(ttyReset, Throws_An_Exception_When_Passed_An_Invalid_File_Descriptor)
{
  ::termios buf;
  int fd = STDOUT_FILENO;

  getTerminalDriverSettings(fd, buf);

  ASSERT_THROW(ttyReset(-fd, buf), std::system_error);
}

TEST(ttyReset, Succeeds_When_Passed_A_Valid_File_Descriptor)
{
  ::termios buf;
  int fd = STDOUT_FILENO;

  getTerminalDriverSettings(fd, buf);

  ASSERT_NO_THROW(ttyReset(fd, buf));
}

}   // namespace Kilo::Terminal
