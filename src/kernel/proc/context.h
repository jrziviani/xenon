#ifndef CONTEXT_H
#define CONTEXT_H

#include <klib/stdint.h>

namespace xenon
{
    struct context_regs;

    class context
    {
    public:
        virtual ~context()               = 0;
        virtual context_regs *get_regs() = 0;
        virtual void save()              = 0;
    };
}

#endif // CONTEXT_H
