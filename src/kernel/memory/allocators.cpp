#include "allocators.h"

#include <config.h>

extern uintptr_t _end;

namespace xenon
{
    uintptr_t __placement = reinterpret_cast<uintptr_t>(&_end);

    uintptr_t *placement_kalloc(size_t size, uintptr_t *paddr)
    {
        uintptr_t *ret = reinterpret_cast<uintptr_t*>(__placement);
        *paddr = __placement - KVIRTUAL_ADDRESS;
        __placement += size;

        return ret;
    }

    uintptr_t *placement_kalloc(size_t size)
    {
        uintptr_t tmp;
        return placement_kalloc(size, &tmp);
    }

    void kfree_block(size_t size)
    {
        __placement -= size;
    }

    uintptr_t current()
    {
        return __placement;
    }
}
