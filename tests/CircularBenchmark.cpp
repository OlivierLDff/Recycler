// Application Headers
#include <Recycler/Circular.hpp>
#include <Recycler/Tests/Foo.hpp>

// C++ Headers
#include <chrono>
#include <iostream>

using namespace recycler;

template <size_t SIZE>
void benchmarkCircular()
{
    Circular<Foo<SIZE>, 256> cache;
    std::shared_ptr<Foo<SIZE>> dummy[256];

    // Warm up cache
    for (auto& i : dummy)
        i = cache.make();
    for (auto& i : dummy)
        i = nullptr;
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    for (int j = 0; j < 1000; ++j)
    {
        for (auto& i : dummy)
            i = cache.make();
        for (auto& i : dummy)
            i = nullptr;
    }
    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    const std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
    for (int j = 0; j < 1000; ++j)
    {
        for (auto& i : dummy)
            i = std::make_shared<Foo<SIZE>>();
        for (auto& i : dummy)
            i = nullptr;
    }
    const std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();

    const auto ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    const auto ms2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - begin2).count();

    std::cout << "CircularCache Perf<" << SIZE << ">   \t" << ms1 << " [ms]" << std::endl;
    std::cout << "make_shared Perf  <" << SIZE << ">   \t" << ms2 << " [ms]" << std::endl;
    std::cout << "Cache is " << (float(ms2) / float(ms1)) << " times faster" << std::endl;
}

int main(int argc, char **argv)
{
    benchmarkCircular<32>();
    benchmarkCircular<64>();
    benchmarkCircular<256>();
    benchmarkCircular<1024>();
    benchmarkCircular<8192>();
    benchmarkCircular<65536>();

    return 0;
}
