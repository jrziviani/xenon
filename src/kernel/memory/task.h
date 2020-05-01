#ifndef TASK_H
#define TASK_H

#include "paging.h"
#include "context.h"

#include <config.h>
#include <klib/llist.h>
#include <arch/x86_64/regs.h>

namespace xenon
{
    enum class TASK_STATE : uint8_t
    {
        CREATED,
        READY,
        RUNNING,
        SLEEPING,
        ZOMBIE,
    };

    inline const char *task_state_name(TASK_STATE state)
    {
        switch (state)
        {
            case TASK_STATE::CREATED:
                return "created";

            case TASK_STATE::READY:
                return "ready";

            case TASK_STATE::RUNNING:
                return "running";

            case TASK_STATE::SLEEPING:
                return "sleeping";

            case TASK_STATE::ZOMBIE:
                return "zombie";

            default:
                return "invalid state";
        }
    }

    class task
    {
        pid_t           pid_;
        TASK_STATE      state_;
        dir_handler_t   *top_dir_;
        context         *context_;
        task            *parent_;

    public:
        task(context *ctx);

        pid_t get_pid();
        task *get_parent();
        TASK_STATE get_state();

        task *fork();
        int kill();
        void create();
        void wait();
        void sleep();
        void wakeup();
        void scheduler();
        void schedule();
        void exit();
    };

    class task_controller
    {
        using ready_queue = llist<task>;
        ready_queue ready_queue_;
        task *running_;

    public:
        task_controller();

        pid_t fork();
        void switch_task();
    };
}

#endif // TASK_H
