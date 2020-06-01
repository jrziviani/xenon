#ifndef IDE_H
#define IDE_H

#include <klib/stdint.h>

namespace xenon
{
    class ide
    {
    public:
        ide();

        void initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4);

        uint8_t read(uint8_t channel, uint8_t reg);
        void write(uint8_t channel, uint8_t reg, uint8_t data);

        void read_buffer(uint8_t channel, uint8_t reg, uint8_t buffer, uint8_t quads);

        uint8_t polling(uint8_t channel, uint8_t advance_check);

        uint8_t log_error(uint8_t drive, uint8_t err);

    };
}

#endif // IDE_H
