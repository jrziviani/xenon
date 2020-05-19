#include "allocators.h"

#include <config.h>

extern uintptr_t _end;

namespace xenon
{
    uintptr_t __placement = reinterpret_cast<uintptr_t>(&_end);

    paddr_t placement_kalloc(size_t size, paddr_t *paddr, bool align/*=false*/)
    {
        if (align) {
            __placement = ALIGN_UP(__placement);
        }

        uintptr_t *ret = reinterpret_cast<uintptr_t*>(__placement);
        *paddr = ptr_to<paddr_t>(__placement - KVIRTUAL_ADDRESS);
        __placement += size;

        return ret;
    }

    paddr_t placement_kalloc(size_t size, bool align/*=false*/)
    {
        paddr_t tmp;
        return placement_kalloc(size, &tmp, align);
    }

    void kfree_block(size_t size)
    {
        /*
        // clean memory content
        for (size_t i = 0; i < size; i++) {
            *ptr_to<uint8_t*>(__placement) = 0;
        }
        */
        __placement -= size;
    }

    uintptr_t current()
    {
        return __placement;
    }
}
