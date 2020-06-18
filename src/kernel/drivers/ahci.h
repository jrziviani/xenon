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
        void probe(hba_memory *abar);

        class ahci_controller
        {
            hba_memory *abar_;

        public:
            ahci_controller(hba_memory *abar);

            void initialize();
            void reset();
            void start();

        private:
            void wait_until(uint32_t reg, uint32_t port, bool cond, uint32_t time);
        };

        struct hba_cmd_header
        {
            // DW0
            uint8_t  cfl:5;  // Command FIS length in DWORDS, 2 ~ 16
            uint8_t  a:1;  // ATAPI
            uint8_t  w:1;  // Write, 1: H2D, 0: D2H
            uint8_t  p:1;  // Prefetchable

            uint8_t  r:1;  // Reset
            uint8_t  b:1;  // BIST
            uint8_t  c:1;  // Clear busy upon R_OK
            uint8_t  rsv0:1;  // Reserved
            uint8_t  pmp:4;  // Port multiplier port

            uint16_t prdtl;  // Physical region descriptor table length in entries

            // DW1
            volatile
                uint32_t prdbc;  // Physical region descriptor byte count transferred

            // DW2, 3
            uint32_t ctba;  // Command table descriptor base address
            uint32_t ctbau;  // Command table descriptor base address upper 32 bits

            // DW4 - 7
            uint32_t rsv1[4]; // Reserved
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
    }
}

#endif // AHCI

/*
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
