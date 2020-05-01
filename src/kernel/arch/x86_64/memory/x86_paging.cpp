#include "x86_paging.h"
#include "config.h"

#include <klib/logger.h>
#include <memory/allocators.h>
#include <arch/x86_64/instructions.h>

#define PTE(addr)       ((ptr_from(addr) >> 12) & 0x1ff)
#define PDE(addr)       ((ptr_from(addr) >> 21) & 0x1ff)
#define PDPT(addr)      ((ptr_from(addr) >> 30) & 0x1ff)
#define PML4(addr)      ((ptr_from(addr) >> 39) & 0x1ff)

#define ADDRESS(addr)   ((ptr_from(addr) & ~0xfff) + KVIRTUAL_ADDRESS)
#define PRESENT(addr)   ((ptr_from(addr) & 0x1) == 1) 

namespace xenon
{
    pte_t *x86_paging::get_page(paddr_t top_dir, vaddr_t vaddr, bool make)
    {
        auto pml4 = PML4(vaddr);
        auto pdpt = PDPT(vaddr);
        auto pde  = PDE(vaddr);

        pml4_t *pml4_table = reinterpret_cast<pml4_t*>(ADDRESS(top_dir));
        if (!PRESENT(pml4_table->dirs[pml4]) && make) {
            uintptr_t physical;
            pdpt_t *tmp = reinterpret_cast<pdpt_t*>(placement_kalloc(sizeof(pdpt_t), &physical, true));
            memset(tmp, 0, sizeof(pdpt_t));
            pml4_table->dirs[pml4] = physical + 0x3;
        }
        else if (!PRESENT(pml4_table->dirs[pml4])) {
            return nullptr;
        }

        pdpt_t *pdpt_table = reinterpret_cast<pdpt_t*>(ADDRESS(pml4_table->dirs[pml4]));
        if (!PRESENT(pdpt_table->dirs[pdpt]) && make) {
            uintptr_t physical;
            pde_t *tmp = reinterpret_cast<pde_t*>(placement_kalloc(sizeof(pde_t), &physical, true));
            memset(tmp, 0, sizeof(pde_t));
            pdpt_table->dirs[pdpt] = physical + 0x3;
        }
        else if (!PRESENT(pdpt_table->dirs[pdpt])) {
            return nullptr;
        }

        pde_t *pde_table = reinterpret_cast<pde_t*>(ADDRESS(pdpt_table->dirs[pdpt]));
        if (!PRESENT(pde_table->dirs[pde]) && make) {
            uintptr_t physical;
            pte_t *tmp = reinterpret_cast<pte_t*>(placement_kalloc(sizeof(pte_t), &physical, true));
            memset(tmp, 0, sizeof(pte_t));
            pde_table->dirs[pde] = physical + 0x3;
        }
        else if (!PRESENT(pde_table->dirs[pde])) {
            return nullptr;
        }

        return reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[pde]));
    }

    void x86_paging::unmap(paddr_t top_dir, vaddr_t vaddr)
    {
        auto addr = ptr_from(vaddr);
        auto pml4 = PML4(addr);
        auto pdpt = PDPT(addr);
        auto pde  = PDE(addr);
        auto pte  = PTE(addr);

        pml4_t *pml4_table = reinterpret_cast<pml4_t*>(ADDRESS(top_dir));
        if (!PRESENT(pml4_table->dirs[pml4])) {
            logger::instance().log("PANIC: unmap: pml4 not present");
            return;
        }

        pdpt_t *pdpt_table = reinterpret_cast<pdpt_t*>(ADDRESS(pml4_table->dirs[pml4]));
        if (!PRESENT(pdpt_table->dirs[pdpt])) {
            logger::instance().log("PANIC: unmap: pdpt not present");
            return;
        }

        pde_t *pde_table = reinterpret_cast<pde_t*>(ADDRESS(pdpt_table->dirs[pdpt]));
        if (!PRESENT(pde_table->dirs[pde])) {
            logger::instance().log("PANIC: unmap: pde not present");
            return;
        }

        pte_t *entry = reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[pde]));
        if (!PRESENT(entry->pages[pte])) {
            logger::instance().log("PANIC: unmap: pte not present");
            return;
        }

        entry->pages[pte] &= ~0xfff;
        tlb_flush(0);
    }

    void x86_paging::unmap(vaddr_t vaddr)
    {
        unmap(get_current_page(), vaddr);
    }

    int x86_paging::map(paddr_t dir, vaddr_t vaddr, paddr_t paddr, uint8_t flags)
    {
        (void)flags;

        auto pte  = PTE(ptr_from(vaddr));

        pte_t *page = get_page(dir, vaddr, true);
        page->pages[pte] = ptr_from(paddr) + 0x3;

        tlb_flush(dir);

        return 0;
    }

    int x86_paging::map(vaddr_t vaddr, paddr_t paddr, uint8_t flags)
    {
        return map(get_current_page(), vaddr, paddr, flags);
    }
}
