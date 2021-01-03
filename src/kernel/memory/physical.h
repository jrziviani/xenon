#ifndef PHYSICAL_H
#define PHYSICAL_H

#include <klib/stdint.h>

/*
 * physical memory
 *
 * Represents physical blocks of PAGE_FRAME size of memory.
 * An instance of 'physical' has a pointer to the list of physical
 * addresses available for use.
 *
 * +------------+
 * |  physical  |
 * |            |     +----+  +----+  +----+
 * | free_list_-+---->| 4K |->| 4K |->| 4K |-> NULL
 * +------------+     +----+  +----+  +----+
 *
 * In order to reserve (alloc) a frame, it simply removes the tail
 * node. When freeing, a new node will be pushed back into the list.
 */
class physical
{
    /*
     * frame is a 16-byte struct containing the physical
     * address and a pointer to the next struct
     * +--------+   +--------+
     * | 0x5000 |   | 0x6000 |
     * |    *---+-->|    *---+-->...--> NULL
     * +--------+   +--------+
     *
     * This solution consumes (total physical memory / PAGE_FRAME * 16).
     * For example, if we have 1GB of physical memory:
     *   - total nodes = 1,073,741,824 / 4,096 (4KiB PAGE_FRAME) = 262,144
     *   - each node costs 16 bytes * 262,144 nodes = 4194304 bytes
     *   - in other words, we need 4MiB to manage 1GiB.
     */
    struct frame
    {
        paddr_t address;
        frame  *previous;
    };

private:
    paddr_t physical_end_;
    frame  *free_list_;

public:
    physical();

    void setup(paddr_t start, size_t len);

    paddr_t alloc();
    paddr_t alloc(size_t blocks);
    void free(paddr_t addr);

public:
    ~physical()               = default;
    physical(const physical&) = delete;
    physical(physical &&)     = delete;

    physical &operator=(const physical&) = delete;
    physical &operator=(physical&&)      = delete;
};

#endif // PHYSICAL_H
