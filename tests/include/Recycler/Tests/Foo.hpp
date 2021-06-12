#ifndef __RECYCLER_TESTS_FOO_HPP__
#define __RECYCLER_TESTS_FOO_HPP__

#include <cstring>
#include <cstdint>

namespace recycler {

template<size_t SIZE = 512>
class Foo
{
public:
    Foo() = default;
    void reset() {
        //std::memset(dummyData, 0, SIZE);
    };

    uint8_t dummyData[SIZE] = {};
};

typedef std::shared_ptr<Foo<>> SharedFoo;

}

#endif
