#ifndef X86_CONTEXT_H
#define X86_CONTEXT_H

#include <arch/x86_64/instructions.h>
#include <klib/stdint.h>
#include <memory/context.h>

namespace xenon
{
    class x86_context : public context
    {
        uint64_t rip_;
        uint64_t rsp_;
        uint64_t rbp_;

    public:
        void update_context() final
        {
            rsp_ = movfrsp();
            rbp_ = movfrbp();
            rip_ = movfrip();
        }

        void set_stack_ptr(void *sp) final
        {
            rsp_ = reinterpret_cast<uint64_t>(sp);
        }

        void set_base_ptr(void *bp) final
        {
            rbp_ = reinterpret_cast<uint64_t>(bp);
        }

        void set_instruction_ptr(void *ip) final
        {
            rip_ = reinterpret_cast<uint64_t>(ip);
        }
    };
}

#endif // X86_CONTEXT_H
