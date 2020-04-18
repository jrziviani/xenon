#ifndef PAGING_H
#define PAGING_H

#include <klib/stdint.h>

namespace xenon
{
    class paging
    {
    public:
        paging();

        int map(uintptr_t top_dir, void *vaddr, uintptr_t paddr, uint8_t flags);
        int map(void *vaddr, uintptr_t paddr, uint8_t flags);

        void unmap(uintptr_t top_dir, void *vaddr);
        void unmap(void *vaddr);
    };
}

#endif // PAGING_H
