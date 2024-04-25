#include "AppendBuffer.hpp"

#include <cstring>
#include <gtest/gtest.h>

using namespace Kilo::AppendBuffer;

TEST(AppendBufferTest, IsEmptyWhenCreated)
{
    AppendBuffer buffer;

    ASSERT_EQ(buffer.length(), 0);
}

TEST(AppendBufferTest, ItsSizeIncreasesByTheLengthOfTheAppendedString)
{
    AppendBuffer buffer;
    char const* str = "Hello, World!";
    buffer.append(str, std::strlen(str));

    ASSERT_EQ(buffer.length(), 13);
}

TEST(AppendBufferTest, c_strReturnsACStringRepresentationOfTheContentsOfTheBuffer)
{
    AppendBuffer buffer;
    char const* str = "The quick brown fox jumped over the lazy dog";

    buffer.append(str, std::strlen(str));
    
    ASSERT_STREQ(buffer.c_str(), str);
}