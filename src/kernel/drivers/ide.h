#ifndef IDE_H
#define IDE_H

#include "block_device.h"

#include <config.h>
#include <drivers/bus/pci.h>
#include <drivers/controller/ahci.h>
#include <memory/manager.h>

class ide : public block_device
{
    public:
        virtual void on_disk_event()
        {
        }
};

#endif // IDE_H
