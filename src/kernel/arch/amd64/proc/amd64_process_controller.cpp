#include "amd64_process_controller.h"
#include "amd64_process.h"

#include <arch/amd64/instructions.h>
#include <klib/new.h>

namespace xenon
{
    amd64_process_controller::amd64_process_controller()
    {
    }

    pid_t amd64_process_controller::create_process(uintptr_t kstack_addr,
                                                   size_t kstack_size,
                                                   uintptr_t program,
                                                   const char *name)
    {
        cli();
        process *proc = new amd64_process(kstack_addr,
                                          kstack_size,
                                          program,
                                          name);

        proc->set_state(PROC_STATE::READY);
        ready_queue_.push_back(proc);
        sti();

        return proc->get_pid();
    }
}
