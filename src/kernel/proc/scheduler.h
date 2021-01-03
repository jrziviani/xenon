#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process_controller.h"

#include <klib/xenon_base.h>
#include <klib/logger.h>
#include <drivers/irq_handler.h>

class scheduler : public xenon_base
{
    process_controller &process_controller_;

public:
    scheduler(process_controller &controller);
    ~scheduler();

    void on_time() final;
};

#endif // SCHEDULER_H
