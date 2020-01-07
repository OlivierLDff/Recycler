#ifndef __SHARED_CACHE_CIRCULAR_HPP__
#define __SHARED_CACHE_CIRCULAR_HPP__

// ─────────────────────────────────────────────────────────────
//                  INCLUDE
// ─────────────────────────────────────────────────────────────

// C Header

// C++ Header

// Qt Header

// Dependencies Header

// Application Header
#include <SharedCache/private/BaseContainer.hpp>

// ─────────────────────────────────────────────────────────────
//                  DECLARATION
// ─────────────────────────────────────────────────────────────

SHAREDCACHE_NAMESPACE_START

// ─────────────────────────────────────────────────────────────
//                  CLASS
// ─────────────────────────────────────────────────────────────

/**
 * @brief      The cache in the container behave in a circular way.
 * It doesn't need to be cleared/reset.
 * Each time an object is requested, a new object gets allocated or reused if available.
 * It's circular because once MAX object created, the shared object returned are going the previous one in the list
 * If an object is still in use it just get removed from the cache.
 *
 * @tparam     T     Class of the object in the cache
 * @tparam     MAX   Size of the circular buffer
 */
template<class T, size_t MAX>
class CircularCache : public BaseContainer<T, MAX>
{
    typedef BaseContainer<T, MAX> Base;

    // ──────── API ────────────
public:
    /**
     * @brief      Find an available object in the cache. If none is found then
     * a new object is allocated
     *
     * @param[in]  args   The arguments of constructor/reset function
     *
     * @tparam     Types  Arguments of constructor/reset function
     *
     * @return     The shared object.
     * The object is always valid and ready to be deserialized in.
     */
    template<typename... Types>
    typename Base::SharedObject make(Types... args)
    {
        // ) Try to return first element if it's available
        if (Base::_size)
        {
            const auto beginIt = Base::_cache.begin();
            const auto& object = *beginIt;
            if (object.use_count() == 1)
            {
                Base::_it = Base::_cache.begin();
                return object;
            }
        }

        // ) Maybe Base::_it is at the end, so either loop or create element until MAX is reached
        auto it = Base::_it;
        ++it;
        if (it == Base::_cache.end())
        {
            const auto object = std::make_shared<T>(std::forward<Types>(args)...);
            // ) Cache can still grow. New object is stored at the end
            if (Base::_size < MAX)
            {
                Base::_cache.insert_after(Base::_it, object);
                ++Base::_it;
                ++Base::_size;
            }
            else
            {
                // ) Cache reached max size, time to loop
                Base::_it = Base::_cache.before_begin();
                *Base::_cache.begin() = object;
            }
            return object;
        }

        // ) Take the next object. Reset it if available or create a new one if not
        return Base::makeOrReset(std::forward<Types>(args)...);
    }
};

SHAREDCACHE_NAMESPACE_END

#endif
