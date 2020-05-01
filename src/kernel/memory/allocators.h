#ifndef ALLOCATORS_H
#define ALLOCATORS_H

#include <klib/stdint.h>

namespace xenon
{
    uintptr_t *placement_kalloc(size_t size, uintptr_t *paddr, bool align);
    uintptr_t *placement_kalloc(size_t size, uintptr_t *paddr);
    uintptr_t *placement_kalloc(size_t size);
    void kfree_block(size_t size);

    uintptr_t current();
}

#endif // ALLOCATORS_H
