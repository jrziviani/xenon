#ifndef HEAP_H
#define HEAP_H

#include <klib/stdint.h>
#include <klib/bst.h>

namespace xenon
{
    class heap
    {
        struct space
        {
            size_t size;
            bool free;
            vaddr_t addr;

            space(vaddr_t a, size_t s) :
                size(s),
                free(true)
            {
                addr = ptr_to<vaddr_t>(ptr_from(a) + sizeof(space));
            }
        };

    private:
        vaddr_t start_;
        size_t size_;
        bst<space*> free_heap_;

    private:
        space *expand(size_t bytes);

    public:
        heap();
        heap(vaddr_t start, size_t size);

        void setup(vaddr_t start, size_t size);

        vaddr_t alloc(size_t size);
        void dealloc(vaddr_t addr);
    };
}

#endif // HEAP_H
