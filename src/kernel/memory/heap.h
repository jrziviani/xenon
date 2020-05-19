#ifndef HEAP_H
#define HEAP_H

#include <klib/stdint.h>
#include <klib/bst.h>

namespace xenon
{
    class heap
    {
        struct node
        {
            size_t size;
            bool free;
            vaddr_t addr;

            node(vaddr_t a, size_t s) :
                size(s),
                free(true),
                addr(a)
            {
            }
        };

    private:
        vaddr_t start_;
        size_t size_;
        bst<node*> free_heap_;

    private:
        node *expand(size_t bytes);

    public:
        heap();
        heap(vaddr_t start, size_t size);

        void setup(vaddr_t start, size_t size);

        vaddr_t alloc(size_t size);
        void dealloc(vaddr_t addr);
    };
}

#endif // HEAP_H
