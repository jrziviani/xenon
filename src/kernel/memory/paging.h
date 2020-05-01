#ifndef PAGING_H
#define PAGING_H

#include <klib/stdint.h>

namespace xenon
{
    using dir_handler_t = void*;
    using page_handler_t = void*;

    class paging
    {
    public:
        paging()
        {
        }

        virtual ~paging()
        {
        }

        dir_handler_t clone_dir(dir_handler_t src);
        page_handler_t clone_page(page_handler_t src);

        virtual int map(paddr_t top_dir, vaddr_t vaddr, paddr_t paddr, uint8_t flags) = 0;
        virtual int map(vaddr_t vaddr, paddr_t paddr, uint8_t flags)                  = 0;

        virtual void unmap(paddr_t top_dir, vaddr_t vaddr)                            = 0;
        virtual void unmap(vaddr_t vaddr)                                             = 0;
    };
}

#endif // PAGING_H
