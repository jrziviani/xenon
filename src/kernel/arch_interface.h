#ifndef ARCH_INTERFACE_H
#define ARCH_INTERFACE_H

#include "memory/context.h"
#include "memory/paging.h"
#include "timer.h"

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
        timer   *timer_;
        paging  *paging_;
        context *context_;

    public:
        arch_interface() :
            timer_(nullptr),
            paging_(nullptr),
            context_(nullptr)
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

    public:
        virtual int init_interrupts() = 0;
        virtual int init_paging()     = 0;
        virtual int init_timer()      = 0;
        virtual int create_context()  = 0;
    };
}

#endif // ARCH_INTERFACE_H
