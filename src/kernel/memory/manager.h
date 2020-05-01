#ifndef MANAGER_H
#define MANAGER_H

#include "physical.h"
#include "virtual.h"
#include "paging.h"

#include <klib/multiboot.h>

namespace xenon
{
    /*
     * memory manager
     *
     * Responsible to reserve page frames from "physical.h", reserve page
     * table entries from "virtual.h", and map. Not less important, it
     * also unmaps, release the virtual addresses and the physical frames.
     *
     * The heavy duty is under physical.h and virtual.h. The job here
     * is to orchestrate their work.
     */
    class manager
    {
        physical physical_;
        virt     virtual_;
        paging  *paging_;

    public:
        manager(multiboot_info_t *info, paging *pg);

        int mmap(vaddr_t addr, size_t size, uint8_t flags);
        void unmap(vaddr_t addr, size_t size);
    };
}

#endif // MANAGER_H
