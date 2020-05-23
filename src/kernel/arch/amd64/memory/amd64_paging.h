#ifndef X86_PAGING_H
#define X86_PAGING_H

#include "pagetable.h"

#include <memory/paging.h>

namespace xenon
{
    class x86_paging : public paging
    {
        uintptr_t *kernel_pages_;
        pml4_t *kernel_directory_;

    private:
        pte_t *get_page(paddr_t top_dir, vaddr_t vaddr, bool make);

    public:
        x86_paging()
        {

        }

        int map(paddr_t top_dir, vaddr_t vaddr, paddr_t paddr, uint8_t flags) final;
        int map(vaddr_t vaddr, paddr_t paddr, uint8_t flags)                  final;

        void unmap(paddr_t top_dir, vaddr_t vaddr)                            final;
        void unmap(vaddr_t vaddr)                                             final;
    }; 
}

#endif // X86_PAGING_H
