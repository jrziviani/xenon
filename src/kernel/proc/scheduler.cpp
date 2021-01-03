#include "scheduler.h"

#include <klib/logger.h>

scheduler::scheduler(process_controller &controller) :
    process_controller_(controller)
{
}

scheduler::~scheduler()
{
}

void scheduler::on_time()
{
    process_controller_.set_running_from_queue();
}
