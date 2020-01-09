#include <gtest/gtest.h>

GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <SharedCache.hpp>

SHAREDCACHE_USING_NAMESPACE;

template<size_t SIZE = 512>
class Foo
{
public:
    Foo() = default;
    void reset()
    {
        std::memset(dummyData, 0, SIZE);
    };

    uint8_t dummyData[SIZE] = {};
};

typedef std::shared_ptr<Foo<>> SharedFoo;

TEST(CircularCacheTests, basic)
{
    CircularCache<Foo<>, 5> cache;

    ASSERT_EQ(cache.size(), 0);
    (void)cache.make();
    ASSERT_EQ(cache.size(), 1);
    (void)cache.make();
    ASSERT_EQ(cache.size(), 1);

    auto c1 = cache.make();
    ASSERT_EQ(c1.use_count(), 2);
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

TEST(CircularCacheTests, resetCall)
{
    CircularCache<Foo<1>, 5> cache;
    auto foo = cache.make();
    foo->dummyData[0] = 6;
    foo = nullptr;

    foo = cache.make();
    ASSERT_EQ(foo->dummyData[0], 0);
}

TEST(CircularCacheTests, example)
{
    // 1) Declare the cache
    CircularCache<Foo<>, 2> cache;

    // 2) Take a reference, first element is returned
    cache.make();
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

TEST(ListCacheTests, basic)
{
    ListCache<Foo<>, 256> cache;
    constexpr size_t MAX_SIZE = ListCache<Foo<>, 256>::MAX_SIZE;

    std::shared_ptr<Foo<>> c;

    for (uint16_t i = 0; i < 10; ++i)
    {
        c = cache.make();
        EXPECT_EQ(c.use_count(), 2);
    }
    c = nullptr;

    EXPECT_EQ(cache.size(), 10);

    cache.release();

    for (size_t i = 0; i < 10; ++i)
    {
        c = cache.make();
        EXPECT_EQ(c.use_count(), 2);
    }

    EXPECT_EQ(cache.size(), 10);

    const auto c1 = cache.make();
    const auto c2 = cache.make();
    const auto c3 = cache.make();
    c = nullptr;

    EXPECT_EQ(cache.size(), 13);

    cache.release();
    EXPECT_EQ(cache.size(), 10);

    cache.clear();
    EXPECT_EQ(cache.size(), 0);

    // Use count is two because referenced in cache and in c
    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        c = cache.make();
        EXPECT_EQ(c.use_count(), 2);
    }
    EXPECT_EQ(cache.size(), MAX_SIZE);

    // Foo created are not stored anymore in the cache
    for (size_t i = 0; i < 10; ++i)
    {
        const auto foo = cache.make();
        EXPECT_EQ(foo.use_count(), 1);
    }
    // Every object are reference only in cache.
    // no object are going to be removed from the cache
    c = nullptr;
    EXPECT_EQ(cache.size(), MAX_SIZE);
    cache.release();
    EXPECT_EQ(cache.size(), MAX_SIZE);

    // Test repeatability
    for (size_t i = 0; i < MAX_SIZE; ++i)
    {
        c = cache.make();
        EXPECT_EQ(c.use_count(), 2);
    }
    EXPECT_EQ(cache.size(), MAX_SIZE);

    for (size_t i = 0; i < 10; ++i)
    {
        const auto foo = cache.make();
        EXPECT_EQ(foo.use_count(), 1);
    }
    c = nullptr;
    EXPECT_EQ(cache.size(), MAX_SIZE);

    cache.clear();
}

TEST(ListCacheTests, example)
{
    // 1) Declare the cache
    ListCache<Foo<>, 4> cache;

    // 2) Create one Foo
    cache.make();

    // 3) Reuse the first foo created. cache.size() == 1
    const auto foo = cache.make();
    EXPECT_EQ(cache.size(), 1);

    // 4) Create a second foo because
    auto foo2 = cache.make();
    EXPECT_EQ(cache.size(), 2);

    // 5) Release foo2
    foo2.reset();

    // 6) The cache will reuse foo2 (cache.size() == 2)
    foo2 = cache.make();
    EXPECT_EQ(cache.size(), 2);

    // 7) foo & foo2 are reference here and in the cache
    // release function will remove foo & foo2 from the cache
    // cache.size() == 0
    cache.release();
    EXPECT_EQ(cache.size(), 0);

    // 8.1) Reinsert elements in the cache and release foo4
    const auto foo3 = cache.make();
    auto foo4 = cache.make();
    const auto foo5 = cache.make();
    foo4.reset();

    // 8.2) Reset the cache mean only reset the internal iterator in the list
    // So size stay (cache.size() == 3)
    cache.reset();
    ASSERT_EQ(cache.size(), 3);

    // 9.1) This function will allocate a new foo at the index of foo3
    // because foo3 is still referenced outside cache
    const auto foo6 = cache.make();
    ASSERT_EQ(cache.size(), 3);

    // 9.2) Because foo4 have been release, the old value of foo4 will be reused
    const auto foo7 = cache.make();
    ASSERT_EQ(cache.size(), 3);

    // 9.3) foo5 still reference outside cache, so foo8 is newly allocated
    // (cache.size() == 3)
    const auto foo8 = cache.make();
    ASSERT_EQ(cache.size(), 3);

    // 9.4) A new value is created inside the cache (cache.size() == 4)
    const auto foo9 = cache.make();
    ASSERT_EQ(cache.size(), 4);

    // 9.5) A new value is created but not stored inside the cache
    // because MAX_SIZE is reached (cache.size() == 4)
    const auto foo10 = cache.make();
    ASSERT_EQ(cache.size(), 4);
}
