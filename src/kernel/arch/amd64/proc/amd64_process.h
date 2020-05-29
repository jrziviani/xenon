#ifndef AMD64_PROCESS_H
#define AMD64_PROCESS_H

#include "amd64_context.h"

#include <proc/process.h>

namespace xenon
{
    class amd64_process : public process
    {
        amd64_context ctx_;

    public:
        amd64_process(uintptr_t kstack_addr,
                      size_t kstack_size,
                      uintptr_t program,
                      const char *name);

    public:
        context *get_context()                 final;
        void switch_process(process *newp)     final;
        void set_program_address(vaddr_t addr) final;
    };
}

#endif // AMD64_PROCESS_H
