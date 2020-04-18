#include "process.h"

namespace xenon
{
    cpu::cpu() :
        process_(nullptr),
        started_(false)
    {
    }

    pid_t next_pid = 0;

    void process::create()
    {
        pid_ = next_pid++;
        state_ = process_state::CREATED;
    }

    void process::exit()
    {
        
    }
}
