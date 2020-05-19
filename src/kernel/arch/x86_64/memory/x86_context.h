#ifndef X86_CONTEXT_H
#define X86_CONTEXT_H

#include <arch/x86_64/instructions.h>
#include <klib/stdint.h>
#include <memory/context.h>

namespace xenon
{
    class x86_context : public context
    {
    public:
        void update_context() final
        {
            stack_ptr_ = ptr_to<vaddr_t>(movfrsp());
            frame_ptr_ = ptr_to<vaddr_t>(movfrbp());
            instruction_ptr_ = ptr_to<vaddr_t>(movfrip());
            top_dir_ = get_current_page();
        }

        void set_stack_ptr(vaddr_t sp) final
        {
            stack_ptr_ = sp;
        }

        void set_base_ptr(vaddr_t bp) final
        {
            frame_ptr_ = bp;
        }

        void set_instruction_ptr(vaddr_t ip) final
        {
            instruction_ptr_ = ip;
        }

        void set_top_dir(paddr_t dir) final
        {
            top_dir_ = dir;
        }
    };
}

#endif // X86_CONTEXT_H
