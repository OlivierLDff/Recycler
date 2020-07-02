# Recycler

[![](https://github.com/OlivierLDff/Recycler/workflows/CI/badge.svg)](https://github.com/OlivierLDff/Recycler/actions?query=workflow%3ACI)

This C++14 library provide classes that let you recycle allocated memory block. It's really convenient to use in a non blocking producer/consumer design pattern.

## How to use

### recycler::Circular

The `recycler::Circular<T>::make(...)` behave like `std::make_shared<T>(...)` except it will recycle previously allocated T if not in use anymore.

This library leverage the power of `std::shared_ptr` and heavily use the thread safe `use_count()` field. When an object created with `recycler::Circular` goes out from user scope, it's automatically reused.

The `Circular` container especially shine when:

* Items are expected to be released in the same order they got created.
* Items are expected to always be released.

If some item are kept by the user, then they will be removed from cache if cache grow more than it's size. It's also possible to explicitly release memory from the cache with `recycler::Circular<T>::release()`.

Max size of the cache can be set at object declaration, with templated arg `MAX`. Cache can be resized later to better suit needs with `recycler::Circular<T>::resize(size_t)`.

All memory in cache can be cleared with `recycler::Circular<T>::clear()`.

> `recycler::Circular<T, 2>` can be used for a double non blocking buffer.

#### Example

Here a basic example that allocate multiple instance of `Foo` and reuse them. A runnable example can be found in `CircularTests.cpp`

```cpp
#include <Recycler/Circular.hpp>

class Foo
{
public:
  Foo() = default;
  void reset() { };
};

int main()
{
  // 1) Declare the cache that contain std::shared_ptr<Foo>.
  // Max element in cache is 2
  recycler::Circular<Foo, 2> cache;

  // 2) Take a reference, first element is returned
  // This will call the constructor
  // cache.size()==1
  (void)cache.make();

  // 2) Take a reference. First element wasn't stored
  // outside the cache. First element is returned again
  // cache.size()==1
  const auto foo1 = cache.make();

  // 2) Take a reference. First element wasn't stored
  // outside the cache. First element is returned again
  // cache.size()==2
  auto foo2 = cache.make();

  // 3) Release foo2, this will be the next value returned
  foo2.reset();

  // 4) Foo2 take the same value as previously
  // cache.size()==2
  foo2 = cache.make();

  // ) foo1 is removed from the circular buffer because
  // it is referenced outside of the cache.
  // foo1 != foo4
  // cache.size()==2
  const auto foo4 = cache.make();

  // ) foo2 is removed from the circular buffer because
  // it is referenced outside of the cache
  // cache.size()==2
  const auto foo5 = cache.make();
}
```

The object contained can also use a custom constructor/reset function.

```cpp
#include <Recycler/Circular.hpp>

class Foo
{
public:
  Foo(int field1, double field2):
    _field1(field1),
    _field2(field2) {}

  void reset(int field1, double field2)
  {
      _field1 = field1;
      _field2 = field2;
  };

private:
  int _field1 = 0;
  double _field2 = 0.f;
};


int main()
{
  recycler::Circular<Foo<>, 2> cache;
  // Call constructor
  cache.make(4, 9.8);
  // Call reset function
  cache.make(5, 10.f);
}
```

### Buffer

The `recycler::Buffer` is fully ready to be used with `recycler::Circular<Buffer>`. It behave like a `std::unique_ptr<T[]>`.

To resize the buffer use `Buffer::resize(size_t)`. Memory will be reallocated only if the internal memory is smaller than the new size. Note than when resizing, data will be lost. No internal copy happened.

```cpp
#include <Recycler/Buffer.hpp>
int main()
{
  recycler::Buffer<std::uint8_t> buffer(1024);

  // Memory is reallocated. Data is lost.
  buffer.resize(4096);

  // No allocation happened. Data is lost.
  buffer.resize(2048);

  // Memory is reallocated to 2048. Data is lost.
  buffer.release();

  // Offset access
  buffer[0] = 10;

  // Range loop
  for(auto& it : buffer)
    it = 45;

  // Auto cast
  std::memset(buffer, 45, 2048);

  // Buffer can also be initilized from std::initializer_lsit
  buffer = {1, 2, 3};

  // Or reset
  buffer.reset({1, 2, 3, 4});
}
```


## Build

Simply clone then run cmake.

```
git clone https://github.com/OlivierLdff/Recycler.git
cd Recycler && mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Tests

Tests depends on google tests. They are build by default so you just need to run:

```
ctest -C Release
```

### CMake Parameters

- **RECYCLER_TARGET** : Library target name. *Default : "Recycler"*
- **RECYCLER_PROJECT** : Project name. *Default : "Recycler"*
- **RECYCLER_BUILD_TEST** : Build Recycler Test executable [ON OFF]. *Default: OFF*.

### CMake Integration

The CMake script give you a target Recycler that is an `INTERFACE`. Simply use CMake FetchContent function.

```cmake
include(FetchContent)
FetchContent_Declare(
    Recycler
    GIT_REPOSITORY https://github.com/OlivierLdff/Recycler.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(Recycler)
target_link_libraries(YourTarget Recycler)
```