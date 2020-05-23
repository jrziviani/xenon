#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <arch/amd64/regs.h>

#include <klib/logger.h>
#include <klib/xenon_base.h>

namespace xenon
{
    void excpetion_handler(const fault &r);
    void interrupt_handler(const fault &r);

    void print_registers(const regs &r);

    void assign_irq(uint8_t irq, xenon_base *handler);
    void unassign_irq(uint8_t irq);
}

#endif // EXCEPTION_H
