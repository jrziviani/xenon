#include "process_controller.h"

#include <memory/paging.h>
#include <klib/logger.h>

namespace xenon
{
    void program_a()
    {
        logger::instance().log("Running program_a");
        while (true) { asm volatile ("sti \t\n"
                                     "hlt \t\n" : :); }
    }

    void program_b()
    {
        logger::instance().log("Running program_b");
        while (true) { asm volatile ("sti \t\n"
                                     "hlt \t\n" : :); }
    }

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
        if (ready_queue_.empty()) {
            return;
        }

        auto next_proc = ready_queue_.pop_front();
        running_->switch_process(next_proc);
        running_->set_state(PROC_STATE::READY);
        ready_queue_.push_back(running_);
        running_ = next_proc;
        running_->set_state(PROC_STATE::RUNNING);
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

    void process_controller::exit()
    {
        if (ready_queue_.empty()) {
            return;
        }

        auto next_proc = ready_queue_.pop_front();
        auto destroy = running_;
        running_->switch_process(next_proc);
        running_ = next_proc;
        running_->set_state(PROC_STATE::RUNNING);

        delete destroy;
        destroy = nullptr;
    }

    void process_controller::create_dummy_processes()
    {
        create_process(0,
                       64_KB,
                       ptr_from(&program_a),
                       "[program_a]");

        create_process(0,
                       64_KB,
                       ptr_from(&program_b),
                       "[program_b]");
    }
}
