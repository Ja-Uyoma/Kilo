#include "Window/Window.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace Kilo::window {

TEST(Window, MembersAreInitializedToZeroByDefault)
{
  Window window {};

  ASSERT_EQ(window.m_rows, 0);
  ASSERT_EQ(window.m_cols, 0);
}

TEST(Window, RowsReturnsTheNumberOfRows)
{
  Window w;

  ASSERT_THAT(w.rows(), ::testing::Eq(0));
}

TEST(Window, ColsReturnsTheNumberOfColumns)
{
  Window w;

  ASSERT_THAT(w.cols(), ::testing::Eq(0));
}

}   // namespace Kilo::window
