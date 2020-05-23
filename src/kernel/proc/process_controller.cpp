#include "process_controller.h"

namespace xenon
{
    process_controller::process_controller(manager &memory_manager) :
        memory_manager_(memory_manager)
    {
    }

    void process_controller::switch_to(process *proc)
    {
        (void)proc;
        // 1. save current_ process context
        // 2. add current_ process into ready_queue_
        // 3. setup proc context
        // 4. run
    }
}
