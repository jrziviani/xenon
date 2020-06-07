#include "pci.h"

#include <klib/string.h>
#include <klib/logger.h>

constexpr unsigned PCI_VENDOR_ID     = 0x00;
constexpr unsigned PCI_DEVICE_ID     = 0x02;
constexpr unsigned PCI_SUBCLASS      = 0x0a;
constexpr unsigned PCI_CLASS         = 0x0b;
constexpr unsigned PCI_HEADER_TYPE   = 0x0e;

constexpr unsigned PCI_BAR_0         = 0x10;
constexpr unsigned PCI_BAR_1         = 0x14;
constexpr unsigned PCI_BAR_2         = 0x18;
constexpr unsigned PCI_BAR_3         = 0x1c;
constexpr unsigned PCI_BAR_4         = 0x20;
constexpr unsigned PCI_BAR_5         = 0x24;

constexpr unsigned PCI_SECONDARY_BUS = 0x19;
constexpr unsigned PCI_TYPE_BRIDGE   = 0x604;
constexpr unsigned PCI_NONE          = 0xffff;

namespace xenon
{
    pci::~pci()
    {
    }

    void pci::scan_hw()
    {
        auto callback = [](const pci_address_t &addr) {
            (void)addr;
        };

        scan(callback);
    }

    void pci::scan(pci_function_t cb)
    {
        // The 7th bit of PCI HEADER tells whether the device is a multifunction
        // or a single function device.
        // Checking if device 0:0:0 is multifunction means the system has
        // multiple PCI host controllers

        // the system has only one PCI host controller, scan the controller
        if ((read8_field(pci_address_t(0, 0, 0), PCI_HEADER_TYPE) & 0x80) == 0) {
            scan_bus(cb, -1, 0);
            return;
        }

        // the system has multiple PCI host controllers, scan each one of them in its
        // own bus
        for (uint8_t function = 0; function < 8; function++) {
            if (read16_field(pci_address_t(0, 0, function), PCI_VENDOR_ID) == PCI_NONE) {
                break;
            }
            scan_bus(cb, -1, function);
        }
    }

    void pci::scan_bus(pci_function_t cb, int type, uint8_t bus)
    {
        // there can be 32 slots in the bus, scan each of them
        for (uint8_t slot = 0; slot < 32; slot++) {
            scan_slot(cb, type, bus, slot);
        }
    }

    void pci::scan_slot(pci_function_t cb, int type, uint8_t bus, uint8_t slot)
    {
        pci_address_t address(bus, slot, 0);

        // if the vendor id of the main function is NONE, there's no device
        // connected here
        if (read16_field(address, PCI_VENDOR_ID) == PCI_NONE) {
            return;
        }

        // there's a device here, scan what device is that
        scan_function(cb, type, bus, slot, 0);

        // make sure it's not a multifunction device...
        if ((read8_field(address, PCI_HEADER_TYPE) & 0x80) == 0) {
            return;
        }

        // ...ops, it's a multifunction, so let's scan each one of
        // its eight possible devices (seven, the first was found above).
        for (uint8_t function = 1; function < 8; function++) {
            if (read16_field(pci_address_t(bus, slot, function), PCI_VENDOR_ID) != PCI_NONE) {
                scan_function(cb, type, bus, slot, function);
            }
        }
    }

    void pci::scan_function(pci_function_t cb, int type, uint8_t bus, uint8_t slot, uint8_t function)
    {
        pci_address_t address(bus, slot, function);

        // we found the device (which can be anything OR a CLASS/SUBCLASS
        // speficied in the type argument)
        if (type == -1 || type == get_type(address)) {

            pci_info_t info {
                address,
                read8_field(address, PCI_CLASS),
                read8_field(address, PCI_SUBCLASS),
                read16_field(address, PCI_VENDOR_ID),
                read16_field(address, PCI_DEVICE_ID),
                read32_field(address, PCI_BAR_0),
                read32_field(address, PCI_BAR_1),
                read32_field(address, PCI_BAR_2),
                read32_field(address, PCI_BAR_3),
                read32_field(address, PCI_BAR_4),
                read32_field(address, PCI_BAR_5)
            };
            devices_.push_back(info);

            logger::instance().log(" > PCI %d:%d:%d - 0x%x 0x%x - 0x%x 0x%x",
                                   info.address.bus,
                                   info.address.slot,
                                   info.address.function,
                                   info.vendor,
                                   info.device,
                                   info.klass,
                                   info.subclass);

            if (info.klass == 0x1 && info.subclass == 0x6) {
                logger::instance().log("0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
                        info.bars[0],
                        info.bars[1],
                        info.bars[2],
                        info.bars[3],
                        info.bars[4],
                        info.bars[5]
                        );
            }
        }

        // we found a bridge, it means we need to scan the bus it's
        // connected just like we before
        if (get_type(address) == PCI_TYPE_BRIDGE) {
            auto secondary_bus = read8_field(address, PCI_SECONDARY_BUS);
            scan_bus(cb, type, secondary_bus);
        }
    }

    uint16_t pci::get_type(pci_address_t address) const
    {
        return static_cast<uint16_t>((read8_field(address, PCI_CLASS) << 8u) |
                                      read8_field(address, PCI_SUBCLASS));
    }

    // https://wiki.osdev.org/PCI
    // Two 32-bit IO locations used: (1) 0xcf8 (config data) and
    //                               (2) 0xcfc (config addr)
    // CONFIG_ADDRESS: 32-bit register
    //            31         24    16       11          8                 0
    //      +-------+----------+-----+--------+----------+-----------------+
    //      |enable | reserved | bus | device | function | register offset |
    //      +-------+----------+-----+--------+----------+-----------------+
    //      Notes:
    //       - Register offset is the offset in the 256-byte configuration space,
    //         which all PCI devices (except host bridges) must provides.
    //       - All reads/writes must be 32-bit aligned, so CONFIG_ADDRESS will
    //         always end with 0b00 (part of reg offset).
    // uint16_t config_read(pci_address_t address, uint8_t offset)
    // {
    //     constexpr unsigned CONFIG_ADDRESS = 0xcf8;
    //     constexpr unsigned CONFIG_DATA    = 0xcfc;
    //
    //     auto dev_address = (1 << 31) |
    //                        (address.bus << 16) |
    //                        (address.slot << 11) |
    //                        (address.function << 8) |
    //                        (offset & 0xfc);
    //
    //     outl(CONFIG_ADDRESS, dev_address);
    //     return static_cast<uint16_t>(inl(CONFIG_DATA) >> ((offset & 2) * 8) & 0xffff);
    // }
}
