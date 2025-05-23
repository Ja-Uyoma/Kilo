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

#include "Terminal/TerminalMode/TerminalMode.hpp"

#include <gsl/util>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <termios.h>
#include <unistd.h>

namespace Kilo::Terminal {

TEST(TerminalMode, SetRawModePutsTheTerminalDriverInRawMode)
{
  TerminalMode state {};

  auto cleanup = gsl::finally([&state] { state.setCanonicalMode(); });

  ASSERT_NO_THROW(state.setRawMode());
  ASSERT_THAT(state.getState(), testing::Eq(TerminalMode::ttystate::Raw));
}

TEST(TerminalMode, ResetRestoresTerminalSettingsToCanonicalMode)
{
  TerminalMode tstate {};
  tstate.setRawMode();

  ASSERT_NO_THROW(tstate.setCanonicalMode());
  ASSERT_THAT(tstate.getState(), testing::Eq(TerminalMode::ttystate::Canonical));
}

namespace detail {

TEST(getTerminalDriverSettings, TerminatesWhenPassedAnInvalidFileDescriptor)
{
  termios buf {};
  ASSERT_DEATH(getTerminalDriverSettings(STDOUT_FILENO, buf), "File descriptor must be STDIN_FILENO");
}

TEST(getTerminalDriverSettings, RunsSuccessfullyWhenPassedAValidFileDescriptor)
{
  termios buf {};
  ASSERT_NO_THROW(getTerminalDriverSettings(STDIN_FILENO, buf));
}

TEST(ttyRaw, TerminatesWhenPassedAnInvalidFileDescriptor)
{
  termios const buf {};
  termios copy {};

  ASSERT_DEATH(ttyRaw(STDOUT_FILENO, buf, copy), "File descriptor must be STDIN_FILENO");
}

TEST(ttyRaw, SucceedsWhenPassedAValidFileDescriptor)
{
  termios buf {};
  termios copy {};

  auto cleanup = gsl::finally([&buf] { ttyCanonicalMode(STDIN_FILENO, buf); });
  getTerminalDriverSettings(STDIN_FILENO, buf);

  ASSERT_NO_THROW(ttyRaw(STDIN_FILENO, buf, copy));
}

TEST(ttyCanonicalMode, TerminatesWhenPassedAnInvalidFileDescriptor)
{
  termios const buf {};
  ASSERT_DEATH(ttyCanonicalMode(STDOUT_FILENO, buf), "File descriptor must be STDIN_FILENO");
}

TEST(ttyCanonicalMode, SucceedsWhenPassedAValidFileDescriptor)
{
  termios buf {};
  getTerminalDriverSettings(STDIN_FILENO, buf);

  ASSERT_NO_THROW(ttyCanonicalMode(STDIN_FILENO, buf));
}

}   // namespace detail

}   // namespace Kilo::Terminal
