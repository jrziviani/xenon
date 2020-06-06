#ifndef XENON_BASE_H
#define XENON_BASE_H

#include "klib/stdint.h"

namespace xenon
{
    class xenon_base
    {
    public:
        virtual ~xenon_base() = default;

        virtual void on_time()
        {
        }

        virtual void on_keyboard()
        {
        }

        virtual void on_disk_event()
        {
        }

    protected:
        xenon_base() = default;
        xenon_base(const xenon_base&) = delete;
    };
}

#endif // XENON_BASE_H
