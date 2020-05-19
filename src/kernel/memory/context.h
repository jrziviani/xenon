#ifndef CONTEXT_H
#define CONTEXT_H

#include <klib/stdint.h>

namespace xenon
{
    class context
    {
    protected:
        vaddr_t instruction_ptr_;
        vaddr_t stack_ptr_;
        vaddr_t frame_ptr_;
        paddr_t top_dir_;

    public:
        virtual ~context()
        {
        }

        virtual void update_context()                = 0;

        virtual void set_stack_ptr(vaddr_t sp)       = 0;
        virtual void set_base_ptr(vaddr_t bp)        = 0;
        virtual void set_instruction_ptr(vaddr_t ip) = 0;
        virtual void set_top_dir(paddr_t dir)        = 0;

        vaddr_t get_stack_ptr() const
        {
            return stack_ptr_;
        }

        vaddr_t get_base_ptr() const
        {
            return frame_ptr_;
        }

        vaddr_t get_instruction_ptr() const
        {
            return instruction_ptr_;
        }

        paddr_t get_dir() const
        {
            return top_dir_;
        }
    };
}

#endif // CONTEXT_H
