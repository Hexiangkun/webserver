#include "buffer.h"
#include <gtest/gtest.h>


using namespace hxk;

TEST(buffer, push) {
    Buffer buf;

    size_t ret = buf.pushData("hello", 5);
    EXPECT_EQ(ret, 5);

    ret = buf.pushData("world\n", 6);
    EXPECT_EQ(ret, 6);
}


TEST(buffer, peek) {
    Buffer buf;

    {
        buf.pushData("hello ", 6);
        buf.pushData("world\n", 6);
    }

    char tmp[12];
    size_t ret = buf.peekDataAt(tmp, 5, 0);
    EXPECT_EQ(ret, 5);
    EXPECT_EQ(tmp[0], 'h');
    EXPECT_EQ(tmp[4], 'o');

    ret = buf.peekDataAt(tmp, 2, 6);
    EXPECT_EQ(ret, 2);
    EXPECT_EQ(tmp[0], 'w');
    EXPECT_EQ(tmp[1], 'o');

    EXPECT_EQ(buf.readableSize(), 12);
}

TEST(buffer, pop) {
    Buffer buf;

    {
        buf.pushData("hello ", 6);
        buf.pushData("world\n", 6);
    }

    size_t cap = buf.capacity();

    std::cout << cap << std::endl;

    char tmp[12];
    size_t ret = buf.popData(tmp, 6);
    EXPECT_EQ(ret, 6);
    EXPECT_EQ(tmp[0], 'h');
    EXPECT_EQ(tmp[5], ' ');

    EXPECT_EQ(buf.readableSize(), 6);

    ret = buf.popData(tmp, 6);
    EXPECT_EQ(ret, 6);
    EXPECT_EQ(tmp[0], 'w');
    EXPECT_EQ(tmp[5], '\n');

    EXPECT_TRUE(buf.isEmpty());
    EXPECT_EQ(buf.capacity(), cap); // pop does not change capacity
}

size_t test(std::size_t size)
{
    if (size == 0)
        return 0;

    std::size_t roundUp = 1;
    while (roundUp < size)
        roundUp *= 2;

    return roundUp;
}

TEST(buffer, shrink) {
    Buffer buf;

    {
        buf.pushData("hello ", 6);
        buf.pushData("world\n", 6);
    }

    EXPECT_NE(buf.capacity(), 12);

    buf.shrink();
    EXPECT_EQ(buf.capacity(), 16);

    buf.pushData("abcd", 4);
    EXPECT_EQ(buf.capacity(), 16);

    char tmp[16];
    buf.popData(tmp, sizeof tmp);

    EXPECT_EQ(buf.capacity(), 16);
}

TEST(buffer, push_pop) {
    Buffer buf;

    buf.pushData("hello ", 6);

    char tmp[8];
    size_t ret = buf.popData(tmp, 5);

    EXPECT_EQ(ret, 5);
    EXPECT_EQ(buf.capacity(), Buffer::kDefaultSize);

    buf.shrink();
    EXPECT_EQ(buf.capacity(), 1);
}


int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}