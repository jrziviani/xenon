#include "amd64_process_controller.h"
#include "amd64_process.h"

#include <klib/new.h>

namespace xenon
{
    amd64_process_controller::amd64_process_controller()
    {
    }

    pid_t amd64_process_controller::create_process(context *ctx,
                                                   paddr_t kstack_addr,
                                                   size_t kstack_size)
    {
        process *proc = new amd64_process(ctx, kstack_addr, kstack_size);
        proc->set_state(PROC_STATE::READY);
        ready_queue_.push_back(proc);

        return proc->get_pid();
    }
}
