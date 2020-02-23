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
        //uint8_t padding[48];
        uint64_t start;
    };
    using blocks = linked_list<block*>;

    class buddy
    {
        uint64_t start_;
        uint64_t len_;
        uint64_t end_;
        blocks free_list_[MAX_ORDER];

    public:
        buddy();
        void set_addresses(uint64_t start, uint64_t len);

        void alloc_page(uint8_t order);

    private:
        void init();
        block *get_block(uint64_t start, uint8_t order);
        block *get_free_block(uint8_t order);
        block *get_buddy(block &dude);

        block *alloc_recursive(uint8_t order);
        void free_recursive(block *blk);
    };
}

#endif // BUDDY_H
