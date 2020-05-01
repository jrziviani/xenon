#include "x86_timer.h"
#include "instructions.h"
#include "exception.h"

#include <klib/logger.h>

const uint16_t DATA_PORT_0  = 0x40;
const uint16_t COMMAND_PORT = 0x43;

const uint8_t REPEAT_MODE   = 0x36;

const uint32_t PIT_CLOCK = 1193180;

namespace xenon
{
    x86_timer::x86_timer(uint64_t frequency) :
        frequency_(frequency),
        tick_(0)
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

        assign_irq(0, this);
    }

    x86_timer::~x86_timer()
    {
        unassign_irq(0);
    }

    void x86_timer::on_time(uint64_t freq)
    {
        for (auto obj : registered_) {
            obj->on_time(freq);
        }

        //logger::instance().log("%d: on time :-)", freq);
    }
}
