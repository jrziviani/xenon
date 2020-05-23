#ifndef TASK_H
#define TASK_H

#include <memory/paging.h>
#include <memory/context.h>
#include <memory/manager.h>

#include <config.h>
#include <klib/llist.h>
#include <arch/x86_64/regs.h>

namespace xenon
{
    const int NCPU = 1;

    /* states
                                   +-(scheduled/descheduled)
       +---------+    +-------+  |  +---------+    .............
       | created |--->| ready |<--->| running |--->. completed .
       +---------+    +-------+     +---------+    .............
                          ^           IO | \        * not an event
                          |        event V  `---.
                          | IO done +----+-----+ \   +--------+
                          +---------| sleeping |  +->| zombie |
                                    +----------+     +--------+
    */
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

    /* task

       each task knows about its parent only, the controller knows
       the order

       +-----------+   +-----------+   +-----------+
       | process 1 |   | process 3 |   | process O |
       +-----------+   +-----------+   +-----------+
             ^               ^
             |               |
       +-----+-----+   +-----+-----+
       | process 2 |   | process M |
       +------------   +-----------+
             ^
             |
       +-----+-----+
       | process N |
       +-----------+
    */

    class task
    {
        pid_t      pid_;
        context    *context_;
        task       *parent_;
        size_t     stack_len_;
        size_t     code_len_;

        TASK_STATE state_;

    public:
        task(context *ctx);

        size_t     get_stack_len() const;
        size_t     get_code_len()  const;
        pid_t      get_pid()       const;
        task      *get_parent();
        TASK_STATE get_state()     const;

        void       set_state(TASK_STATE state);
    };

    class task_controller
    {
        using ready_queue = llist<task*>;

        ready_queue ready_queue_;
        task *running_;
        manager &memory_manager_;


    private:
        void switch_task();

    public:
        task_controller(manager &memory_manager);

        void init_multitasking();

        void create(context *ctx);
        pid_t fork();
        void exec();
        void signal();
        TASK_STATE state();
    };
}

#endif // TASK_H
