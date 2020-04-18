#ifndef NEW_H
#define NEW_H

#include "stdint.h"

inline void *operator new(size_t, void *p) noexcept
{
    return p;
}

inline void *operator new[](size_t, void *p) noexcept
{
    return p;
}

inline void operator delete(void*, void*) noexcept
{
}

inline void operator delete[](void*, void*) noexcept
{
}

inline void operator delete(void*, unsigned long) noexcept
{
}

/*
void operator delete(void*) noexcept
{
}
*/
#endif // NEW_H
