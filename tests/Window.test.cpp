#include "Window/Window.hpp"

#include <gtest/gtest.h>

namespace Kilo::window {

TEST(Window, MembersAreInitializedToZeroByDefault)
{
  Window window {};

  ASSERT_EQ(window.m_rows, 0);
  ASSERT_EQ(window.m_cols, 0);
}

}   // namespace Kilo::window
