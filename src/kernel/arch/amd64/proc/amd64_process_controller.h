#ifndef AMD64_PROCESS_CONTROLLER_H
#define AMD64_PROCESS_CONTROLLER_H

#include <proc/process_controller.h>

namespace xenon
{
    class amd64_process_controller : public process_controller
    {
    public:
        amd64_process_controller();

    public:
        pid_t create_process(context *ctx,
                             paddr_t kstack_addr,
                             size_t kstack_size) final;
    };
}

#endif // AMD64_PROCESS_CONTROLLER_H
