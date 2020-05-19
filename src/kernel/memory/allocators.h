#ifndef ALLOCATORS_H
#define ALLOCATORS_H

#include <klib/stdint.h>

namespace xenon
{
    vaddr_t placement_kalloc(size_t size, paddr_t *paddr, bool align=false);
    vaddr_t placement_kalloc(size_t size, bool align=false);
    void kfree_block(size_t size);

    uintptr_t current();
}

#endif // ALLOCATORS_H
