#include "process_controller.h"

#include <memory/paging.h>

namespace xenon
{
    process_controller::process_controller()
    {
    }

    void process_controller::set_running(pid_t pid)
    {
        (void)pid;
        for (auto proc : ready_queue_) {
            (void)proc;
        }
    }

    void process_controller::set_running(process *proc)
    {
        running_ = proc;
    }

    void process_controller::set_running_from_queue()
    {
        running_ = ready_queue_.pop_back();
    }

    void process_controller::user_init()
    {
        /*
        context_regs *regs = running_->get_context()->get_regs();
        paddr_t address_space = memory_manager_.create_address_space();
        */
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
