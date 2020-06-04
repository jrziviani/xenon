#ifndef IDE_H
#define IDE_H

#include <klib/stdint.h>
#include <drivers/block_device.h>

namespace xenon
{
    struct ide_channel
    {
        uint16_t base;  // I/O base
        uint16_t ctrl;  // Control base
        uint16_t bmine; // Bus master IDE
        uint8_t  nIEN;  // nIEN (no interrupt)
    };

    struct ide_device
    {
        uint8_t  reserved;    // 1 - drive really exists
        uint8_t  channel;     // 0 - primary ch, 1 - secondary
        uint8_t  drive;       // 0 - master, 2 - slave
        uint16_t type;        // 0 - ATA, 1 - ATAPI
        uint16_t signature;   // drive signature
        uint16_t capabilities;// features
        uint32_t commands;    // command sets supported
        uint32_t size;        // size in sectors
        uint8_t  model[41];   // model
    };

    class ide : public block_device
    {
    public:
        ide();

        void initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4);

        void read_buffer(uint8_t channel, uint8_t reg, uint8_t buffer, uint8_t quads);

        uint8_t polling(uint8_t channel, uint8_t advance_check);

        uint8_t log_error(uint8_t drive, uint8_t err);

    public:
        int read(uint32_t index, char *buffer) final;
        int write(uint32_t index, const char *buffer) final;

    };
}

#endif // IDE_H
