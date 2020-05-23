#include "task.h"

namespace xenon
{
    pid_t next_pid = 0;

    task::task(context *ctx) :
        pid_(next_pid++),
        context_(ctx),
        parent_(nullptr),
        stack_len_(512_KB),
        code_len_(1_MB),
        state_(TASK_STATE::CREATED)
    {
        context_->set_base_ptr(nullptr);
        context_->set_stack_ptr(nullptr);
        context_->set_instruction_ptr(nullptr);
    }

    size_t task::get_stack_len() const
    {
        return stack_len_;
    }

    size_t task::get_code_len() const
    {
        return code_len_;
    }

    pid_t task::get_pid() const
    {
        return pid_;
    }

    task *task::get_parent()
    {
        return parent_;
    }

    TASK_STATE task::get_state() const
    {
        return state_;
    }

    void task::set_state(TASK_STATE state)
    {
        state_ = state;
    }

    task_controller::task_controller(manager &memory_manager) :
        memory_manager_(memory_manager)
    {
    }

    void task_controller::init_multitasking()
    {
    }

    void task_controller::create(context *ctx)
    {
        vaddr_t place = memory_manager_.kalloc(sizeof(task));
        task *process = new (place) task(ctx);
        ready_queue_.push_back(process);

        vaddr_t kstack = memory_manager_.kalloc(process->get_stack_len());
        char *stack_bottom = static_cast<char*>(kstack) + process->get_stack_len();
        ctx->set_stack_ptr(static_cast<vaddr_t>(stack_bottom));
        ctx->set_base_ptr(ctx->get_stack_ptr());

        process->set_state(TASK_STATE::READY);
    }

    pid_t task_controller::fork()
    {
        return 0;
    }
}
