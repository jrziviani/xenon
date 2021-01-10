#ifndef EXT2_H
#define EXT2_H

#include <drivers/device_interface.h>

class ext2
{
    device_interface *device_;

private:
    void read_block();
    void create_block();

public:
    ext2(device_interface *device);
    void probe();
    bool mount();
};

#endif // EXT2_H
