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

#include "kilo/terminal/TerminalMode/TerminalMode.hpp"

#include <gsl/util>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <termios.h>
#include <unistd.h>

namespace kilo::terminal::inline v1 {

TEST(TerminalMode, SetRawModeSetsTerminalModeToRawMode)
{
  terminal_mode state {};
  ASSERT_TRUE(state.initialize());

  auto cleanup = gsl::finally([&state]() -> void {
    auto result = state.set_canonical_mode();
    (void)result;
  });

  ASSERT_TRUE(state.set_raw_mode());
  ASSERT_THAT(state.get_mode(), testing::Eq(terminal_mode::tty_mode::raw));
}

TEST(TerminalMode, SetCanonicalModeSetsTerminalModeToCanonicalMode)
{
  terminal_mode tstate {};
  ASSERT_TRUE(tstate.initialize());
  ASSERT_TRUE(tstate.set_raw_mode());

  ASSERT_TRUE(tstate.set_canonical_mode());
  ASSERT_THAT(tstate.get_mode(), testing::Eq(terminal_mode::tty_mode::canonical));
}

namespace detail {

TEST(TerminalMode, GetTerminalDriverSettingsReturnsAnErrorWhenGivenABadFileDescriptor)
{
  termios buf {};
  auto result = get_terminal_driver_settings(-1, buf);
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error().code.value(), EBADF);
}

TEST(TerminalMode, GetTerminalDriverSettingsRunsSuccessfullyWhenPassedAValidFileDescriptor)
{
  termios buf {};
  ASSERT_TRUE(get_terminal_driver_settings(STDIN_FILENO, buf));
}

TEST(TerminalMode, TtyRawReturnsAnErrorWhenGivenABadFileDescriptor)
{
  termios const buf {};
  termios copy {};
  auto result = tty_raw(-1, buf, copy);
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error().code.value(), EBADF);
}

TEST(TerminalMode, TtyRawSucceedsWhenPassedAValidFileDescriptor)
{
  termios buf {};
  termios copy {};

  ASSERT_TRUE(get_terminal_driver_settings(STDIN_FILENO, buf));
  auto cleanup = gsl::finally([&buf]() -> void {
    auto result = tty_canonical_mode(STDIN_FILENO, buf);
    (void)result;
  });

  ASSERT_TRUE(tty_raw(STDIN_FILENO, buf, copy));
}

TEST(TerminalMode, TtyCanonicalModeReturnsAnErrorWhenGivenABadFileDescriptor)
{
  termios const buf {};
  auto result = tty_canonical_mode(-1, buf);
  ASSERT_FALSE(result);
  EXPECT_EQ(result.error().code.value(), EBADF);
}

TEST(TerminalMode, TtyCanonicalModeSucceedsWhenPassedAValidFileDescriptor)
{
  termios buf {};
  ASSERT_TRUE(get_terminal_driver_settings(STDIN_FILENO, buf));

  ASSERT_TRUE(tty_canonical_mode(STDIN_FILENO, buf));
}

}   // namespace detail

}  // namespace kilo::terminal::inline v1
