#include "pagetable.h"
#include "../instructions.h"

#include <config.h>
#include <klib/string.h>
#include <klib/logger.h>
#include <memory/allocators.h>

#define PTE(addr)       ((addr >> 12) & 0x1ff)
#define PDE(addr)       ((addr >> 21) & 0x1ff)
#define PDPT(addr)      ((addr >> 30) & 0x1ff)
#define PML4(addr)      ((addr >> 39) & 0x1ff)

#define ADDRESS(addr)   ((reinterpret_cast<uintptr_t>(addr) & ~0xfff) + KVIRTUAL_ADDRESS)
#define PRESENT(addr)   ((reinterpret_cast<uintptr_t>(addr) & 0x1) == 1) 

namespace xenon
{
    void map_kernel_memory()
    {
        // get kernel page tables
        pml4_t *pml4_table = reinterpret_cast<pml4_t*>(ADDRESS(get_current_page()));
        pdpt_t *pdpt_table = reinterpret_cast<pdpt_t*>(ADDRESS(pml4_table->dirs[511]));
        pde_t  *pde_table  = reinterpret_cast<pde_t*>(ADDRESS(pdpt_table->dirs[510]));

        // get the last used physical address
        uintptr_t paddr = reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[3]))->pages[511];

        // map the whole PDE (1GiB)
        for (int i = 4; i < 512; i++) {
            pde_table->dirs[i] = pde_table->dirs[i - 1] + 4096;

            pte_t *pte = reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[i]));
            for (int j = 0; j < 512; j++) {
                pte->pages[j] = paddr + 0x1000;
                paddr = pte->pages[j];
            }
        }

#if 0
        *((uintptr_t*)0xffffffff80700000) = 70;
        xenon::logger::instance().log("9200: %d", *(uintptr_t*)0xffffffff80700000);
        *((uintptr_t*)0xffffffffbffff000) = 70;
        xenon::logger::instance().log("9200: %d", *(uintptr_t*)0xffffffffbffff000);
        //pte_t *tmp = reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[511]));
        tlb_flush(get_current_page());
        return;

        // unmap kernel stack boundaries
        // this will create two fences giving some protections
        // to the kernel stack against both underflow and overflow
        pte_t *pte = reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[1]));
        pte->pages[391] &= ~0xfff;
        pte = reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[2]));
        pte->pages[0] &= ~0xfff;
#endif
    }
}
