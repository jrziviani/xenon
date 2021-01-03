#include "physical.h"
#include "allocators.h"

#include <config.h>
#include <klib/logger.h>

extern uint64_t _end;

physical::physical() :
    physical_end_(0),
    free_list_{nullptr}
{
}

void physical::setup(paddr_t start, size_t len)
{
    physical_end_  = ptr_to<paddr_t>(ptr_from(start) + len);
    uintptr_t addr = ALIGN_UP(ptr_from(start));
    len            = ALIGN_DOWN(len);

    // add the whole physical memory in the list of free
    // frames
    for (; addr < len; addr += FRAME_SIZE) {
        free(ptr_to<paddr_t>(addr));
    }
}

paddr_t physical::alloc()
{
    // our list contains the free page frames, allocate a frame means
    // to remove a node from the list.
    frame *reserve = free_list_;
    if (reserve == nullptr) {
        // TODO: no free page frame available, swap may be needed
        klib::logger::instance().log("PANIC: No free page frame available");
        return nullptr;
    }

    //   initial: [ 0x5000 ]->[ 0x6000 ]->[ 0x7000]->NULL
    //   after:   [ 0x5000 ]->[ 0x6000 ]->NULL
    //            return: 0x7000
    kfree_block(sizeof(frame));
    free_list_ = reserve->previous;
    return reserve->address;
}

// TODO: this is broken, this method must returns a list with all
// frames allocated.
paddr_t physical::alloc(size_t blocks)
{
    // sanity check: nor we can allocate more than existing memory
    // neither zero blocks
    if (blocks * FRAME_SIZE > ptr_from(physical_end_) || blocks == 0) {
        return nullptr;
    }

    paddr_t addr = alloc();
    if (addr == nullptr) {
        klib::logger::instance().log("PANIC: unvailable physical address");
        return nullptr;
    }

    while (--blocks) {
        if (alloc() == nullptr) {
            // TODO: in this point we have at least one block
            //       allocated, that must be freed.
            klib::logger::instance().log("PANIC: unvailable physical address");
            return nullptr;
        }
    }

    return addr;
}

void physical::free(paddr_t addr)
{
    // sanity check: there's no reason to have a unaligned address
    // here as well as we have the physical_end_ limit.
    if (!IS_ALIGNED(ptr_from(addr)) || addr >= physical_end_) {
        klib::logger::instance().log("PANIC: free 0x%x", ptr_from(addr));
        return;
    }

    // create a node containing the physical address as value and push
    // it into the list
    frame *new_head = reinterpret_cast<frame*>(placement_kalloc(sizeof(frame)));
    new_head->address = addr;
    new_head->previous = free_list_;
    free_list_ = new_head;
}
