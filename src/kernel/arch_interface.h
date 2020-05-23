#ifndef ARCH_INTERFACE_H
#define ARCH_INTERFACE_H

#include "timer.h"
#include <proc/context.h>
#include <proc/process_controller.h>
#include <memory/paging.h>
#include <memory/manager.h>

#include <klib/new.h>

namespace xenon
{
    /*
     * Arch Interface
     *
     * Provides a common interface to portable code access the
     * architecture dependent code without coupling.
     *
     *     | | | | | | |     In a perfect world, support new architecture
     *    +-------------+    is the matter of implementing this interface
     *   -|             |-   which basically initializes and deals with
     *   -|             |-    * interruptions and exceptions
     *   -|             |-    * timers
     *   -|             |-    * paging
     *    +-------------+
     *     | | | | | | |
     */
    class arch_interface
    {
    protected:
        timer *timer_;
        paging *paging_;
        context *context_;
        process_controller *process_controller_;

    public:
        arch_interface() :
            timer_(nullptr),
            paging_(nullptr),
            context_(nullptr),
            process_controller_(nullptr)
        {
        }

        virtual ~arch_interface()
        {
        }

        paging *get_paging()
        {
            return paging_;
        }

        timer *get_timer()
        {
            return timer_;
        }

        context *get_context()
        {
            return context_;
        }

        process_controller *get_process_controller()
        {
            return process_controller_;
        }

    public:
        virtual int init_timer()                = 0;
        virtual int init_paging()               = 0;
        virtual int init_interrupts()           = 0;

        virtual int create_context()            = 0;
        virtual int create_process_controller(manager &memory_manager) = 0;
    };
}

#endif // ARCH_INTERFACE_H
