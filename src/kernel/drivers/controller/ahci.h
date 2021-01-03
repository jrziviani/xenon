#ifndef AHCI_H
#define AHCI_H

#include "ahci_structs.h"

#include <klib/stdint.h>
#include <drivers/device_interface.h>
#include <drivers/bus/pci.h>

/*
                 +-----+
                 | CPU |
                 +--+--+
                    |
  +------+     +----+----+
  | SATA |--+  | Chipset |
  +------+  |  | +----+  |    [               ]
  +------+  +--->|ahci|  +---->[ system memory ]
  | SATA |--+  | +----+  |      [               ]
  +------+     +----+----+
                    |
<========o==========o=====[ PCI ]================o==>
         |                                       |
    +----+----+              +------+        +---+--+
    | Bridge  |              | SATA |----+-->| AHCI |
    +----+----+              +------+    |   +------+
         |                   +------+    |
    <=======[ PCI ]=====>    | SATA |----+
                     |       +------+
 +------+        +---+---+
 | SATA |   +--->|  AHCI |
 +---+--+   |    +-------+
     |------+
 +---+--+
 | SATA |
 +------+

 FIS - Frame Information Struct         HBA - Host Bus Adapter
 H2D - HBA to device                    D2H - Device to HBA
 PRD - Physical Region Descriptor       ABAR - AHCI Base Address
 RWC - Write 1 to CLEAR                 RW1 - Write 1 to SET

 https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/serial-ata-ahci-spec-rev1-3-1.pdf
*/

namespace ahci
{
    void detect(pci_info_t info);

    class ahci_controller : public device_interface
    {
        hba_memory *abar_;
        uint32_t total_cmd_slots_;
        SATA_ident identifier_;

    public:
        ahci_controller(hba_memory *abar);
        void sata_read(uint64_t start_lo, uint64_t start_hi, uint64_t count, uintptr_t buffer);
        void sata_write(uint64_t start_lo, uint64_t start_hi, uint64_t count, uintptr_t buffer);

    private:
        void initialize();
        void reset(hba_port *port);
        void start(hba_port *port);
        void stop(hba_port *port);
        auto sata_get_port();
        void sata_identify();
        void sata_execute(hba_port *port, uint16_t command,
                          uint64_t start_lo, uint64_t start_hi,
                          uint64_t count, uintptr_t buffer);
        bool wait_until(uint32_t reg, uint32_t port, bool cond, uint32_t time);
    };

    // frame information structures
    enum class FIS_TYPES
    {
        FIS_TYPE_REG_H2D    = 0x27, // Register FIS - host to device
        FIS_TYPE_REG_D2H    = 0x34, // Register FIS - device to host
        FIS_TYPE_DMA_ACT    = 0x39, // DMA activate FIS - device to host
        FIS_TYPE_DMA_SETUP  = 0x41, // DMA setup FIS - bidirectional
        FIS_TYPE_DATA       = 0x46, // Data FIS - bidirectional
        FIS_TYPE_BIST       = 0x58, // BIST activate FIS - bidirectional
        FIS_TYPE_PIO_SETUP  = 0x5F, // PIO setup FIS - device to host
        FIS_TYPE_DEV_BITS   = 0xA1, // Set device bits FIS - device to host
    };
}

#endif // AHCI
