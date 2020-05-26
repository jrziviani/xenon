#ifndef NEW_H
#define NEW_H

#include "stdint.h"
#include "move.h"

#include <memory/allocators.h>

inline void *operator new(size_t, void *p) noexcept
{
    return p;
}

inline void *operator new[](size_t, void *p) noexcept
{
    return p;
}

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete[](void *p);

void operator delete(void*, void*) noexcept;
void operator delete[](void*, void*) noexcept;
void operator delete(void*, unsigned long) noexcept;
void operator delete[](void*, unsigned long) noexcept;

namespace xenon
{
    struct placement_new
    {
        template <typename T, typename... Args>
        T *create(Args&&... args)
        {
            void *place = placement_kalloc(sizeof(T));
            return new (place) T(forward<Args>(args)...);
        }

        template <typename T>
        void destroy(T *tp)
        {
            if (tp != nullptr) {
                tp->~T();
            }

            kfree_block(sizeof(T));
        }
    };

    struct basic_allocator
    {
        template <typename T, typename... Args>
        T *create(Args&&... args)
        {
            return new T(forward<Args>(args)...);
        }

        template <typename T>
        void destroy(T *tp)
        {
            delete tp;
        }
    };
}

#endif // NEW_H
