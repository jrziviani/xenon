#include "process.h"


/*
    User
    Space   +---Shell               Cat <-----+
 -----------|---------------------------------|--
    Kernel  | +-----------------------------+ |
    Space   | |  +--+      +--+      +--+   | |
            +-|->|  |----->|  |----->|  |---|-+
           [1]|  |  | [2]  |  | [3]  |  |   |[4]
              |  +--+      +--+      +--+   |
              |  kstack    kstack    kstack |
              |  shell     sched.    cat    |
              +-----------------------------+
[1] - save
[2] - switch_page
[3] - switch_page
[4] - restore
*/

namespace xenon
{
    pid_t next_pid = 0;

    process::process(context *ctx,
                     paddr_t kstack_addr,
                     size_t kstack_size) :
        pid_(next_pid++),
        context_(ctx),
        parent_(nullptr),
        kstack_addr_(kstack_addr),
        kstack_len_(kstack_size),
        ustack_addr_(0x0),
        ustack_len_(USTACK_SIZE),
        state_(PROC_STATE::CREATED)
    {
    }

    size_t process::get_kstack_len() const
    {
        return kstack_len_;
    }

    size_t process::get_ustack_len() const
    {
        return ustack_len_;
    }

    pid_t process::get_pid() const
    {
        return pid_;
    }

    process *process::get_parent()
    {
        return parent_;
    }

    context *process::get_context()
    {
        return context_;
    }

    PROC_STATE process::get_state() const
    {
        return state_;
    }

    void process::set_state(PROC_STATE state)
    {
        state_ = state;
    }
}
