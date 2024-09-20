#include "Terminal/window/window.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Kilo::terminal {

TEST(Window, RowsReturnsAPositiveNumberOfRows)
{
  Window w = Window::create();

  ASSERT_THAT(w.rows(), ::testing::Gt(0));
}

TEST(Window, ColsReturnsAPositiveNumberOfColumns)
{
  Window w = Window::create();

  ASSERT_THAT(w.cols(), ::testing::Gt(0));
}

}   // namespace Kilo::terminal
