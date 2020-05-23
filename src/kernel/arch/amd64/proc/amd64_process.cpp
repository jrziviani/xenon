#include "amd64_process.h"

// function prototype, implemented in assembly
extern "C" void switch_context(xenon::context_regs *o, xenon::context_regs *n);

namespace xenon
{
    amd64_process::amd64_process(context *context) :
        process(context)
    {
    }

    void amd64_process::switch_process(process *newp)
    {
        context_regs *o = this->get_context()->get_regs();
        context_regs *n = newp->get_context()->get_regs();
        switch_context(o, n);
    }
}
