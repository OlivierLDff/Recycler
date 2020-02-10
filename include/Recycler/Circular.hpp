#ifndef __RECYCLER_CIRCULAR_HPP__
#define __RECYCLER_CIRCULAR_HPP__

// ─────────────────────────────────────────────────────────────
//                  INCLUDE
// ─────────────────────────────────────────────────────────────

// C++ Headers
#include <cstdint>
#include <memory>

// ─────────────────────────────────────────────────────────────
//                  DECLARATION
// ─────────────────────────────────────────────────────────────

namespace Recycler {

// ─────────────────────────────────────────────────────────────
//                  CLASS
// ─────────────────────────────────────────────────────────────

/**
 * @brief      The cache in the container behave in a circular way.
 * Each time an object is requested, a new object gets allocated or reused if available.
 * It's circular because once MAX object created, the object try to reallocate first object
 * If an object is still in use it just get removed from the cache and replaced by the newly allocated one.
 *
 * @tparam     T     Class of the object in the cache
 * @tparam     MAX   Size of the circular buffer
 */
template<class T, std::size_t MAX = 16>
class Circular
{
    // ──────── TYPE ────────────
protected:
    typedef std::shared_ptr<T> SharedObject;

    // ──────── CONSTRUCTOR ────────────
public:
    /**
     * @brief Allocate `_cache` to size MAX. It can be resized later with `resize()`
     */
    Circular() : _cache(std::make_unique<SharedObject[]>(MAX)) { }

    // ──────── ATTRIBUTES ────────────
protected:
    /** @brief Array that store shared object reference of size `_maxSize` */
    std::unique_ptr<SharedObject[]> _cache;
    /** @brief Index of the next element returned by makeOrAllocate inside `_cache` */
    std::size_t _idx = 0;
    /** @brief Number of element `_cache` */
    std::size_t _size = 0;
    /** @brief Size of `_cache` */
    std::size_t _maxSize = MAX;

    // ──────── API ────────────
public:
    /**
     * @brief      Find an available object in the cache.
     * If none is found then a new object is allocated.
     * This function tries to mimic how a circular buffer works.
     * - If first element is free then return the first.
     *   This limit the growth of the cache if SharedObject are released fast enough
     * - Look for the first available object from there
     *
     * @param[in]  args   The arguments of constructor/reset function
     *
     * @tparam     Types  Arguments of constructor/reset function
     *
     * @return     The shared object.
     * The object is always valid and ready to be deserialized in.
     */
    template<typename... Types>
    SharedObject make(Types... args)
    {
        if(_size)
        {
            // Try to recycle first object
            const auto& first = _cache[0];
            if (first && first.use_count() == 1)
            {
                _idx = 0;
                first->reset(std::forward<Types>(args)...);
                return first;
            }

            // Try to recycle next object
            if(_idx + 1 < _size)
            {
                const auto& next = _cache[_idx+1];
                if (next && next.use_count() == 1)
                {
                    ++_idx;
                    next->reset(std::forward<Types>(args)...);
                    return next;
                }
            }
        }

        // Create new object that will either make the cache grow if (_size != _maxSize) or replace
        // a previously allocated item in the cache
        const auto object = std::make_shared<T>(std::forward<Types>(args)...);

        // Cache is growing
        if (_size != _maxSize)
            ++_size;

        ++_idx;

        // Handle circular loop
        if (_idx >= _size)
            _idx = 0;

        // Create or override item at idx
        _cache[_idx] = object;

        return object;
    }
public:
    /**
     * @brief      Number of objects in cache
     * This function doesn't count objects that got allocated after MAX got reached
     *
     * @return     Number of objects in cache
     */
    std::size_t size() const { return _size; }

    /**
     * @brief      Max size of object in cache
     *
     * @return     Max size of object in cache
     */
    std::size_t maxSize() const { return _maxSize; }

    /**
     * @brief           Resize the maximum number of objects in the cache.
     * All object already in the cache will be released and cache reset
     *
     * @param maxSize   New max size for the cache
     *
     * @return          True if maxSize is >= 1, otherwise false.
     */
    bool resize(const std::size_t maxSize)
    {
        if (maxSize < 1)
            return false;

        _cache = std::make_unique<SharedObject[]>(maxSize);
        _idx = 0;
        _size = 0;
        return true;
    }

    /**
     * @brief      Release all item that have a reference on them
     */
    void release()
    {
        std::unique_ptr<SharedObject[]> cache = std::make_unique<SharedObject[]>(_maxSize);
        std::size_t size = 0;

        // Keep in cache all reusable items
        for(auto i = 0; i < _maxSize; ++i)
            if (_cache[i] && _cache[i].use_count() == 1)
                cache[size++] = _cache[i];

        // Clear our current cache
        clear();

        // Delete the previous cache to only keep the new one.
        // Since cache is already allocated, this will avoid a memcpy
        _cache = std::move(cache);
        _size = size;
    }

    /**
     * @brief      Remove all objects from the cache and release them
     */
    void clear()
    {
        for (std::size_t i = 0; i < _maxSize; ++i)
            _cache[i] = nullptr;
        _idx = 0;
        _size = 0;
    }
};

}

#endif
