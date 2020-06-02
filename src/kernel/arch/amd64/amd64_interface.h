#ifndef AMD64_INTERFACE_H
#define AMD64_INTERFACE_H

#include "arch_interface.h"

#include <proc/context.h>
#include <klib/timer.h>
#include <klib/logger.h>

namespace xenon
{
    class amd64_interface : public arch_interface
    {
    public:
        int init_interrupts()           final;
        int init_paging()               final;
        int init_timer()                final;
        int init_processes()            final;
        int init_pci()                  final;

        void cpu_halt()                 final;
    };
}

#endif // AMD64_INTERFACE_H
