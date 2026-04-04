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

#include <system_error>

#include <gsl/util>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <termios.h>
#include <unistd.h>

namespace kilo::terminal::inline v1 {

TEST(TerminalMode, SetRawModeSetsTerminalModeToRawMode)
{
  terminal_mode state {};

  auto cleanup = gsl::finally([&state]() -> void { state.set_canonical_mode(); });

  ASSERT_NO_THROW(state.set_raw_mode());
  ASSERT_THAT(state.get_mode(), testing::Eq(terminal_mode::tty_mode::raw));
}

TEST(TerminalMode, SetCanonicalModeSetsTerminalModeToCanonicalMode)
{
  terminal_mode tstate {};
  tstate.set_raw_mode();

  ASSERT_NO_THROW(tstate.set_canonical_mode());
  ASSERT_THAT(tstate.get_mode(), testing::Eq(terminal_mode::tty_mode::canonical));
}

namespace detail {

TEST(TerminalMode, GetTerminalDriverSettingsThrowsAnExceptionWhenGivenABadFileDescriptor)
{
  ASSERT_THROW({
    termios buf {};
    get_terminal_driver_settings(-1, buf);
  }, std::system_error);
}

TEST(TerminalMode, GetTerminalDriverSettingsRunsSuccessfullyWhenPassedAValidFileDescriptor)
{
  termios buf {};
  ASSERT_NO_THROW(get_terminal_driver_settings(STDIN_FILENO, buf));
}

TEST(TerminalMode, TtyRawThrowsAnExceptionWhenGivenABadFileDescriptor)
{
  ASSERT_THROW({
    termios const buf{};
    termios copy {};

    tty_raw(-1, buf, copy);
  }, std::system_error);
}

TEST(TerminalMode, TtyRawSucceedsWhenPassedAValidFileDescriptor)
{
  termios buf {};
  termios copy {};

  auto cleanup = gsl::finally([&buf]() -> void { tty_canonical_mode(STDIN_FILENO, buf); });
  get_terminal_driver_settings(STDIN_FILENO, buf);

  ASSERT_NO_THROW(tty_raw(STDIN_FILENO, buf, copy));
}

TEST(TerminalMode, TtyCanonicalModeThrowsAnExceptionWhenGivenABadFileDescriptor)
{
  ASSERT_THROW({
    termios const buf {};
    tty_canonical_mode(-1, buf);
  }, std::system_error);
}

TEST(TerminalMode, TtyCanonicalModeSucceedsWhenPassedAValidFileDescriptor)
{
  termios buf {};
  get_terminal_driver_settings(STDIN_FILENO, buf);

  ASSERT_NO_THROW(tty_canonical_mode(STDIN_FILENO, buf));
}

}   // namespace detail

}  // namespace kilo::terminal::inline v1
