#ifndef CONTEXT_H
#define CONTEXT_H

#include <klib/stdint.h>

namespace xenon
{
    class context
    {
    public:
        virtual ~context()
        {
        }

        virtual void update_context()              = 0;

        virtual void set_stack_ptr(void *sp)       = 0;
        virtual void set_base_ptr(void *bp)        = 0;
        virtual void set_instruction_ptr(void *ip) = 0;
    };
}

#endif // CONTEXT_H
