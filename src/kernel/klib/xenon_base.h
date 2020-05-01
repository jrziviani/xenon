#ifndef XENON_BASE_H
#define XENON_BASE_H

#include "klib/stdint.h"
#include "llist.h"

namespace xenon
{
    class xenon_base
    {
    public:
        virtual ~xenon_base() = 0;

        virtual void on_time(uint64_t /*freq*/)
        {
            // not implement in this abstract class
        }
    };

    inline xenon_base::~xenon_base()
    {
    }
}

#endif // XENON_BASE_H
