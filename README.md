# SharedCache

The library provide 2 containers that contains a list of `shared_ptr`. The goals of those container is to reduce dynamic allocation call by reusing already allocated objects that are no longer in use.

* **ListCache**: Allocate objects in a list. When the list if full the objects continued to get allocated but are not stored in the list.
  * Calling `reset` reset the list counter without deleting any shared object.
  * Calling `release` will remove from the list all object reference elsewhere.
  * When iterating in the list to recreate object, the ones that are shared are removed and new objects are allocated.
* **CircularCache**: Allocate object until the list is full. Then reuse the first objects in the list if they are not used somewhere else. Otherwise reallocate.

Using one type of list or the other really depends on your usage. You should benchmark your use case. Efficiency of List  vs Circular depends on how you keep reference on created objects.

* For an object that is a container of other object choose ListCache.
* For a messaging system that send object use CircularCache.

## Examples

### ListCache

In the following example the whole API is demonstrated.

```cpp
#include <SharedCache/ListCache.hpp>

class Foo
{
public:
  Foo() = default;
  void reset() { };
};

int main()
{
  // 1) Declare the cache
  ListCache<Foo, 4> cache;

  // 2) Create one Foo
  cache.make();

  // 3) Reuse the first foo created. cache.size() == 1
  const auto foo = cache.make();

  // 4) Create a second foo because
  auto foo2 = cache.make();

  // 5) Release foo2
  foo2.reset();

  // 6) The cache will reuse foo2 (cache.size() == 2)
  foo2 = cache.make();

  // 7) foo & foo2 are reference here and in the cache
  // release function will remove foo & foo2 from the cache
  // cache.size() == 0
  cache.release();

  // 8.1) Reinsert elements in the cache and release foo4
  const auto foo3 = cache.make();
  auto foo4 = cache.make();
  const auto foo5 = cache.make();
  foo4.reset();

  // 8.2) Reset the cache mean only
  // reset the internal iterator in the list
  // So size stay (cache.size() == 3)
  cache.reset();

  // 9.1) This function will allocate a new foo at the index of foo3
  // because foo3 is still referenced outside cache
  const auto foo6 = cache.make();

  // 9.2) Because foo4 have been release, the old value of foo4 will be reused
  const auto foo7 = cache.make();

  // 9.3) foo5 still reference outside cache, so foo8 is newly allocated
  // (cache.size() == 3)
  const auto foo8 = cache.make();

  // 9.4) A new value is created inside the cache (cache.size() == 4)
  const auto foo9 = cache.make();

  // 9.5) A new value is created but not stored inside the cache
  // because MAX_SIZE is reached (cache.size() == 4)
  const auto foo10 = cache.make();
}
```

### CircularCache

CircularCache is simpler to use because there is only one function exposed: `make`.

```cpp
#include <SharedCache/CircularCache.hpp>

class Foo
{
public:
  Foo() = default;
  void reset() { };
};

int main()
{
  // 1) Declare the cache
  CircularCache<Foo, 2> cache;

  // 2) Take a reference, first element is returned
  cache.make();

  // 2) Take a reference. First element wasn't stored
  // outside the cache. First element is returned again
  const auto foo1 = cache.make();

  // 2) Take a reference. First element wasn't stored
  // outside the cache. First element is returned again
  auto foo2 = cache.make();

  // 3) Release foo2, this will be the next value returned
  foo2.reset();

  // 4) Foo2 take the same value as previously
  foo2 = cache.make();

  // ) foo1 is removed from the circular buffer because
  // it is referenced outside of the cache
  const auto foo4 = cache.make();

  // ) foo2 is removed from the circular buffer because
  // it is referenced outside of the cache
  const auto foo5 = cache.make();
}
```

### Custom Constructor

The object contained can also use a custom constructor/reset function.

```cpp
#include <SharedCache/CircularCache.hpp>

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
  CircularCache<Foo<>, 2> cache;
  // Call constructor
  cache.make(4, 9.8);
  // Call reset function
  cache.make(5, 10.f);
}
```

## Build

Simply clone then run cmake.

```
git clone https://github.com/OlivierLdff/SharedCache.git
cd SharedCache && mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Tests

Tests depends on google tests. They are build by default so you just need to run:

```
ctest -C Release
```

### CMake Parameters

- **SHAREDCACHE_TARGET** : Library target name. *Default : "SharedCache"*
- **SHAREDCACHE_PROJECT** : Project name. *Default : "SharedCache"*
- **SHAREDCACHE_USE_NAMESPACE** : Should the library be compiled with a namespace. *Default: ON*.
- **SHAREDCACHE_NAMESPACE** : Namespace of the library is SHAREDCACHE_USE_NAMESPACE is ON. *Default : SharedCache.*
- **SHAREDCACHE_BUILD_TEST** : Build SharedCache Test executable [ON OFF]. *Default: OFF*.

### CMake Integration

The CMake script give you a target SharedCache that is an `INTERFACE`. Simply use CMake FetchContent function.

```cmake
include(FetchContent)
FetchContent_Declare(
    SharedCache
    GIT_REPOSITORY https://github.com/OlivierLdff/SharedCache.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(SharedCache)
target_link_libraries(YourTarget SharedCache)
```