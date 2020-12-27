#ifndef AHCI_H
#define AHCI_H

#include "ahci_structs.h"

#include <klib/stdint.h>

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

namespace xenon
{
    namespace ahci
    {
        class ahci_controller
        {
            hba_memory *abar_;
            uint32_t total_cmd_slots_;

        public:
            ahci_controller(hba_memory *abar);

            void initialize();
            void reset(hba_port *port);
            void start(hba_port *port);
            void stop(hba_port *port);
            void sata_read(hba_port *port, uint64_t start_lo, uint64_t start_hi, uint64_t count, uintptr_t buffer)
            {
                // 0x25 - Read DMA Ext
                sata_execute(port, 0x25, start_lo, start_hi, count, buffer);
            }

            void sata_write(hba_port *port, uint64_t start_lo, uint64_t start_hi, uint64_t count, uintptr_t buffer)
            {
                // 0x35 - Write DMA Ext
                sata_execute(port, 0x35, start_lo, start_hi, count, buffer);
            }

            void sata_identify();

        private:
            bool wait_until(uint32_t reg, uint32_t port, bool cond, uint32_t time);

            void sata_execute(hba_port *port, uint16_t command,
                              uint64_t start_lo, uint64_t start_hi,
                              uint64_t count, uintptr_t buffer);
        };

        struct hba_prdt_entry
        {
            uint32_t dba; // Data base address
            uint32_t dbau;  // Data base address upper 32 bits
            uint32_t rsv0;  // Reserved

            // DW3
            uint32_t dbc:22;  // Byte count, 4M max
            uint32_t rsv1:9;  // Reserved
            uint32_t i:1;  // Interrupt on completion
        };

        struct hba_cmd_tbl
        {
            // 0x00
            uint8_t  cfis[64]; // Command FIS

            // 0x40
            uint8_t  acmd[16]; // ATAPI command, 12 or 16 bytes

            // 0x50
            uint8_t  rsv[48]; // Reserved

            // 0x80
            hba_prdt_entry prdt_entry[1]; // Physical region descriptor table entries, 0 ~ 65535
        };

        struct host_to_device
        {
            // DWORD 0
            uint8_t  fis_type;  // FIS_TYPE_REG_H2D
            uint8_t  pmport:4;  // Port multiplier
            uint8_t  rsv0:3;    // Reserved
            uint8_t  c:1;       // 1: Command, 0: Control
            uint8_t  command;   // Command register
            uint8_t  featurel;  // Feature register, 7:0

            // DWORD 1
            uint8_t  lba0;      // LBA low register, 7:0
            uint8_t  lba1;      // LBA mid register, 15:8
            uint8_t  lba2;      // LBA high register, 23:16
            uint8_t  device;    // Device register

            // DWORD 2
            uint8_t  lba3;      // LBA register, 31:24
            uint8_t  lba4;      // LBA register, 39:32
            uint8_t  lba5;      // LBA register, 47:40
            uint8_t  featureh;  // Feature register, 15:8

            // DWORD 3
            uint8_t  countl;    // Count register, 7:0
            uint8_t  counth;    // Count register, 15:8
            uint8_t  icc;       // Isochronous command completion
            uint8_t  control;   // Control register

            // DWORD 4
            uint8_t  rsv1[4];   // Reserved
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
}

#endif // AHCI

/*

        struct device_to_host
        {
            // DWORD 0
            uint8_t  fis_type;  // FIS_TYPE_REG_D2H
            uint8_t  pmport:4;  // Port multiplier
            uint8_t  rsv0:2;    // Reserved
            uint8_t  i:1;       // Interrupt bit
            uint8_t  rsv1:1;    // Reserved
            uint8_t  status;    // Status register
            uint8_t  error;     // Error register

            // DWORD 1
            uint8_t  lba0;      // LBA low register, 7:0
            uint8_t  lba1;      // LBA mid register, 15:8
            uint8_t  lba2;      // LBA high register, 23:16
            uint8_t  device;    // Device register

            // DWORD 2
            uint8_t  lba3;      // LBA register, 31:24
            uint8_t  lba4;      // LBA register, 39:32
            uint8_t  lba5;      // LBA register, 47:40
            uint8_t  rsv2;      // Reserved

            // DWORD 3
            uint8_t  countl;    // Count register, 7:0
            uint8_t  counth;    // Count register, 15:8
            uint8_t  rsv3[2];   // Reserved

            // DWORD 4
            uint8_t  rsv4[4];   // Reserved
        };

        struct data
        {
            // DWORD 0
            uint8_t  fis_type;  // FIS_TYPE_DATA
            uint8_t  pmport:4;  // Port multiplier
            uint8_t  rsv0:4;    // Reserved
            uint8_t  rsv1[2];   // Reserved

            // DWORD 1 ~ N
            uint32_t data[1];   // Payload
        };
*/
