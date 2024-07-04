#include "Cursor.hpp"

#include <gtest/gtest.h>

namespace Kilo {

TEST(Cursor, MembersAreInitializedToZeroByDefault)
{
  Cursor c;

  ASSERT_EQ(c.x, 0);
  ASSERT_EQ(c.y, 0);
}

}   // namespace Kilo
