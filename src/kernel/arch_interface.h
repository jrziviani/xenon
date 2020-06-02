#ifndef ARCH_INTERFACE_H
#define ARCH_INTERFACE_H

#include "timer.h"
#include <proc/context.h>
#include <proc/process_controller.h>
#include <memory/paging.h>
#include <memory/manager.h>
#include <drivers/bus/pci.h>

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
        pci *pci_         = nullptr;
        timer *timer_     = nullptr;
        paging *paging_   = nullptr;
        context *context_ = nullptr;
        process_controller *process_controller_ = nullptr;

    public:
        arch_interface()
        {
        }

        virtual ~arch_interface()
        {
        }

        pci *get_pci()
        {
            return pci_;
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
        virtual int init_processes()            = 0;
        virtual int init_pci()                  = 0;

        virtual void cpu_halt()                 = 0;
    };
}

#endif // ARCH_INTERFACE_H
