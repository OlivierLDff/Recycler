#include <Recycler/Buffer.hpp>
#include <gtest/gtest.h>
#include <string>
#include <cstring>

TEST(Buffer, resize)
{
    recycler::Buffer<std::uint8_t> buffer(2048);

    EXPECT_EQ(buffer.length(), 2048);
    EXPECT_EQ(buffer.maxSize(), 2048);

    buffer.reset(1024);
    EXPECT_EQ(buffer.length(), 1024);
    EXPECT_EQ(buffer.maxSize(), 2048);

    buffer.reset(4096);
    EXPECT_EQ(buffer.length(), 4096);
    EXPECT_EQ(buffer.maxSize(), 4096);

    buffer.reset(1024);
    EXPECT_EQ(buffer.length(), 1024);
    EXPECT_EQ(buffer.maxSize(), 4096);

    buffer.release();
    EXPECT_EQ(buffer.length(), 1024);
    EXPECT_EQ(buffer.maxSize(), 1024);
}

TEST(Buffer, operator)
{
    recycler::Buffer<std::uint8_t> buffer(2048);

    buffer[1] = 10;
    EXPECT_EQ(buffer[1], 10);
    EXPECT_EQ(&buffer[0], buffer.buffer());
}

TEST(Buffer, iterator)
{
    recycler::Buffer<std::uint8_t> buffer(2048);

    int c = 0;
    for(auto& i: buffer) i = ++c;

    ASSERT_EQ(buffer[0], 1);
    ASSERT_EQ(buffer[1], 2);
    ASSERT_EQ(buffer[2], 3);
    ASSERT_EQ(buffer[254], 255);
    ASSERT_EQ(buffer[255], 0);
}

TEST(Buffer, memset_cpy)
{
    recycler::Buffer<std::uint8_t> buffer(2048);

    std::memset(buffer, 45, 2048);
    for(const auto& i: buffer) ASSERT_EQ(i, 45);

    recycler::Buffer<std::uint8_t> dst(2048);

    std::memcpy(dst, buffer, 2048);
    for(const auto& i: dst) ASSERT_EQ(i, 45);
}

TEST(Buffer, initializer_list)
{
    recycler::Buffer<std::uint8_t> buffer = {1, 2, 3};
    ASSERT_EQ(buffer.length(), 3);

    for(std::size_t i = 0; i < buffer.length(); ++i)
        ASSERT_EQ(buffer[i], i + 1);
}

TEST(Buffer, initializer_list2)
{
    recycler::Buffer<std::uint64_t> buffer = {1, 2, 3};
    ASSERT_EQ(buffer.length(), 3);

    for(std::size_t i = 0; i < buffer.length(); ++i)
        ASSERT_EQ(buffer[i], i + 1);
}

TEST(Buffer, initializer_list3)
{
    recycler::Buffer<std::string> buffer = {"1", "2", "3"};
    ASSERT_EQ(buffer.length(), 3);

    for(std::size_t i = 0; i < buffer.length(); ++i)
        ASSERT_EQ(buffer[i], std::to_string(i + 1));
}
