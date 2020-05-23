#ifndef PROCESS_H
#define PROCESS_H

#include "context.h"

#include <config.h>
#include <arch/amd64/regs.h>

namespace xenon
{
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
    enum class PROC_STATE : uint8_t
    {
        CREATED,
        READY,
        RUNNING,
        SLEEPING,
        ZOMBIE,
    };

    inline const char *task_state_name(PROC_STATE state)
    {
        switch (state)
        {
            case PROC_STATE::CREATED:
                return "created";

            case PROC_STATE::READY:
                return "ready";

            case PROC_STATE::RUNNING:
                return "running";

            case PROC_STATE::SLEEPING:
                return "sleeping";

            case PROC_STATE::ZOMBIE:
                return "zombie";

            default:
                return "invalid state";
        }
    }

    class process
    {
        pid_t      pid_;
        context    *context_;
        process    *parent_;
        size_t     stack_len_;
        size_t     code_len_;

        PROC_STATE state_;

    public:
        virtual void switch_process(process *newp) = 0;

    public:
        process(context *ctx);
        virtual ~process() = default;

    public:
        size_t     get_stack_len() const;
        size_t     get_code_len()  const;
        pid_t      get_pid()       const;
        process    *get_parent();
        context    *get_context();
        PROC_STATE get_state()     const;

        void       set_state(PROC_STATE state);
    };
}

#endif // PROCESS_H
