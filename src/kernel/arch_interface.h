#ifndef ARCH_INTERFACE_H
#define ARCH_INTERFACE_H

#include <klib/new.h>

namespace xenon
{
    class arch_interface
    {
    public:
        virtual ~arch_interface()
        {
        }

        virtual int init_interrupts() = 0;
        virtual int init_paging() = 0;
    };
}

#endif // ARCH_INTERFACE_H
