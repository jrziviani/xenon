#ifndef X86_INTERFACE_H
#define X86_INTERFACE_H

#include "arch_interface.h"

#include <klib/logger.h>

namespace xenon
{
    class x86_interface : public arch_interface
    {
    public:
        int init_interrupts() final;
        int init_paging()     final;
    };
}

#endif // X86_INTERFACE_H
