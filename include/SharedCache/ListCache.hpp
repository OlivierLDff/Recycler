#ifndef __SHARED_CACHE_LIST_HPP__
#define __SHARED_CACHE_LIST_HPP__

// ─────────────────────────────────────────────────────────────
//                  INCLUDE
// ─────────────────────────────────────────────────────────────

// C Header

// C++ Header

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
 * @brief      This class a list of shared object T.
 * The goals is to reuse already allocated objects to avoid reallocating an object when some were already allocated before and are not used anymore
 *
 * @tparam     T     Class allocated by the cache.
 * T must implement a function reset(args...) and a constructor T(args...) when calling make(args...)
 * @tparam     MAX   Maximum number of object in the cache.
 * One this number is reached, object can still be allocated, but wont be keep in cache.
 */
    template<class T, size_t MAX>
class ListCache : public BaseContainer<T, MAX>
{
    typedef BaseContainer<T, MAX> Base;

    // ──────── API ────────────
public:
    /**
     * @brief      Find an available object in the cache.
     * If none is available then a new object will be created.
     * If there is still place in the cache, the object will be appended in the cache
     *
     * @param[in]  args   The arguments of constructor/reset function
     *
     * @tparam     Types  Arguments of constructor/reset function
     *
     * @return     The shared object.
     * The object is always valid. The function will never return a nullptr even if the cache is full.
     * When cache is full the object is just not kept in memory.
     * @note       Don't forget to call release() to remove from the cache objects that are not available anymore
     */
    template<typename... Types>
    typename Base::SharedObject make(Types... args)
    {
        // ) Maybe current object is available and can be reused
        if(Base::_it != Base::_cache.before_begin())
        {
            const auto& object = *Base::_it;
            if(object.use_count() == 1)
            {
                object->reset(std::forward<Types>(args)...);
                return object;
            }
        }

        // ) If the it is at the end of the list, it mean a new object must be created
        auto it = Base::_it;
        ++it;
        if (it == Base::_cache.end())
        {
            const auto object = std::make_shared<T>(std::forward<Types>(args)...);

            // ) Object is kept in the cache only if there is some place
            if (Base::_size < MAX)
            {
                Base::_cache.insert_after(Base::_it, object);
                ++Base::_it;
                ++Base::_size;
            }
            return object;
        }

        return Base::makeOrReset(std::forward<Types>(args)...);
    }

    /**
     * @brief      This function remove object that are reference elsewhere from the list
     */
    void release()
    {
        auto beforeIt = Base::_cache.before_begin();
        auto it = Base::_cache.begin();
        while (it != Base::_cache.end())
        {
            if (it->use_count() > 1)
            {
                it = Base::_cache.erase_after(beforeIt);
                --Base::_size;
            }
            else
            {
                ++it;
                ++beforeIt;
            }
        }
        reset();
    }

    void reset()
    {
        Base::_it = Base::_cache.before_begin();
    }
};

SHAREDCACHE_NAMESPACE_END

#endif
