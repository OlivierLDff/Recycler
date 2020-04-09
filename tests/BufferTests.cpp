#include <Recycler/Buffer.hpp>
#include <gtest/gtest.h>
#include <string>

TEST(Buffer, resize)
{
    Recycler::Buffer<std::uint8_t> buffer(2048);

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
    Recycler::Buffer<std::uint8_t> buffer(2048);

    buffer[1] = 10;
    EXPECT_EQ(buffer[1], 10);
    EXPECT_EQ(&buffer[0], buffer.buffer());
}

TEST(Buffer, iterator)
{
    Recycler::Buffer<std::uint8_t> buffer(2048);

    int c = 0;
    for(auto& i : buffer)
        i = ++c;

    ASSERT_EQ(buffer[0], 1);
    ASSERT_EQ(buffer[1], 2);
    ASSERT_EQ(buffer[2], 3);
    ASSERT_EQ(buffer[254], 255);
    ASSERT_EQ(buffer[255], 0);
}

TEST(Buffer, memset_cpy)
{
    Recycler::Buffer<std::uint8_t> buffer(2048);

    std::memset(buffer, 45, 2048);
    for (const auto& i : buffer)
        ASSERT_EQ(i, 45);

    Recycler::Buffer<std::uint8_t> dst(2048);

    std::memcpy(dst, buffer, 2048);
    for (const auto& i : dst)
        ASSERT_EQ(i, 45);
}

TEST(Buffer, initializer_list)
{
    Recycler::Buffer<std::uint8_t> buffer = { 1, 2, 3 };
    ASSERT_EQ(buffer.length(), 3);

    for (std::size_t i = 0; i < buffer.length(); ++i)
        ASSERT_EQ(buffer[i], i + 1);
}

TEST(Buffer, initializer_list2)
{
    Recycler::Buffer<std::uint64_t> buffer = { 1, 2, 3 };
    ASSERT_EQ(buffer.length(), 3);

    for (std::size_t i = 0; i < buffer.length(); ++i)
        ASSERT_EQ(buffer[i], i + 1);
}

TEST(Buffer, initializer_list3)
{
    Recycler::Buffer<std::string> buffer = { "1", "2", "3" };
    ASSERT_EQ(buffer.length(), 3);

    for (std::size_t i = 0; i < buffer.length(); ++i)
        ASSERT_EQ(buffer[i], std::to_string(i + 1));
}
