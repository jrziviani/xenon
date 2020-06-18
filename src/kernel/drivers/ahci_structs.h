#ifndef AHCI_STRUCTS_H
#define AHCI_STRUCTS_H

#include <klib/stdint.h>

namespace xenon
{
    namespace ahci
    {
        constexpr size_t COMMAND_LIST_ENTRY_COUNT = 32;
        constexpr size_t PRD_TABLE_ENTRY_COUNT = 128;

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

        struct hba_memory
        {
            // - bit 31: indicates whether the HBA supports 64-bit addressing
            //   which seems to be the only useful information for me. [RO]
            // - bit 12:8 [Number of Command Slots]: number of command slots per
            //   port supported by this HBA. Min:1, Max: 32.
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

        struct command_list_entry
        {
            union {
                struct {
                    uint16_t command_fis_length : 5;
                    uint16_t atapi              : 1;
                    uint16_t write              : 1;
                    uint16_t prefetchable       : 1;
                    uint16_t reset              : 1;
                    uint16_t bist               : 1;
                    uint16_t clear_busy_ROK     : 1;
                    uint16_t reserved_0         : 1;
                    uint16_t port_multiplier    : 4;
                    uint16_t prdt_length;
                };
                uint32_t description_information;
            };
            // - bit 31:0 [Physical Region Descriptor Byte Count]: indicates the current
            //   byte count that has transferred on device writes (mem to device) or dev
            //   reads (dev to memory).
            uint32_t physical_reg_desc_byte_count;
            // - bit 31:7 [Command Table Descriptor Base Address]: indicates 32-bit physical
            //   address of the command table. Must be aligned 128-byte cache line (bits 6:0) = 0
            uint32_t command_table_base_addr_lo;
            // - bit 31:0 [Command Table Descriptor Base Address HI]: the upper 32-bits of
            //   the command table base.
            uint32_t command_table_base_addr_hi;
            uint32_t reserved_1[4];
        };

        struct command_table
        {
            // - Command FIS: SW constructed FIS. For data transfer ops, this is the H2D
            //   register FIS format as specified in SATA rev. 2.6. HBA sets
            //   hba_port.task_file_data.Busy and sends this struct to attached port.
            //   Valid CFIS lengths are 2 to 16 dwords and must be dwords granularity.
            uint32_t command_cfis[16];
            // - Atapi Command: 12 or 16 bytes length (16 here) that contains ATAPI command
            //   to transmit if the A bit set in the command header.
            uint32_t atapi_command[4];
            uint32_t reserved[4];
        };

        struct physical_reg_desc_table
        {
            // - bit 31:1 [Data Base Address]: indicates 32-bit physical address of the
            //   data block, which is word aligned, bit 0 must be clear.
            uint32_t data_base_address_lo;
            // - bit 31:0 [Data Base Address Hi]: upper 32-bit physica address of the
            //   data block
            uint32_t data_base_address_hi;
            uint32_t reserved_0;
            // - bit 31 [Interrupt on Completion]: 1 indicates HW should assert an interrupt
            //   when data block for this entry has transferred. Which means no data in HBA HW
            //   Data may be still in flight to memory (disk reads) or at the device (and R_OK
            //   of R_ERR has yet to be received). HBA shall set hba_port.interrupt_status.DPS
            //   after completing, and if enabled, generate the interrupt.
            // - bit 21:00 [Data Byte Count]: 0 based value indicates the length (in bytes) of
            //   the data block. Max 4MB may exist for any entry.
            //      - bit 0: must always be 1
            //      - value 1: indicates 2 bytes
            //      - value 3: indicates 4 bytes, etc...
            uint32_t description_information;
        };
    }
}

#endif // AHCI_STRUCTS_H
