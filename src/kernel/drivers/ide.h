#ifndef IDE_H
#define IDE_H

#include "block_device.h"

namespace xenon
{
    class ide : public block_device
    {
    public:
        virtual void on_disk_event()
        {
        }
    };
}

#endif // IDE_H
