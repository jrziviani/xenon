#include "virtual.h"
#include "allocators.h"

const vaddr_t VADDR_START   = reinterpret_cast<vaddr_t>(0);
const size_t VADDR_SIZE     = 256_GB;

virt::virt()
{
    vaddr_t place = placement_kalloc(sizeof(node));
    node *first = new (place) node(VADDR_START, VADDR_SIZE);
    allocated_list_.push_back(first);
}

virt::virt(const virt &other)
{
    allocated_list_.clear();
    for (auto slot : other.allocated_list_) {
        allocated_list_.push_back(slot);
    }
}

virt &virt::operator=(const virt &other)
{
    if (this != &other) {
        allocated_list_.clear();
        for (auto slot : other.allocated_list_) {
            allocated_list_.push_back(slot);
        }
    }

    return *this;
}

vaddr_t virt::alloc(size_t size)
{
    for (auto free_spot : allocated_list_) {
        // free slot doesn't fit the requirement, skip it
        if (free_spot->size < size) {
            continue;
        }

        // shrink the slot size and return the address after (start + new_size)
        uintptr_t offset = free_spot->size - size;
        vaddr_t ret = reinterpret_cast<vaddr_t>(free_spot->addr_to_int() + offset);
        free_spot->size = offset;
        return ret;
    }

    return nullptr;
}

/*
    [free_spot                               ]--> null
    [free_spot      ]+------------>[new slot ]--> null
                     |
                     +---> initial_addr + size
*/
bool virt::alloc(vaddr_t initial_addr, size_t size)
{
    uintptr_t addr = reinterpret_cast<uintptr_t>(initial_addr);
    for (auto free_spot : allocated_list_) {
        // search for the slot which contains the address we're
        // looking for
        uintptr_t free_addr = free_spot->addr_to_int();
        if (free_addr < addr && free_addr + free_spot->size <= addr + size) {
            continue;
        }

        // create a new slot after the initial_addr + size
        vaddr_t place = placement_kalloc(sizeof(node));
        node *slot = new (place) node(reinterpret_cast<vaddr_t>(addr + size),
                                      free_spot->size);
        allocated_list_.push_back(slot);

        // resize the current slot
        free_spot->size = addr - free_addr;

        return true;
    }

    return false;
}

void virt::free(vaddr_t addr, size_t size)
{
    vaddr_t place = placement_kalloc(sizeof(node));
    node *next = new (place) node(addr, size);
    allocated_list_.push_front(next);
}
