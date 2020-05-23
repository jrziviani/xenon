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
    /*
     *   ADDRESS    CONTENT     top_dir = 0x1000
     *   0x1000     0x8003
     *   0x1008     0x9003
     *   0x1010     0xA003
     *   0x1018     0xB003
     *   0x1020     0xC003
     *   ...        ...
     *
     *   top_dir contains the address to the first element of PML4 table
     *   (in other words, top_dir points to PML4), and the content of each
     *   PML4 entry (8 bytes each entry in this 64-bit world) is the
     *   address to the first PDPT element.
     *
     *   The scheme continues:
     *   ADDRESS    CONTENT
     *   0x8000     0x10003
     *   0x8008     0x11003
     *   0x8010     0x12003
     *   ...        ...
     *
     *   which can be represented as (in practice, sum 0x3 to all entries):
     *                PML4           PDPT           PDE             PTE              FRAME
     *   top_dir ---> 0x1000 ------> 0x8000 ------> 0x10000 ------> 0x100000 ------> 0x200000
     *                0x1008 ---+    0x8008 ---+    0x11000 ...     0x101000 ...     ...
     *                0x1010 -+ |    0x8010    |    0x12000         0x102000
     *                ...     | |    ...      ...   ...             ...
     *                        | +--> 0x9000 ------> 0xN0000
     *                        |      0x9008 ---+    0xN1000
     *                       ...     0x9010    |    0xN2000
     *                               ...      ...   ...
     */ 
    pte_t *x86_paging::get_page(paddr_t top_dir, vaddr_t vaddr, bool make)
    {
        auto pml4 = PML4(vaddr);
        auto pdpt = PDPT(vaddr);
        auto pde  = PDE(vaddr);

        // top_dir points to PML4 table and PML4(vaddr) is the index, this cell
        // points to the PDPT table. this code checks if the PDPT table at
        // PML4[index] is present, if it's not present (and the caller wants it
        // present) we allocate a new PDPT table and store its (physical)
        // addresss at PML4[index]. If the page is not present and the caller
        // doesn't want to make it present, simply return null.
        //
        // it's important to notice that we'll store the physical address but the
        // table itself is managed using the virtual address. thanks to identity
        // mapping, we can be sure that:
        //     virtual address = physical address & ~0xfff + KVIRTUAL_ADDRESS
        pml4_t *pml4_table = reinterpret_cast<pml4_t*>(ADDRESS(top_dir));
        if (!PRESENT(pml4_table->dirs[pml4]) && make) {
            paddr_t paddr;
            pdpt_t *tmp = reinterpret_cast<pdpt_t*>(placement_kalloc(sizeof(pdpt_t), &paddr, true));
            memset(tmp, 0, sizeof(pdpt_t));
            pml4_table->dirs[pml4] = ptr_from(paddr) + 0x3;
        }
        else if (!PRESENT(pml4_table->dirs[pml4])) {
            return nullptr;
        }

        // PML4[index] points to a PDPT table and PDPT(vaddr) is the index, this
        // PDPT[index] points to the PDE table and the process is exactly the same
        // used by PML4 above.
        pdpt_t *pdpt_table = reinterpret_cast<pdpt_t*>(ADDRESS(pml4_table->dirs[pml4]));
        if (!PRESENT(pdpt_table->dirs[pdpt]) && make) {
            paddr_t paddr;
            pde_t *tmp = reinterpret_cast<pde_t*>(placement_kalloc(sizeof(pde_t), &paddr, true));
            memset(tmp, 0, sizeof(pde_t));
            pdpt_table->dirs[pdpt] = ptr_from(paddr) + 0x3;
        }
        else if (!PRESENT(pdpt_table->dirs[pdpt])) {
            return nullptr;
        }

        // PDPT[index] points to a PDE table and PDE(vaddr) is the index, this PDE[index]
        // points to the PTE table.
        pde_t *pde_table = reinterpret_cast<pde_t*>(ADDRESS(pdpt_table->dirs[pdpt]));
        if (!PRESENT(pde_table->dirs[pde]) && make) {
            paddr_t paddr;
            pte_t *tmp = reinterpret_cast<pte_t*>(placement_kalloc(sizeof(pte_t), &paddr, true));
            memset(tmp, 0, sizeof(pte_t));
            pde_table->dirs[pde] = ptr_from(paddr) + 0x3;
        }
        else if (!PRESENT(pde_table->dirs[pde])) {
            return nullptr;
        }

        // returns a pointer to PTE table
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
