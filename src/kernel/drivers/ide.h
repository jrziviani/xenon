#ifndef IDE_H
#define IDE_H

#include "block_device.h"

#include <config.h>
#include <drivers/bus/pci.h>
#include <drivers/ahci.h>
#include <memory/manager.h>

namespace xenon
{
    class ide : public block_device
    {
    public:
        virtual void on_disk_event()
        {
        }

        void read(ahci::hba_port *port);

        static void detect_ide(pci_info_t info);
    };
}

#endif // IDE_H
