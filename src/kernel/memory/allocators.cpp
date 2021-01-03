#include "allocators.h"
#include "manager.h"

#include <config.h>

extern uintptr_t _end;

uintptr_t __placement = reinterpret_cast<uintptr_t>(&_end);

vaddr_t placement_kalloc(size_t size, paddr_t *paddr, bool align/*=false*/)
{
    if (align) {
        __placement = ALIGN_UP(__placement);
    }

    uintptr_t *ret = reinterpret_cast<uintptr_t*>(__placement);
    *paddr = ptr_to<paddr_t>(__placement - KVIRTUAL_ADDRESS);
    __placement += size;

    return ret;
}

vaddr_t placement_kalloc(size_t size, bool align/*=false*/)
{
    paddr_t tmp;
    return placement_kalloc(size, &tmp, align);
}

vaddr_t kalloc(size_t size)
{
    if (manager::instance().is_initialized()) {
        return manager::instance().kalloc(size);
    }

    return placement_kalloc(size);
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

void kfree(vaddr_t addr)
{
    if (manager::instance().is_initialized()) {
        return manager::instance().kfree(addr);
    }
}

uintptr_t current()
{
    return __placement;
}
