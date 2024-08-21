#include "Window/Window.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Kilo::window {

TEST(Window, RowsReturnsTheNumberOfRows)
{
  Window w;

  ASSERT_THAT(w.rows(), ::testing::Gt(0));
}

TEST(Window, ColsReturnsTheNumberOfColumns)
{
  Window w;

  ASSERT_THAT(w.cols(), ::testing::Gt(0));
}

}   // namespace Kilo::window
