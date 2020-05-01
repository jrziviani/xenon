#include "task.h"
#include "allocators.h"

namespace xenon
{
    pid_t next_pid = 0;

    task::task(context *ctx) :
        pid_(next_pid++),
        state_(TASK_STATE::CREATED),
        context_(ctx),
        parent_(nullptr)
    {
        context_->set_base_ptr(nullptr);
        context_->set_stack_ptr(nullptr);
        context_->set_instruction_ptr(nullptr);
    }

    pid_t task::get_pid()
    {
        return pid_;
    }

    task *task::get_parent()
    {
        return parent_;
    }

    TASK_STATE task::get_state()
    {
        return state_;
    }

    task *task::fork()
    {
        task *child = reinterpret_cast<task*>(placement_kalloc(sizeof(task)));
        child->parent_ = this;
        child->context_->update_context();
        return child;
    }

    void task::exit()
    {
    }

    task_controller::task_controller()
    {

    }

    pid_t task_controller::fork()
    {
        task *child = running_->fork();
        ready_queue_.push_back(child);

        if (running_ == child) {
            return 0;
        }
        else if (running_ == child->get_parent()) {
            return child->get_pid();
        }

        return 0;
    }
}
