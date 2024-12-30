#include "Termios/Termios.hpp"

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

}   // namespace Kilo::Terminal
