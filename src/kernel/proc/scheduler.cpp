#include "scheduler.h"

#include <klib/logger.h>

namespace xenon
{
    scheduler::scheduler(timer *t, process_controller &controller) :
        process_controller_(controller)
    {
        //t->register_me(this);
    }

    scheduler::~scheduler()
    {
    }

    void scheduler::on_time(uint64_t freq)
    {
        (void)freq;
        process_controller_.set_running_from_queue();
    }
}
