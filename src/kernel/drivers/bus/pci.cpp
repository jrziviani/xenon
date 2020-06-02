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

        };

        scan(callback);
    }

    void pci::scan(pci_function_t cb)
    {
        if ((read8_field(pci_address_t(0, 0, 0), PCI_HEADER_TYPE) & 0x80) == 0) {
            scan_bus(cb, -1, 0);
            return;
        }

        for (uint8_t function = 0; function < 8; function++) {
            if (read16_field(pci_address_t(0, 0, function), PCI_VENDOR_ID) == PCI_NONE) {
                break;
            }
            scan_bus(cb, -1, function);
        }
    }

    void pci::scan_bus(pci_function_t cb, int type, uint8_t bus)
    {
        for (uint8_t slot = 0; slot < 32; slot++) {
            scan_slot(cb, type, bus, slot);
        }
    }

    void pci::scan_slot(pci_function_t cb, int type, uint8_t bus, uint8_t slot)
    {
        pci_address_t address(bus, slot, 0);
        if (read16_field(address, PCI_VENDOR_ID) == PCI_NONE) {
            return;
        }

        scan_function(cb, type, bus, slot, 0);
        if ((read8_field(address, PCI_HEADER_TYPE) & 0x80) == 0) {
            return;
        }

        for (uint8_t function = 1; function < 8; function++) {
            if (read16_field(pci_address_t(bus, slot, function), PCI_VENDOR_ID) != PCI_NONE) {
                scan_function(cb, type, bus, slot, function);
            }
        }
    }

    void pci::scan_function(pci_function_t cb, int type, uint8_t bus, uint8_t slot, uint8_t function)
    {
        pci_address_t address(bus, slot, function);
        if (type == -1 || type == get_type(address)) {
            logger::instance().log(" > PCI %d:%d:%d - 0x%x 0x%x", address.bus,
                                                                  address.slot,
                                                                  address.function,
                                                                  read16_field(address, PCI_VENDOR_ID),
                                                                  read16_field(address, PCI_DEVICE_ID));
        }

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
}
