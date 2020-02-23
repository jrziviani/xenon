#include "manager.h"
#include "config.h"
#include "libs/logger.h"

#define rmap_cast(a) reinterpret_cast<multiboot_memory_map_t*>(a)

namespace xenon
{
    manager::manager()
    {
    }

    void manager::init(multiboot_info_t *info)
    {
        auto mmap_addr = info->mmap_addr + KVIRTUAL_ADDRESS;

        if ((info->flags & MULTIBOOT_INFO_MEM_MAP) == 0) {
            // panic
            return;
        }

        auto mmap = rmap_cast(mmap_addr);
        while (mmap < rmap_cast(mmap_addr + info->mmap_length)) {

            // tell the physical allocator how much physical memory
            // we currently have available
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {

                // ignore lower memory for while
                if (mmap->addr != 0x0) {
                    logger::instance().log("Address: 0x%x - Length: %d",
                                           mmap->addr, mmap->len);

                    physical_[ZONE_GPA].set_addresses(mmap->addr,
                                                      mmap->len);
                }
            }

            mmap = rmap_cast(reinterpret_cast<uint64_t>(mmap) +
                             mmap->size + sizeof(mmap->size));
        }
    }
}
