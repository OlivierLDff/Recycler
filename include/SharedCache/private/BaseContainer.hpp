#ifndef __SHARED_CACHE_BASE_CONTAINER_HPP__
#define __SHARED_CACHE_BASE_CONTAINER_HPP__

// ─────────────────────────────────────────────────────────────
//                  INCLUDE
// ─────────────────────────────────────────────────────────────

// C++ Header
#include <cstdint>
#include <memory>
#include <forward_list>

// Application Header
#include <SharedCache/private/Export.hpp>

// ─────────────────────────────────────────────────────────────
//                  DECLARATION
// ─────────────────────────────────────────────────────────────

SHAREDCACHE_NAMESPACE_START

// ─────────────────────────────────────────────────────────────
//                  CLASS
// ─────────────────────────────────────────────────────────────

template<class T, size_t MAX>
class BaseContainer
{
    // ──────── TYPE ────────────
public:
    typedef std::shared_ptr<T> SharedObject;
    typedef std::forward_list<SharedObject> ForwardList;
    typedef typename ForwardList::iterator Iterator;
    typedef typename ForwardList::const_iterator ConstIterator;

    // ──────── CONSTRUCTOR ────────────
public:
    BaseContainer() : _it(_cache.before_begin()) { }

    // ──────── ATTRIBUTES ────────────
protected:
    ForwardList _cache;
    Iterator _it;
    size_t _size = 0;
public:
    constexpr static size_t MAX_SIZE = MAX;

    // ──────── API ────────────
protected:
    template<typename... Types>
    SharedObject makeOrReset(Types... args)
    {
        ++_it;
        auto& object = *_it;
        if (object.use_count() == 1)
            object->reset(std::forward<Types>(args)...);
        else
            object = std::make_shared<T>(std::forward<Types>(args)...);
        return object;
    }

public:
    /**
     * @brief      Remove all objects from the cache
     */
    void clear()
    {
        _cache.clear();
        _it = _cache.before_begin();
        _size = 0;
    }

    /**
     * @brief      Number of objects in cache
     * This function doesn't count objects that got allocated after MAX got reached
     *
     * @return     Number of objects in cache
     */
    size_t size() const { return _size; }

    /**
     * @brief      Max size of object in cache
     *
     * @return     Max size of object in cache
     */
    static size_t max_size() { return MAX_SIZE; }
};

SHAREDCACHE_NAMESPACE_END

#endif
