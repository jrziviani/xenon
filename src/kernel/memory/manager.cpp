#include "manager.h"

#include <config.h>
#include <klib/logger.h>

namespace xenon
{
    void manager__::initialize(multiboot_info_t *info, paging *pg)
    {
        paging_ = pg;
        // for now, and maybe forever, this code relies on the information
        // given by the multiboot protocol, implemented by the bootloader.
        // professional operating systems prefer to gather HW information by
        // themselves.
        auto mmap_addr = info->mmap_addr + KVIRTUAL_ADDRESS;
        auto mmap = ptr_to<multiboot_memory_map_t*>(mmap_addr);
        while (mmap < ptr_to<multiboot_memory_map_t*>(mmap_addr + info->mmap_length)) {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {

                // ignore lower memory for while ...
                if (mmap->addr != 0x0) {
                    /*
                    logger::instance().log(" > Physical Address: 0x%x, Length: %d",
                                           mmap->addr,
                                           mmap->len);
                    */
                    physical_.setup(ptr_to<paddr_t>(mmap->addr), mmap->len);
                    break;
                }
            }
            else {
                const char *type;
                switch (mmap->type) {
                    case MULTIBOOT_MEMORY_RESERVED:
                        type = "reserved";
                        break;

                    case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                        type = "reclaimable";
                        break;

                    case MULTIBOOT_MEMORY_NVS:
                        type = "nvs";
                        break;

                    case MULTIBOOT_MEMORY_BADRAM:
                        type = "bad ram";
                        break;
                }
                /*
                logger::instance().log(" > %s: 0x%x, Length: %d",
                                        type, mmap->addr, mmap->len);
                                        */
            }

            mmap = ptr_to<multiboot_memory_map_t*>(ptr_from(mmap) +
                                                   mmap->size + sizeof(mmap->size));
        }

        heap_.setup(ptr_to<vaddr_t>(KVIRTUAL_ADDRESS + 64_MB), 16_MB);
        initialized_ = true;
    }

    bool manager__::is_initialized() const
    {
        return initialized_;
    }

    int manager__::mmap(vaddr_t addr, size_t size, uint8_t flags)
    {
        // reserve a contiguous virtual address space starting at addr
        // up to size. note that the function will fail if the address is
        // in use or if there's no contiguous addresses available.
        if (!virtual_.alloc(addr, size)) {
            logger::instance().log("PANIC: cannot allocate virtual address 0x%x", addr);
            return 1;
        }

        // reserve page frames available to be used. note that it cannot reserve
        // less then PAGE_FRAME size.
        size = size / FRAME_SIZE;
        paddr_t physical = physical_.alloc();
        if (physical == nullptr) {
            logger::instance().log("PANIC: cannot allocate %d addresses", size);
            return 1;
        }

        return paging_->map(addr, physical, flags);
    }

    void manager__::unmap(vaddr_t addr, size_t size)
    {
        (void)size;

        paging_->unmap(addr);
    }

    vaddr_t manager__::kalloc(size_t size)
    {
        return heap_.alloc(size);
    }

    void manager__::kfree(vaddr_t addr)
    {
        heap_.dealloc(addr);
    }

    paddr_t manager__::create_address_space()
    {
        return paging_->create_top_page_directory();
    }

    void manager__::destroy_address_space(paddr_t addr)
    {
        (void)addr;
    }
}
