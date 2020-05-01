#ifndef HEAP_H
#define HEAP_H

#include "physical.h"
#include "virtual.h"

#include <klib/stdint.h>

namespace xenon
{
    class heap
    {
        physical &physical_;
        virt &virtual_;

    public:
        heap(physical &p, virt &v);

        int sbrk(size_t size);
        int mmap(vaddr_t vaddr, paddr_t paddr, uint8_t flags);
        int mmap(vaddr_t vaddr, uint8_t flags);
    };
}

#endif // HEAP_H
