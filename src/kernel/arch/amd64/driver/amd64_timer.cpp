#include "amd64_timer.h"

#include <arch/amd64/bootstrap/exception.h>
#include <arch/amd64/instructions.h>
#include <klib/logger.h>

const uint16_t DATA_PORT_0  = 0x40;
const uint16_t COMMAND_PORT = 0x43;

const uint8_t REPEAT_MODE   = 0x36;

const uint32_t PIT_CLOCK = 1193180;

amd64_timer::amd64_timer(uint64_t frequency) :
    frequency_(frequency)
{
    if (frequency == 0) {
        // panic
        return;
    }

    uint8_t lo = (PIT_CLOCK / frequency) & 0xff;
    uint8_t hi = ((PIT_CLOCK / frequency) >> 8) & 0xff;

    outb(COMMAND_PORT, REPEAT_MODE);
    outb(DATA_PORT_0, lo);
    outb(DATA_PORT_0, hi);
}

amd64_timer::~amd64_timer()
{
}

void amd64_timer::wait_for(uint64_t miliseconds)
{
    clock::wait_for(miliseconds);
}
