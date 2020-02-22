#ifndef BUDDY_H
#define BUDDY_H

#include "stdint.h"
#include "config.h"
#include "linked_list.h"

namespace xenon
{
    struct block
    {
        uint8_t free;
        uint8_t order;
        uint8_t padding[48];
        uint64_t start;
    };
    using blocks = linked_list<block*>;


    class buddy
    {
        blocks[MAX_ORDER] free_list_;

    public:
        buddy(uint64_t start, uint64_t end);

        void alloc_page(uint8_t order);

    private:
        void init();
        block *get_block(uint64_t start, uint8_t order);
        block *get_free_block(uint8_t order);
        block *get_buddy(block &dude);

        block *alloc_recursive(uint8_t order);
        void free_recursive(block &blk);

        void coalesce(uint8_t order);
    };
}

#endif // BUDDY_H
