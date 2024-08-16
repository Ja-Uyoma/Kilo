#include "Window/Window.hpp"

#include <gtest/gtest.h>

namespace Kilo::window {

TEST(Window, MembersAreInitializedToZeroByDefault)
{
  Window window {};

  ASSERT_EQ(window.rows, 0);
  ASSERT_EQ(window.cols, 0);
}

}   // namespace Kilo::window
