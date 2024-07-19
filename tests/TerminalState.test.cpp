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

#include "TerminalState.hpp"

#include <gsl/util>
#include <gtest/gtest.h>
#include <system_error>

namespace Kilo {

TEST(TerminalState, getTerminalDriverSettingsFailsWhenGivenAnInvalidFileDescriptor)
{
  termios buf;

  ASSERT_THROW(getTerminalDriverSettings(-1, buf), std::system_error);
}

TEST(TerminalState, getTerminalDriverSettingsSucceedsWhenGivenAValidFileDescriptor)
{
  termios buf;

  ASSERT_NO_THROW(getTerminalDriverSettings(0, buf));
}

TEST(TerminalState, ttyRawFailsWhenGivenAnInvalidFileDescriptor)
{
  termios buf;
  termios copy;

  getTerminalDriverSettings(STDIN_FILENO, buf);
  ASSERT_THROW(ttyRaw(-1, buf, copy), std::system_error);
}

TEST(TerminalState, ttyRawSucceedsWhenGivenAValidFileDescriptor)
{
  termios buf;
  termios copy;
  int fd = STDIN_FILENO;

  getTerminalDriverSettings(fd, buf);
  auto const cleanup = gsl::finally([&fd, &buf] { ttyReset(fd, buf); });

  ASSERT_NO_THROW(ttyRaw(fd, buf, copy));
}

TEST(TerminalState, ttyResetFailsWhenGivenAnInvalidFileDescriptor)
{
  termios buf;
  int fd = -1;

  getTerminalDriverSettings(STDIN_FILENO, buf);

  ASSERT_THROW(ttyReset(fd, buf), std::system_error);
}

TEST(TerminalState, ttyResetSucceedsWhenGivenAValidFileDescriptor)
{
  termios buf;
  int fd = STDIN_FILENO;

  getTerminalDriverSettings(fd, buf);

  ASSERT_NO_THROW(ttyReset(fd, buf));
}

TEST(TerminalState, EachInstanceIsWellFormed)
{
  ASSERT_NO_THROW(TerminalState tstate {});
}

TEST(TerminalState, SetRawModePutsTheTerminalDriverInRawMode)
{
  TerminalState state {};

  auto cleanup = gsl::finally([&state] { state.reset(); });

  ASSERT_NO_THROW(state.setRawMode());
}

TEST(TerminalState, ResetRestoresTerminalSettingsToCanonicalMode)
{
  TerminalState tstate {};
  tstate.setRawMode();

  ASSERT_NO_THROW(tstate.reset());
}

}   // namespace Kilo
