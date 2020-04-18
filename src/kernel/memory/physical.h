#ifndef PHYSICAL_H
#define PHYSICAL_H

#include <klib/stdint.h>

namespace xenon
{
    class free_list
    {
        struct list
        {
            uintptr_t address;
            list *previous;
        };

        struct kmem
        {
            list *tail;
        };

    private:
        uint64_t physical_end_;
        kmem free_list_;

    public:
        free_list();

        void setup(uint64_t start, uint64_t len);

        uintptr_t alloc();
        void free(uintptr_t physical_addr);
    };
}

#endif // PHYSICAL_H
