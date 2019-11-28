#pragma once

#include "ObjectPool.h"
#include "Header.h"

#ifdef NDEBUG
#define _DBG_FILL_FREE_PATTERN
#else
#define _DBG_FILL_FREE_PATTERN(p, sz) memset(p, 0xfe, sz)
#endif

#define DECLARE_POOLED_ALLOCATION(cls)          \
private:                                        \
    static ObjectPool<cls, Header> __pool;      \
    static void __fin(void *allocated)          \
    {                                           \
        __pool.Free(allocated);                 \
    }                                           \
public:                                         \
    void* operator new(size_t count)            \
    {                                           \
       assert(sizeof(cls) == count);            \
       void* _ptr = __pool.Alloc();             \
                                                \
       Header& hdr(*(Header*)_ptr);             \
       hdr.count = 0x80000000;                  \
       hdr.func = nullptr;                      \
                                                \
       return &hdr + 1;                         \
    }                                           \
                                                \
    void operator delete(void *p)               \
    {                                           \
        _DBG_FILL_FREE_PATTERN(p, sizeof(cls)); \
        Header& hdr(ExtractFrom<void>(p));      \
        hdr.count &= 0x7fffffff;                \
        if (hdr.count == 0)                     \
            __pool.Free(&hdr);                  \
        else                                    \
            hdr.func = __fin;                   \
    }                                           \

#define IMPLEMENT_POOLED_ALLOCATION(cls)        \
    ObjectPool<cls, Header> cls::__pool;
