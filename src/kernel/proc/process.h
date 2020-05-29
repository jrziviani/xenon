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
    protected:
        pid_t       pid_;
        process     *parent_;
        uintptr_t   kstack_addr_;
        size_t      kstack_len_;
        uintptr_t   ustack_addr_;
        size_t      ustack_len_;
        char        name_[64];
        paddr_t     top_dir_;
        PROC_STATE  state_;

    public:
        process(uintptr_t kstack_addr,
                size_t kstack_size,
                uintptr_t nip,
                const char *name);

        virtual ~process();

    public:
        size_t     get_kstack_len() const;
        size_t     get_ustack_len() const;
        pid_t      get_pid()        const;
        PROC_STATE get_state()      const;
        process    *get_parent();

        void       set_state(PROC_STATE state);
        void       set_parent(process *parent);

    public:
        virtual context *get_context()                 = 0;
        virtual void switch_process(process *newp)     = 0;
        virtual void set_program_address(vaddr_t addr) = 0;
    };
}

#endif // PROCESS_H
