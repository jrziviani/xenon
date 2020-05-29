#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process_controller.h"

#include <klib/xenon_base.h>
#include <klib/logger.h>
#include <klib/timer.h>

namespace xenon
{
    class scheduler : public xenon_base
    {
        process_controller &process_controller_;

    public:
        scheduler(timer *t, process_controller &controller);
        ~scheduler();

        void on_time(uint64_t freq) final;
    };
}

#endif // SCHEDULER_H
