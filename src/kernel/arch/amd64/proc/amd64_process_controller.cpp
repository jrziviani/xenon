#include "amd64_process_controller.h"
#include "amd64_process.h"

namespace xenon
{
    amd64_process_controller::amd64_process_controller(manager &memory_manager) :
        process_controller(memory_manager)
    {
    }

    void amd64_process_controller::create_process(context *ctx)
    {
        vaddr_t place = memory_manager_.kalloc(sizeof(process));
        process *proc = new (place) amd64_process(ctx);
        ready_queue_.push_back(proc);
        proc->set_state(PROC_STATE::READY);
    }
}
