#ifndef AHCI_H
#define AHCI_H

#include "ahci_structs.h"

#include <klib/stdint.h>
#include <klib/list.h>
#include <klib/cmemory.h>
#include <drivers/bus/pci.h>
#include <drivers/block_device.h>

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
    klib::unique_ptr<block_device> detect(pci_info_t info);

    enum class IPM : uint8_t
    {
        ACTIVE_STATE = 1,
    };

    enum class DET : uint8_t
    {
        GENERATION_3_RATE = 3,
    };

    enum class SIG : uint64_t
    {
        SATA            = 0x101,
        SATAPI          = 0xEB140101,
        ENCLOSURE       = 0xC33C0101,
        PORT_MULTIPLIER = 0x96690101,
        ERROR           = 0,
    };

    enum PORT
    {
        CMD_ST  = 1,
        CMD_FRE = 1 << 4,
        CMD_FR  = 1 << 14,
        CMD_CR  = 1 << 15,
    };

    enum GHC
    {
        AE = 1 << 31,
    };

    class ahci_controller : public block_device
    {
        hba_memory *abar_;
        uint32_t total_cmd_slots_;
        SATA_ident identifier_;
        klib::list<klib::unique_ptr<block_device>> devices_;

    public:
        ahci_controller(hba_memory *abar);
        klib::unique_ptr<char[]> read(size_t offset, size_t blocks) override final;

    private:
        void initialize();
        void reset(hba_port *port);
        void start(hba_port *port);
        void stop(hba_port *port);
        auto sata_get_port();
        void sata_identify();
        void sata_execute(hba_port *port, uint16_t command, size_t lba, uintptr_t buffer, size_t buffer_size);
        void sata_read(size_t lba, uintptr_t buffer, size_t buffer_size);
        void sata_write(size_t lba, uintptr_t buffer, size_t buffer_size);
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
