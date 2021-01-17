#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <klib/stdint.h>
#include <klib/cmemory.h>
#include <klib/functional.h>
#include "klib/xenon_base.h"

class block_device : public xenon_base
{
protected:
    size_t sector_size_;

protected:
    explicit block_device(size_t sector_size) :
        sector_size_(sector_size)
    {
    }

public:
    virtual ~block_device()
    {
    }

    size_t get_sector_size() const
    {
        return sector_size_;
    }

    virtual klib::unique_ptr<char[]> read(size_t offset, size_t bytes) = 0;

    /*
    void read(uint32_t offset, uint32_t blocks, klib::function &fn);
    void write(uint32_t offset, klib::string data, klib::function &fn);

    klib::string read(uint32_t offset, uint32_t block);
    */
};

#endif // BLOCK_DEVICE_H
