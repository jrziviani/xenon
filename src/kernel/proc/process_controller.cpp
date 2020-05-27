#include "process_controller.h"

#include <memory/paging.h>

namespace xenon
{
    process_controller::process_controller()
    {
    }

    void process_controller::set_running(pid_t pid)
    {
        auto proc = get_process_by_pid(pid);
        ready_queue_.remove(proc);
        set_running(proc);
    }

    void process_controller::set_running(process *proc)
    {
        running_ = proc;
        proc->set_state(PROC_STATE::RUNNING);
        proc->switch_process(running_);
    }

    void process_controller::set_running_from_queue()
    {
        set_running(ready_queue_.pop_back());
    }

    void process_controller::user_init()
    {
        /*
        context_regs *regs = running_->get_context()->get_regs();
        paddr_t address_space = memory_manager_.create_address_space();
        */
    }

    process *process_controller::get_process_by_pid(pid_t pid)
    {
        for (auto proc : ready_queue_) {
            if (proc->get_pid() == pid) {
                return proc;
            }
        }

        return nullptr;
    }

    void process_controller::switch_to(process *proc)
    {
        (void)proc;
        // 1. save current_ process context
        // 2. add current_ process into ready_queue_
        // 3. setup proc context
        // 4. run
    }
}
