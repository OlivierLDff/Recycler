#include <chrono>
#include <iostream>

#include <SharedCache/ListCache.hpp>

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

template <size_t SIZE>
void benchmarkList()
{
    ListCache<Foo<SIZE>, 256> cache;
    const size_t MAX_SIZE = cache.MAX_SIZE;
    std::shared_ptr<Foo<SIZE>> dummy[MAX_SIZE];

    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (int j = 0; j < 1000; ++j)
    {
        for (size_t i = 0; i < MAX_SIZE; ++i)
            dummy[i] = cache.make();
        for (size_t i = 0; i < MAX_SIZE; ++i)
            dummy[i] = nullptr;
        cache.release();
    }
    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    const std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
    for (int j = 0; j < 1000; ++j)
    {
        for (size_t i = 0; i < MAX_SIZE; ++i)
            dummy[i] = std::make_shared<Foo<SIZE>>();
        for (size_t i = 0; i < MAX_SIZE; ++i)
            dummy[i] = nullptr;
    }
    const std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();

    const auto ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    const auto ms2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - begin2).count();

    std::cout << "ListCache Perf    <" << SIZE << ">   \t" << ms1 << " [ms]" << std::endl;
    std::cout << "make_shared Perf  <" << SIZE << ">   \t" << ms2 << " [ms]" << std::endl;
    std::cout << "Cache is " << (float(ms2) / float(ms1)) << " times faster" << std::endl;
}

int main(int argc, char **argv)
{
    benchmarkList<32>();
    benchmarkList<64>();
    benchmarkList<256>();
    benchmarkList<1024>();
    benchmarkList<8192>();
    benchmarkList<65536>();

    return 0;
}
