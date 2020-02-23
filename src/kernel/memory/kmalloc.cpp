#include "kmalloc.h"
#include "config.h"

extern uint64_t pdpt;

#define FRAME_SIZE 4096
#define align_down(address) ((address) & (-FRAME_SIZE))
#define align_up(address)   align_down(address + FRAME_SIZE - 1)
#define is_aligned(address) ((address & (FRAME_SIZE - 1)) == 0)

namespace xenon
{
    uint64_t placement_ = reinterpret_cast<uint64_t>(&pdpt);

    uintptr_t kmalloc(size_t size, uintptr_t &physical) noexcept
    {
        if (!is_aligned(placement_)) {
            align_up(placement_);
        }

        physical = placement_ - KVIRTUAL_ADDRESS;

        uintptr_t address = placement_;
        placement_ += size;

        return address;
    }

    uintptr_t kmalloc(size_t size) noexcept
    {
        uintptr_t physical;
        return kmalloc(size, physical);
    }

    void kfree(uintptr_t addr) noexcept
    {
        (void)addr;
    }
}
