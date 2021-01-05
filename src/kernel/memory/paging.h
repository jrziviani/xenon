#ifndef PAGING_H
#define PAGING_H

#include <klib/stdint.h>

class paging
{
public:
    paging()
    {
    }

    virtual ~paging()
    {
    }

    virtual int map(paddr_t top_dir, vaddr_t vaddr, paddr_t paddr, uint8_t flags) = 0;
    virtual int map(vaddr_t vaddr, paddr_t paddr, uint8_t flags)                  = 0;

    virtual void unmap(paddr_t top_dir, vaddr_t vaddr)                            = 0;
    virtual void unmap(vaddr_t vaddr)                                             = 0;

    virtual vaddr_t mapio(uintptr_t addr, uint8_t flags)                          = 0;
    virtual void unmapio(vaddr_t vaddr)                                           = 0;

    virtual paddr_t create_top_page_directory()                                   = 0;
};

#endif // PAGING_H
