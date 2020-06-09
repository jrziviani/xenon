#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <arch/amd64/regs.h>

#include <klib/logger.h>
#include <klib/xenon_base.h>
#include <drivers/irq_handler.h>

namespace xenon
{
    void excpetion_handler(const fault &r);
    void interrupt_handler(const fault &r);

    void print_registers(const regs &r);

    void assign_irq(uint8_t irq, irq_handler *handler);
    void unassign_irq(uint8_t irq);

    namespace clock
    {
        void wait_for(uint64_t miliseconds);
    }
}

#endif // EXCEPTION_H
