#ifndef PROCESS_CONTROLLER_H
#define PROCESS_CONTROLLER_H

#include "process.h"

#include <klib/llist.h>

namespace xenon
{
    /* process controller

       each task knows about its parent only, the controller knows
       the order

       +-----------+   +-----------+   +-----------+  |   +----------+
       | process 1 |   | process 3 |   | process O |  |   | running_ |
       +-----------+   +-----------+   +-----------+  |   +-----+----+
             ^               ^                        |         |
             |               |                        |         |
       +-----+-----+   +-----+-----+                  |         |
       | process 2 |   | process M |<---------------------------+
       +------------   +-----------+                  |
             ^                                        |
             |                                        |
       +-----+-----+                                  |
       | process N |
       +-----------+
    */

    class process_controller
    {
    protected:
        using ready_queue = llist<process*>;

        ready_queue ready_queue_;
        process *running_;

    private:
        void switch_to(process *proc);

    public:
        process_controller();

        void set_running(pid_t pid);
        void set_running(process *proc);
        void set_running_from_queue();

        void user_init();

    public:
        virtual pid_t create_process(context *ctx) = 0;
    };
}

#endif // PROCESS_CONTROLLER_H
