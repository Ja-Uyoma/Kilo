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
