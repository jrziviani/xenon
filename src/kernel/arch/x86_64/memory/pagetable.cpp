#include "pagetable.h"

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

inline uintptr_t get_current_page()
{
    uintptr_t cr3;
    asm volatile("mov %%cr3, %0"
                 : "=r"(cr3));

    return cr3;
}

inline void tlb_flush(uintptr_t addr)
{
    asm volatile("invlpg (%0)"
                 :
                 : "r"(addr)
                 : "memory");
}

inline void switch_page(uintptr_t addr)
{
    asm volatile("mov %0, %%cr3"
                 :
                 : "r"(addr));
}

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
        //pte_t *pte = reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[1]));
        //pte->pages[391] &= ~0xfff;
        //pte = reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[2]));
        //pte->pages[0] &= ~0xfff;
    }

    page_t::page_t() :
        kernel_pages_(nullptr)
    {
        map_kernel_memory();
    }

    uintptr_t page_t::vaddr_to_paddr(uintptr_t vaddr)
    {
        (void)vaddr;
        if (kernel_pages_ == nullptr) {
            return 0;
        }

        return 0;
    }

    pte_t *page_t::get_page(uintptr_t top_dir, uintptr_t vaddr, bool make)
    {
        (void)make;

        auto pml4 = PML4(vaddr);
        auto pdpt = PDPT(vaddr);
        auto pde  = PDE(vaddr);

        pml4_t *pml4_table = reinterpret_cast<pml4_t*>(ADDRESS(top_dir));
        if (!PRESENT(pml4_table->dirs[pml4])) {
            uintptr_t physical;
            pdpt_t *tmp = reinterpret_cast<pdpt_t*>(placement_kalloc(sizeof(pdpt_t), &physical));
            memset(tmp, 0, sizeof(pdpt_t));
            pml4_table->dirs[pml4] = physical + 0x3;
        }

        pdpt_t *pdpt_table = reinterpret_cast<pdpt_t*>(ADDRESS(pml4_table->dirs[pml4]));
        if (!PRESENT(pdpt_table->dirs[pdpt])) {
            uintptr_t physical;
            pde_t *tmp = reinterpret_cast<pde_t*>(placement_kalloc(sizeof(pde_t), &physical));
            memset(tmp, 0, sizeof(pde_t));
            pdpt_table->dirs[pdpt] = physical + 0x3;
        }

        pde_t *pde_table = reinterpret_cast<pde_t*>(ADDRESS(pdpt_table->dirs[pdpt]));
        if (!PRESENT(pde_table->dirs[pde])) {
            uintptr_t physical;
            pte_t *tmp = reinterpret_cast<pte_t*>(placement_kalloc(sizeof(pte_t), &physical));
            memset(tmp, 0, sizeof(pte_t));
            pde_table->dirs[pde] = physical + 0x3;
        }

        pte_t *entry = reinterpret_cast<pte_t*>(ADDRESS(pde_table->dirs[pde]));
        return entry;
    }

    void page_t::unmap(uintptr_t top_dir, void *vaddr)
    {
        uintptr_t addr = reinterpret_cast<uintptr_t>(vaddr);
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

    int page_t::map(uintptr_t dir, void *vaddr, uint64_t paddr, uint8_t flags)
    {
        (void)flags;

        uintptr_t uvaddr = reinterpret_cast<uintptr_t>(vaddr);

        auto pte  = PTE(uvaddr);

        pte_t *page = get_page(dir, uvaddr, true);
        page->pages[pte] = paddr + 0x3;

        tlb_flush(0);

        return 0;
    }

    int page_t::map(void *vaddr, uint64_t paddr, uint8_t flags)
    {
        return map(get_current_page(), vaddr, paddr, flags);
    }
}
