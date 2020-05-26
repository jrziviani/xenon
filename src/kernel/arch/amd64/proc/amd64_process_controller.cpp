#include "amd64_process_controller.h"
#include "amd64_process.h"

#include <klib/new.h>

namespace xenon
{
    amd64_process_controller::amd64_process_controller()
    {
    }

    pid_t amd64_process_controller::create_process(context *ctx)
    {
        process *proc = new amd64_process(ctx);
        ready_queue_.push_back(proc);
        proc->set_state(PROC_STATE::READY);

        return proc->get_pid();
    }
}
