#include "Terminal/window/window.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Kilo::Terminal {

TEST(Window, RowsReturnsAPositiveNumberOfRows)
{
  Window w;

  ASSERT_THAT(w.rows(), ::testing::Gt(0));
}

TEST(Window, ColsReturnsAPositiveNumberOfColumns)
{
  Window w;

  ASSERT_THAT(w.cols(), ::testing::Gt(0));
}

}   // namespace Kilo::Terminal
