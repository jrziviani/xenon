#include "buddy.h"
#include "kmalloc.h"
#include "new.h"
#include "libs/logger.h"

namespace xenon
{
    const uint32_t ORDER_SIZES[] = {
        2'097'152,  //   2MiB
        1'048'576,  //   1MiB
        524'288,    // 512KiB
        262'144,    // 256KiB
        131'072,    // 128KiB
        65'536,     //  64KiB
        32'768,     //  32KiB
        16'384,     //  16KiB
        8'192,      //   8KiB
        4'096,      //   4KiB
    };

    buddy::buddy()
    {
    }

    void buddy::set_addresses(uint64_t start, uint64_t len)
    {
        start_ = start;
        len_ = len;
        end_ = start_ + len_;

        init();
    }

    void buddy::alloc_page(uint8_t order)
    {
        alloc_recursive(order);
    }

    void buddy::init()
    {
        uint64_t total = len_ / ORDER_SIZES[MAX_ORDER - 1];
        uint32_t max_order_size = ORDER_SIZES[MAX_ORDER - 1];

        logger::instance().log("total: %d - max_order_size: %d",
                               total, max_order_size);

        for (uint8_t i = 0; i < total; i++) {
            uint64_t block_offset = i * max_order_size;
            block *blk = new block{1, MAX_ORDER - 1, start_ + block_offset};
            /*
            free_list_[MAX_ORDER - 1].push_back(blk);
            */
        }
    }

    block *buddy::get_block(uint64_t start, uint8_t order)
    {
        // try to find a block beginning at start
        return free_list_[order].find_or([start](block *blk) -> bool {
            return start == blk->start;
        }, nullptr);
    }

    block *buddy::get_free_block(uint8_t order)
    {
        // try to find a free block
        return free_list_[order].find_or([](const block *blk) -> bool {
            return blk->free ? true : false;
        }, nullptr);
    }

    block *buddy::get_buddy(block &dude)
    {
        uint64_t buddy_start = dude.start;

        // if dude is on an odd address, buddy is on its right
        if (dude.start / ORDER_SIZES[dude.order] & 0x1) {
            buddy_start += ORDER_SIZES[dude.order];
        }
        // otherwise, buddy is on its left
        else {
            buddy_start -= ORDER_SIZES[dude.order];
        }

        // now get the buddy's block
        return get_block(buddy_start, dude.order);
    }

    block *buddy::alloc_recursive(uint8_t order)
    {
        // try to find a free block
        block *blk = get_free_block(order);

        // free block, mark it not free and return
        if (blk != nullptr) {
            blk->free = 0;
            return blk;
        }
        // no free block, we're out of memory
        else if (blk == nullptr && order + 1 >= MAX_ORDER) {
            return nullptr;
        }
        // no free block, check higher order and create buddies
        blk = alloc_recursive(static_cast<uint8_t>(order + 1));
        free_list_[order].push_back(new block{0, order, blk->start});
        free_list_[order].push_back(new block{1, order, blk->start + (2 << order << PAGE_SHIFT)});

        return free_list_[order].front_or(nullptr);
    }

    void buddy::free_recursive(block *blk)
    {
        auto min = [](uint64_t a, uint64_t b) -> uint64_t {
            return (a > b) ? a : b;
        };

        // check blk's buddy is free
        block *buddy = get_buddy(*blk);
        if (buddy == nullptr) {
            // panic
        }
        // buddy is free too, remove buddies and check if
        // parent can be released as well
        else if (buddy->free == 1) {
            block *parent = get_block(min(buddy->start, blk->start),
                                      static_cast<uint8_t>(blk->order + 1));

            // delete buddy and blk
            delete buddy;
            delete blk;
            free_recursive(parent);
        }
        // buddy is not free, set only current block free
        else {
            blk->free = 1;
        }
    }
}
