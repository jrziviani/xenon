#include "physical.h"
#include "allocators.h"

#include <config.h>
#include <klib/logger.h>

extern uint64_t _end;

namespace xenon
{
    free_list::free_list() :
        physical_end_(0),
        free_list_{nullptr}
    {
    }

    void free_list::setup(uint64_t start, uint64_t len)
    {
        physical_end_ = start + len;

        uint64_t new_start = 0x800000;
        start = (start > new_start) ? start : new_start;
        len = ALIGN_DOWN(len);

        for (uint64_t addr = ALIGN_UP(start); addr < len; addr += FRAME_SIZE) {
            free(addr);
        }
    }

    void free_list::free(uintptr_t physical_addr)
    {
        if (!IS_ALIGNED(physical_addr) ||
            physical_addr >= physical_end_) {
            logger::instance().log("PANIC: free 0x%x", physical_addr);
            return;
        }

        list *new_head = reinterpret_cast<list*>(placement_kalloc(sizeof(list)));
        new_head->address = physical_addr;
        new_head->previous = free_list_.tail;
        free_list_.tail = new_head;
    }

    uintptr_t free_list::alloc()
    {
        list *reserve = free_list_.tail;
        if (reserve) {
            kfree_block(sizeof(list));
            free_list_.tail = reserve->previous;
            return reserve->address;
        }

        return 0;
    }
}
