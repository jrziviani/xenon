#ifndef MANAGER_H
#define MANAGER_H

#include "physical.h"
#include "virtual.h"
#include "paging.h"
#include "heap.h"

#include <klib/singleton.h>
#include <klib/multiboot.h>

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
class manager__
{
    friend class klib::singleton<manager__>;

    physical physical_;
    virt     virtual_;
    paging  *paging_ = nullptr;
    heap     heap_;
    bool     initialized_ = false;

private:
    manager__()
    {
    }

public:
    manager__(const manager__&) = delete;
    manager__(manager__&&) = delete;
    manager__ &operator=(const manager__&) = delete;
    manager__ &&operator=(manager__&&) = delete;

public:
    void initialize(multiboot_info_t *info, paging *pg);
    bool is_initialized() const;

    int mmap(vaddr_t addr, size_t size, uint8_t flags);
    void unmap(vaddr_t addr, size_t size);

    int mapio(uintptr_t dev_addr, size_t size, uint8_t flags);
    void unmapio(uintptr_t dev_addr);

    vaddr_t kalloc(size_t size);
    void kfree(vaddr_t addr);

    paddr_t create_address_space();
    void destroy_address_space(paddr_t addr);
};

using manager = klib::singleton<manager__>;

#endif // MANAGER_H
