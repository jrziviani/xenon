#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "regs.h"

#include <klib/logger.h>

namespace xenon
{
    void excpetion_handler(const fault &r);
    void interrupt_handler(const fault &r);

    void print_registers(const regs &r);
}

#endif // EXCEPTION_H
