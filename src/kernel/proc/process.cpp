#include "process.h"

#include <klib/string.h>
#include <memory/manager.h>

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

pid_t next_pid = 0;

process::process(uintptr_t kstack_addr,
                 size_t kstack_size,
                 uintptr_t nip,
                 const char *name) :
    pid_(next_pid++),
    kstack_addr_(kstack_addr),
    kstack_len_(kstack_size)
{
    (void)nip;
    auto len = klib::strlen(name);
    klib::strncpy(name_, name, (len >= 64) ? 64 : len);

    // create a new stack for the process if no
    // address for stack was passed
    if (kstack_addr == 0) {
        char *stack = new char[kstack_size];
        kstack_addr_ = ptr_from(stack) + kstack_len_;
    }

    // create a new address space for this process
    top_dir_ = ptr_from(manager::instance().create_address_space());
}

process::~process()
{
    char *stack = ptr_to<char*>(kstack_addr_ - kstack_len_);
    delete[] stack;

    paddr_t top = ptr_to<paddr_t>(top_dir_);
    manager::instance().destroy_address_space(top);
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

PROC_STATE process::get_state() const
{
    return state_;
}

void process::set_state(PROC_STATE state)
{
    state_ = state;
}

void process::set_parent(process *parent)
{
    parent_ = parent;
}
