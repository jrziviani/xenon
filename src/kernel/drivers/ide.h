#ifndef IDE_H
#define IDE_H

#include "block_device.h"

#include <config.h>
#include <drivers/bus/pci.h>
#include <drivers/controller/ahci.h>
#include <memory/manager.h>

namespace xenon
{
    class ide : public block_device
    {
    public:
        virtual void on_disk_event()
        {
        }

        static void detect_ide(pci_info_t info);
    };
}

#endif // IDE_H
