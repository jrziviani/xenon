#ifndef PROCESS_H
#define PROCESS_H

#include "pagetable.h"

#include <config.h>
#include <arch/x86_64/regs.h>

namespace xenon
{
    enum class process_state : uint8_t
    {
        CREATED,
        READY,
        RUNNING,
        SLEEPING,
        ZOMBIE,
    };

    inline const char *process_state_name(process_state state)
    {
        switch (state)
        {
            case process_state::CREATED:
                return "created";

            case process_state::READY:
                return "ready";

            case process_state::RUNNING:
                return "running";

            case process_state::SLEEPING:
                return "sleeping";

            case process_state::ZOMBIE:
                return "zombie";

            default:
                return "invalid state";
        }
    }

    class process
    {
        process_state   state_;
        pid_t           pid_;
        pml4_t          *dir_;
        process         *parent_;
        context         *context_;
        char            name_[16];

    public:
        process();

        pid_t fork();
        int kill();
        void create();
        void wait();
        void sleep();
        void wakeup();
        void scheduler();
        void schedule();
        void exit();
    };

    class cpu
    {
        process         *process_;
        bool            started_;
        char            name_[4];

    public:
        cpu();
    };

    cpu cpus[MAX_CPUS];
}

#endif // PROCESS_H
