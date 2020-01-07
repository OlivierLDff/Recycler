#ifndef __SHAREDCACHE_EXPORT_HPP__
#define __SHAREDCACHE_EXPORT_HPP__

// ─────────────────────────────────────────────────────────────
//                  DECLARATION
// ─────────────────────────────────────────────────────────────

#ifdef SHAREDCACHE_USE_NAMESPACE

    #ifndef SHAREDCACHE_NAMESPACE
        #define SHAREDCACHE_NAMESPACE SharedCache
    #endif // ifndef SHAREDCACHE_NAMESPACE

    #define SHAREDCACHE_NAMESPACE_START namespace SHAREDCACHE_NAMESPACE {
    #define SHAREDCACHE_NAMESPACE_END }
    #define SHAREDCACHE_USING_NAMESPACE using namespace SHAREDCACHE_NAMESPACE;

#else // SHAREDCACHE_USE_NAMESPACE

    #undef SHAREDCACHE_NAMESPACE
    #define SHAREDCACHE_NAMESPACE
    #define SHAREDCACHE_NAMESPACE_START
    #define SHAREDCACHE_NAMESPACE_END
    #define SHAREDCACHE_USING_NAMESPACE

#endif // SHAREDCACHE_USE_NAMESPACE

#endif // __SHAREDCACHE_EXPORT_HPP__
