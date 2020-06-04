#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <klib/stdint.h>

namespace xenon
{
    class block_device
    {
    public:
        virtual int read(uint32_t index, char *buffer)        = 0;
        virtual int write(uint32_t index, const char *buffer) = 0;
    };
}

#endif // BLOCK_DEVICE_H
