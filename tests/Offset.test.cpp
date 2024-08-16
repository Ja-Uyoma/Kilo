#include "Offset/Offset.hpp"

#include <gtest/gtest.h>

namespace Kilo::offset {

TEST(Offset, MembersAreInitializedToZeroByDefault)
{
  Offset off;

  ASSERT_EQ(off.row, 0);
  ASSERT_EQ(off.col, 0);
}

}   // namespace Kilo::offset
