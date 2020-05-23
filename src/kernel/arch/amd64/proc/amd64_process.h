#ifndef AMD64_PROCESS_H
#define AMD64_PROCESS_H

#include "amd64_context.h"

#include <proc/process.h>

namespace xenon
{
    class amd64_process : public process
    {
    public:
        amd64_process(context *context);

    public:
        void switch_process(process *newp) final;
    };
}

#endif // AMD64_PROCESS_H
