#include "heap.h"
#include "paging.h"

namespace xenon
{
    heap::heap(physical &p, virt &v) :
        physical_(p),
        virtual_(v)
    {
    }

    int heap::mmap(vaddr_t vaddr, paddr_t paddr, uint8_t flags)
    {
    }

    int heap::mmap(vaddr_t vaddr, uint8_t flags)
    {

    }

    int heap::sbrk(size_t size)
    {

    }
}
