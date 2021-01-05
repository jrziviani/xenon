#ifndef AMD64_PAGING_H
#define AMD64_PAGING_H

#include "pagetable.h"

#include <memory/paging.h>

class amd64_paging : public paging
{
    uintptr_t *kernel_pages_;
    pml4_t *kernel_directory_;

    private:
    pte_t *get_page(paddr_t top_dir, vaddr_t vaddr, bool make);

    public:
    amd64_paging()
    {
    }

    ~amd64_paging()
    {
    }

    int map(paddr_t top_dir, vaddr_t vaddr, paddr_t paddr, uint8_t flags) final;
    int map(vaddr_t vaddr, paddr_t paddr, uint8_t flags)                  final;

    void unmap(paddr_t top_dir, vaddr_t vaddr) final;
    void unmap(vaddr_t vaddr)                  final;

    vaddr_t mapio(uintptr_t addr, uint8_t flags) final;
    void unmapio(vaddr_t vaddr)                  final;

    paddr_t create_top_page_directory() final;
}; 

#endif // AMD64_PAGING_H
