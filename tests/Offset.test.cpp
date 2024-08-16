#include "Offset/Offset.hpp"

#include <gtest/gtest.h>

namespace Kilo {

TEST(Offset, MembersAreInitializedToZeroByDefault)
{
  Offset off;

  ASSERT_EQ(off.row, 0);
  ASSERT_EQ(off.col, 0);
}

}   // namespace Kilo
