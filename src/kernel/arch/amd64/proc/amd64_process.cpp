#include "amd64_process.h"
#include <arch/amd64/instructions.h>

// function prototype, implemented in assembly
extern "C" void switch_context(xenon::context_regs *o, xenon::context_regs *n);

namespace xenon
{
    amd64_process::amd64_process(uintptr_t kstack_addr,
                                 size_t kstack_size,
                                 uintptr_t nip,
                                 const char *name) :
        process(kstack_addr, kstack_size, nip, name)
    {
        ctx_.get_regs()->rsi = 0;
        ctx_.get_regs()->rdi = 0;
        ctx_.get_regs()->rsp = kstack_addr_;
        ctx_.get_regs()->rip = nip;
        ctx_.get_regs()->cr3 = top_dir_;
    }

    context *amd64_process::get_context()
    {
        return &ctx_;
    }

    void amd64_process::switch_process(process *newp)
    {
        if (newp == this) {
            return;
        }

        uintptr_t ip;
        asm volatile ("movq $(.quit), %0    \t\n"
                      : "=a"(ip));

        context_regs *running = get_context()->get_regs();
        context_regs *new_context = newp->get_context()->get_regs();

        running->rip = ip;

        switch_context(running, new_context);

        asm volatile (".quit:   \t\n");
    }

    void amd64_process::set_program_address(vaddr_t addr)
    {
        this->get_context()->get_regs()->rip = ptr_from(addr);
    }
}
