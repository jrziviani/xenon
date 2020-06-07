#ifndef AHCI_H
#define AHCI_H

#include <klib/stdint.h>

namespace xenon
{
    namespace ahci
    {
        volatile struct hba_port
        {
            uint32_t clb;       // 0x00, command list base address, 1K-byte aligned
            uint32_t clbu;      // 0x04, command list base address upper 32 bits
            uint32_t fb;        // 0x08, FIS base address, 256-byte aligned
            uint32_t fbu;       // 0x0C, FIS base address upper 32 bits
            uint32_t is;        // 0x10, interrupt status
            uint32_t ie;        // 0x14, interrupt enable
            uint32_t cmd;       // 0x18, command and status
            uint32_t rsv0;      // 0x1C, Reserved
            uint32_t tfd;       // 0x20, task file data
            uint32_t sig;       // 0x24, signature
            uint32_t ssts;      // 0x28, SATA status (SCR0:SStatus)
            uint32_t sctl;      // 0x2C, SATA control (SCR2:SControl)
            uint32_t serr;      // 0x30, SATA error (SCR1:SError)
            uint32_t sact;      // 0x34, SATA active (SCR3:SActive)
            uint32_t ci;        // 0x38, command issue
            uint32_t sntf;      // 0x3C, SATA notification (SCR4:SNotification)
            uint32_t fbs;       // 0x40, FIS-based switch control
            uint32_t rsv1[11];  // 0x44 ~ 0x6F, Reserved
            uint32_t vendor[4]; // 0x70 ~ 0x7F, vendor specific
        };

        volatile struct hba_mem
        {
            // 0x00 - 0x2B, Generic Host Control
            uint32_t cap;       // 0x00, Host capability
            uint32_t ghc;       // 0x04, Global host control
            uint32_t is;        // 0x08, Interrupt status
            uint32_t pi;        // 0x0C, Port implemented
            uint32_t vs;        // 0x10, Version
            uint32_t ccc_ctl;   // 0x14, Command completion coalescing control
            uint32_t ccc_pts;   // 0x18, Command completion coalescing ports
            uint32_t em_loc;    // 0x1C, Enclosure management location
            uint32_t em_ctl;    // 0x20, Enclosure management control
            uint32_t cap2;      // 0x24, Host capabilities extended
            uint32_t bohc;      // 0x28, BIOS/OS handoff control and status

            // 0x2C - 0x9F, Reserved
            uint8_t  rsv[0xA0-0x2C];

            // 0xA0 - 0xFF, Vendor specific registers
            uint8_t  vendor[0x100-0xA0];

            // 0x100 - 0x10FF, Port control registers
            hba_port ports[1];  // 1 ~ 32
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
