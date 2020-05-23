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
        int create_context()            final;
        int create_process_controller(manager &memory_manager) final;
    };
}

#endif // AMD64_INTERFACE_H
