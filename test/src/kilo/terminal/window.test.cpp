#include "kilo/terminal/Window/Window.hpp"

#include "kilo/io/File.hpp"
#include <sys/ioctl.h>
#include <system_error>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unistd.h>

namespace kilo::terminal {

// NOLINTBEGIN(modernize-use-trailing-return-type)

class mock_file : public io::file_interface
{
public:
  MOCK_METHOD(int64_t, write, (int, std::string const&), (noexcept, override));
  MOCK_METHOD(int64_t, write, (int, std::string const&, std::size_t), (noexcept, override));
  MOCK_METHOD(int64_t, read, (int, std::string&), (noexcept, override));
  MOCK_METHOD(int64_t, read, (int, std::string&, std::size_t), (noexcept, override));
  MOCK_METHOD(int64_t, ioctl, (int, std::uint64_t, void*), (noexcept, override));
};

// NOLINTEND(modernize-use-trailing-return-type)

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

TEST(Window, WindowAccessorsReturnExpectedValues)
{
  constexpr auto win = window({.cols = 64, .rows = 32});
  ASSERT_THAT(win.cols(), ::testing::Eq(64));
  ASSERT_THAT(win.rows(), ::testing::Eq(32));
}

TEST(Window, GetWindowSizeReturnsTheWindowSizeOnSuccess)
{
  mock_file mfile;
  ::winsize winsz {};

  EXPECT_CALL(mfile, ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsz))
    .WillOnce([&winsz](int, std::uint64_t, void*) -> std::int64_t {
      winsz.ws_col = 80;
      winsz.ws_row = 60;
      return 0;
    });

  ASSERT_NO_THROW({ detail::get_window_size(mfile, winsz); });
  ASSERT_EQ(winsz.ws_col, 80);
  ASSERT_EQ(winsz.ws_row, 60);
}

TEST(Window, GetWindowSizeThrowsAnExceptionWhenIoctlAndWriteFail)
{
  mock_file mfile;
  ::winsize winsz {};
  static constexpr std::string move_cursor_bottom_right("\x1b[999c\x1b[999B");

  EXPECT_CALL(mfile, ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsz)).WillOnce([](int, std::uint64_t, void*) -> std::int64_t {
    errno = EBADF;
    return -1;
  });

  EXPECT_CALL(mfile, write(STDOUT_FILENO, move_cursor_bottom_right)).WillOnce(::testing::Return(0));

  ASSERT_THROW({ detail::get_window_size(mfile, winsz); }, std::system_error);
}

TEST(Window, GetWindowSizeThrowsAnExceptionWhenWindowColsIsZeroAndWriteFails)
{
  mock_file mfile;
  ::winsize winsz {};
  static constexpr std::string move_cursor_bottom_right("\x1b[999c\x1b[999B");

  EXPECT_CALL(mfile, ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsz))
    .WillOnce([&winsz](int, std::uint64_t, void*) -> std::int64_t {
      winsz.ws_col = 0;
      return 0;
    });

  EXPECT_CALL(mfile, write(STDOUT_FILENO, move_cursor_bottom_right)).WillOnce(::testing::Return(0));

  ASSERT_THROW({ detail::get_window_size(mfile, winsz); }, std::system_error);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

}   // namespace kilo::terminal
