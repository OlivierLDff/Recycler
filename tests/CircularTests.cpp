#include <Recycler/Circular.hpp>
#include <Recycler/Tests/Foo.hpp>

#include <gtest/gtest.h>

#include <random>

using namespace Recycler;

TEST(CircularCacheTests, basic)
{
    Circular<Foo<>, 5> cache;

    ASSERT_EQ(cache.size(), 0);
    (void)cache.make();
    ASSERT_EQ(cache.size(), 1);
    (void)cache.make();
    ASSERT_EQ(cache.size(), 1);

    auto c1 = cache.make();
    ASSERT_EQ(c1.use_count(), 2);
    ASSERT_EQ(cache.size(), 1);
    auto c2 = cache.make();
    const auto c2Ptr = c2.get();
    c2 = nullptr;
    ASSERT_EQ(cache.size(), 2);
    (void)cache.make();
    ASSERT_EQ(cache.size(), 3);
    const auto c1Ptr = c1.get();
    c1 = nullptr;
    c1 = cache.make();
    ASSERT_EQ(cache.size(), 3);
    ASSERT_EQ(c1.get(), c1Ptr);
    ASSERT_EQ(cache.make().get(), c2Ptr);

    cache.clear();

    ASSERT_EQ(cache.size(), 0);

    SharedFoo c[5];

    for (int i = 0; i < 24; ++i)
    {
        c[i % 5] = cache.make();
        ASSERT_EQ(c[i % 5].use_count(), 2);
    }
    ASSERT_EQ(cache.size(), 5);
}

TEST(CircularCacheTests, fuzz)
{
    Circular<Foo<>, 10> cache;
    std::random_device rd;

    SharedFoo c[3];
    for (int i = 0; i < 1000; ++i)
    {
        const auto rdmIdx = rd() % 2;
        c[rdmIdx] = cache.make();
        ASSERT_EQ(c[rdmIdx].use_count(), 2);
    }
}

TEST(CircularCacheTests, fuzz2)
{
    Circular<Foo<>, 10> cache;

    SharedFoo c[3];
    int ci = 0;
    for (int i = 0; i < 20; ++i)
    {
        c[ci++] = cache.make();
        if (ci == 3)
            ci = 0;

        // Cache should never grow bigger than 4
        ASSERT_TRUE(cache.size() < 5);
    }
}

TEST(CircularCacheTests, release)
{
    Circular<Foo<>, 10> cache;

    SharedFoo c[3];
    int ci = 0;
    for (int i = 0; i < 3; ++i)
        c[ci++] = cache.make();
    ASSERT_EQ(cache.size(), 3);
    cache.release();
    ASSERT_EQ(cache.size(), 0);
    ci = 0;
    for (int i = 0; i < 3; ++i)
        c[ci++] = cache.make();
    ASSERT_EQ(cache.size(), 3);
    cache.release();
    ASSERT_EQ(cache.size(), 0);
}

TEST(CircularCacheTests, example)
{
    // 1) Declare the cache
    Circular<Foo<>, 2> cache;

    // 2) Take a reference, first element is returned
    (void)cache.make();
    ASSERT_EQ(cache.size(), 1);

    // 2) Take a reference. First element wasn't stored
    // outside the cache. First element is returned again
    const auto foo1 = cache.make();
    ASSERT_EQ(cache.size(), 1);

    // 2) Take a reference. First element wasn't stored
    // outside the cache. First element is returned again
    auto foo2 = cache.make();
    ASSERT_EQ(cache.size(), 2);

    // 3) Release foo2, this will be the next value returned
    foo2.reset();

    // 4) Foo2 take the same value as previously
    foo2 = cache.make();
    ASSERT_EQ(cache.size(), 2);

    // ) foo1 is removed from the circular buffer because
    // it is referenced outside of the cache
    const auto foo4 = cache.make();
    ASSERT_EQ(cache.size(), 2);
    ASSERT_EQ(foo1.use_count(), 1);

    // ) foo2 is removed from the circular buffer because
    // it is referenced outside of the cache
    const auto foo5 = cache.make();
    ASSERT_EQ(cache.size(), 2);
    ASSERT_EQ(foo2.use_count(), 1);
}
