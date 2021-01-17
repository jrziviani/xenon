#ifndef EXT2_H
#define EXT2_H

#include "inode.h"

#include <drivers/block_device.h>

class ext2
{
    block_device &device_;

private:
    void read_block();
    void create_block();

public:
    ext2(block_device &device);
    void probe();
    bool mount();

    void list_root_directory();
    void list_directory(const ext2fs::inode &inode);
};

#endif // EXT2_H
