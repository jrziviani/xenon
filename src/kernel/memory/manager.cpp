#include "manager.h"

#include <config.h>
#include <klib/logger.h>

inline multiboot_memory_map_t *to_rmap(uintptr_t a)
{
    return reinterpret_cast<multiboot_memory_map_t*>(a);
}

namespace xenon
{
    manager::manager(multiboot_info_t *info)
    {
        return;
        auto mmap_addr = info->mmap_addr + KVIRTUAL_ADDRESS;
        auto mmap = to_rmap(mmap_addr);
        while (mmap < to_rmap(mmap_addr + info->mmap_length)) {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {

                // ignore lower memory for while ...
                if (mmap->addr != 0x0) {
                    logger::instance().log("Physical Address: 0x%x, Length: %d",
                                           mmap->addr,
                                           mmap->len);

                    physical_.setup(mmap->addr, mmap->len);
                    break;
                }
            }

            mmap = to_rmap(reinterpret_cast<uint64_t>(mmap) +
                           mmap->size + sizeof(mmap->size));
        }
        /*
        auto phy = physical_.alloc();
        pages_.map(0x0, phy, 0);
        *((uintptr_t*)0x0) = 83;
        logger::instance().log("0x0: 0x%x", *reinterpret_cast<uintptr_t*>(0x0));
        */
    }
}
