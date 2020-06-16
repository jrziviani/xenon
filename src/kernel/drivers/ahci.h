#ifndef AHCI_H
#define AHCI_H

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
        struct hba_port
        {
            // - bit 31:10: 32-bit base physical addr for the
            //   command list for this port, used when fetching commands to
            //   execute. The struct pointed to by it is 1K, so this address
            //   must be 1K aligned (bits 9:0 zeroed). [RW]
            uint32_t comm_list_base_addr_lo;
            // - bit 31:0: upper 32-bit part. (0 if HBA doesn't support 64-bit)
            //   [RW].
            uint32_t comm_list_base_addr_hi;
            // - bit 31:8: 32-bit base physical address for received FISes.
            //   data is 256-byte lenght, so it must be 256-byte aligned (bits
            //   7:0 zeroes). [RW]
            uint32_t fis_base_addr_lo;
            // - bit 31:0: upper 32-bit part. (0 if HBA doesn't support 64-bit)
            //   [RW].
            uint32_t fis_base_addr_hi;
            // - bit 29 [Host Bus Fatal Error Status]: HBA encountered a host bus
            //   error that it cannot recover from. In PCI, indicates a target or
            //   master abort. [RWC]
            // - bit 28 [Host Bus Data Error Status]: HBA encountered a data error
            //   when reading from/writing to memory. [RWC]
            // - bit 27 [Interface Fatal Error Status]: HBA encountered error on
            //   SATA interface that caused the transfer to stop. [RWC]
            // - bit 26 [Interface Non-fatal Error Status]: HBA encountered error on
            //   SATA interface but was able to continue. [RWC] (section 6.1.2)
            // - bit 24 [Overflow Status]: HBA received more bytes that specified in
            //   the PRD table for command. [RWC]
            // - bit 4: [Unknown FIS interrupt]: 1 indicates an unknown FIS was received
            //   and copied into memory. [RO] TODO: revisit this field if needed
            // - bit 3: [Set Device Bits Interrupt]: 1 Set device bits FIS received and
            //   copied into memory [RWC].
            // - bit 2: [DMA Setup FIS interrupt]: 1 DMA setup FIS received and copied
            //   into memory [RWC].
            // - bit 1: [PIO Setup FIS Interrupt]: 1 PIO setup FIS received, copied into
            //   memory, and the data to that FIS has been transferred. [RWC].
            // - bit 0: [Set Device Bits Interrupt]: 1 D2H register FIS received and copied
            //   into memory [RWC].
            uint32_t interrupt_status;
            // - Enable/disable each of the status above, bit placement refers to exact same
            //   feature.
            uint32_t interrupt_enable;
            // - bit 31:28 [Interface Communication Control]: control power management
            //   states of the interface. [RW]
            // - bit 17 [Port Multiplier Attached]: 1 PM is attached to HBA to this port,
            //   [RO if HBA doesn't support PM - My case :-)].
            // - bit 15 [Command List Running]: 1, command list DMA engine for this port
            //   is running. AHCI state machine details on 5.3.2. [RO]
            // - bit 14 [FIS Receive Running]: 1, FIS receive DMA engine for the port is
            //   running. Details on 10.3.2. [RO].
            // - bit 12:8 [Current Command Slot]: field valid when field START (below) is 1
            //   and must be sset to the command slot value of the command that is currently
            //   being issued by HBA. [RO]
            //   bit 4 [FIS Receive Enable]: When set, HBA may post received FISes into the
            //   FIS receive area pointed to by fis_base_addr_*. When cleared, received
            //   FISes are not accepted by the HBA, except for the first D2H register FIS
            //   after the initialization sequence. fis_base_addr_* must be valid before
            //   setting this field. [RW]
            // - bit 0 [Start]: when set, HBA may process the command list. Whenever the bit
            //   changed from 0 to 1, HBA start processing the first command list at entry 0.
            //   Changing from 1 to 0, command_issue is cleared by HBA upon the HBA putting
            //   the controller into an idel state. Software must set it to 1 ONLY when FIS
            //   Receive Enable (above) has been set to one. Refer to 10.3.1. [RW]
            uint32_t command_status;
            uint32_t reserved_0;
            // - Copy specific fields of task file when FISes are received. Doesn't see
            //   important now.
            uint32_t task_file_data;
            // - Register contains the initial signature of the attached device when the
            //   first D2H Register FIS is received from that device - updated after reset seq.
            // - bit 31:24: LBA high register [RO]
            // - bit 23:16: LBA mid register [RO]
            // - bit 15:8: LBA low register [RO]
            // - bit 7:0: sector count register [RO]
            uint32_t signature;
            // - Register conveys the current stated of interface and host. When HBA transmits
            //   a COMRESET, this register is updated to its reset values. Page 29 for details.
            // - bit 11:8 [Interface Power Management]: indicates current interface state [RO].
            // - bit 7:4 [Current Interface Speed]: indicates the negotiated iface speed [RO].
            // - bit 3:0 [Device Detection]: indicates the interface dev detection and
            //   Phy state. [RO]
            uint32_t sata_status;
            // - bit 7:4 [Speed Allowed]: allowable speed of the iface. [RW]
            //   0h) no speed restrictions
            // - bit 3:0 [Device Detection Initialization]: [RW]
            //   0h) no device detection
            //   1h) perform iface comm. initialization sequence to establish communication.
            //       This is functionally equivalent to a hard reset. COMRESET is transmited
            //       on the interface. Software must leave this field set to 1 for a minimum
            //       of => 1 milisecond <= to ensure COMRESET is sent.
            // - This field may only be modified when command_status.Start == 0.
            uint32_t sata_control;
            // - bit 31:16 [Diagnosis]: page 31 of Serial ATA AHCI 1.3.1 [RWC]
            // - bit 15:0 [Error]: page 32. :-) [RWC]
            uint32_t sata_error;
            // - bit 31:0 [Device Status]: Each bit corresponds to the TAG and command slot
            //   of a native queued command. Bit 0 corresponds to TAG 0 and command slot 0.
            //   This field is set by SW prior to issuing a native queue command for a
            //   particular command slot. Prior to writing command_issue[TAG]=1, SW will
            //   set DS[TAG]=1 to indicate that a command with that TAG is outstanding.
            //   Should only be set with command_status.Start == 1. [RW1]
            uint32_t sata_active;
            // - bit 31:0: Each bit corresponds to a command slot, where bit 0 corresponds to
            //   command slot 0. Set by SW to indicate to HBA that a command has been built in
            //   memory for a command slot and may be sent to the device. [RW1]
            uint32_t command_issue;
            // - bit 15:0: indicates that asynchronous notifications events have accurred. [RWC]
            uint32_t sata_notification;
            // - Control/obtains status for Port Multiplier FIS-base switchin. Not used here.
            uint32_t fis_based_swtch_control;
            // - Power management control. Not used here.
            uint32_t device_sleep;
            uint32_t reserved_1[10];
            // - vendor specific.
            uint32_t vendor[4];
        };

        struct hba_ports
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

        struct hba_memory
        {
            // - bit 31: indicates whether the HBA supports 64-bit addressing
            //   which seems to be the only useful information for me. [RO]
            // - There's also the number of ports supported, but "ports_implemented"
            //   seems to be what I really want.
            uint32_t capabilities;
            // - bit 31 [AHCI Enable]: if 1, communication must be via AHCI
            //   mechanism. Irrelevant, I won't support legacy here. [RO]
            // - bit 1 [Interrupt Enable]: if 1, interrupts enabled, else
            //   all interrupts sources from all ports are disabled [RW]
            // - bit 0 [HBA Reset]: When set by SW cause internal reset of HBA,
            //   all ports must be re-initialized via COMRESET. [RW1]
            uint32_t global_host_control;
            // - bit 0:31 [Interrupt Pending Status]: if set, indicates that the
            //   corresponding port has an interrupt pending, software use to know
            //   which ports require service after interrupt. [RWC]
            uint32_t interrupt_status;
            // - bit 0:31 [Port Implemented]: If bit is 1, the corresponding port is
            //   available for software to use.
            uint32_t ports_implemented;
            // - too much for me :-)
            uint32_t version;
            // - Interesting feature. We can define a number of commands completed,
            //   or a timeout, that will raise an interrupt (which can be specified
            //   as well) and triggered. Too much for me.
            // - bit 0 [Enable]: I'll keep it zero.
            uint32_t com_completion_coalesc_control;
            // - If above struct has enable==1, this field will configure which
            //   ports will be coalesced. None for us.
            uint32_t com_completion_coalesc_ports;
            // - bit 31:16 [Offset]: message buffer offset in doublewords, from the
            //   beginning of the ABAR. [RO]
            // - bit 15:0 [Buffer size]: size of the transmit message buffer area in
            //   doublewords. If both transmit and receive buffers are supported, then
            //   the transmit buffer begins at ABAR[Offset * 4] and the receive buffer
            //   directly follows it. [RO]
            uint32_t enclosure_mng_location;
            // - Enclosure management is a mechanism by which the storage driver can
            //   monitor and control auxiliary services in a drive enclosure. The most
            //   common taks is controlling LEDs that show per-drive information (device
            //   activity, fault, etc).
            // - I don't think I'll need this. I'll need to revise it later.
            uint32_t enclosure_mng_control;
            // - Most related to power management and NVMP, not important for me.
            // - bit 0 [BIOS/OS handoff]: 1, HBA supports BIOS/OS handoff. 0, forget
            //   the next bios_os_handoff_control field.
            uint32_t host_capabilities_ex;
            // - bit 1 [OS Owned Semaphore]: OS sets to 1 to request ownership
            //   of the HBA controller [RW]
            uint32_t bios_os_handoff_control;
            // - Registers at offset 0xa0 to 0xff are vendor specific.
            //   So far we used 44 bytes = (11 fields * 4 bytes), in other words
            //   bios_os_handoff_control ends at offset 0x2c (44). It means that
            //   from 0x2c to 0xa0 we have just padding. 0xa0 - 0x2c - 0x74 (116)
            //   (116 / 4 = 29)
            uint32_t padding[29];
            // - Vendor: starts at 0xa0, ends at 0xff.
            //   0x100 - 0xa0 = 0x60 (96) - 96 / 4 = 24
            uint32_t vendor[24];
            // - Just to make sure: 44 + 116 + 96 = 256 (0x100).
            // - offset 0x100 + (ports_implemented bit position * 0x80)
            //                       each sizeof(hba_port) == 0x80
            // - we can have up to 32 ports, but defining an array of 1 is
            //   good enough because this structure will only be used to
            //   frame a piece of memory allocated and initialized for us by
            //   BIOS.
            hba_port ports[1];
        };

        struct hba_mem
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

        void probe(hba_memory *abar);
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
