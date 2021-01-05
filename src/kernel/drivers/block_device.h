#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <klib/stdint.h>
#include <klib/string.h>
#include <klib/functional.h>
#include "klib/xenon_base.h"

class block_device : public xenon_base
{
    uint32_t block_size_;

public:
    void read(uint32_t offset, uint32_t blocks, klib::function &fn);
    void write(uint32_t offset, klib::string data, klib::function &fn);

    klib::string read(uint32_t offset, uint32_t block);
};

#endif // BLOCK_DEVICE_H
