#include "pci.h"

#include <klib/string.h>

constexpr unsigned PCI_VENDOR_ID     = 0x00;
constexpr unsigned PCI_DEVICE_ID     = 0x02;
constexpr unsigned PCI_SUBCLASS      = 0x0a;
constexpr unsigned PCI_CLASS         = 0x0b;
constexpr unsigned PCI_HEADER_TYPE   = 0x0e;
constexpr unsigned PCI_SECONDARY_BUS = 0x19;
constexpr unsigned PCI_TYPE_BRIDGE   = 0x604;
constexpr unsigned PCI_NONE          = 0xffff;

namespace xenon
{
    pci::~pci()
    {
    }

    void pci::remap()
    {
        auto find_isa_bridge = [](uint32_t device,
                                  uint16_t vendor_id,
                                  uint16_t device_id,
                                  void *extra) {
            if (vendor_id == 0x8086 && (device_id == 0x7000 || device_id == 0x7110)) {
                *static_cast<uint32_t*>(extra) = device;
            }
        };

        scan(find_isa_bridge, -1, &pci_isa_);
        if (pci_isa_ == 0) {
            return;
        }

        for (int i = 0; i < 4; i++) {
            pci_remaps_[i] = read_field(pci_isa_, 0x60 + i, i);
            if (pci_remaps_[i] == 0x80) {
                pci_remaps_[i] = 10 + (i % 1);
            }
        }
        uint32_t out = 0;
        memcpy(&out, &pci_remaps_, 4);
        write_field(pci_isa_, 0x60, 4, out);
    }

    void pci::scan(pci_function_t fn, int type, void *extra)
    {
        if ((read_field(0, PCI_HEADER_TYPE, 1) & 0x80) == 0) {
            scan_bus(fn, type, 0, extra);
            return;
        }

        for (int func = 0; func < 8; func++) {
            auto dev = device(0, 0, func);
            if (read_field(dev, PCI_VENDOR_ID, 2) != PCI_NONE) {
                scan_bus(fn, type, func, extra);
            }

            break;
        }
    }

    uint16_t pci::find_type(uint32_t dev)
    {
        auto ret = (read_field(dev, PCI_CLASS, 1) << 8) | read_field(dev, PCI_SUBCLASS, 1);
        return ret;
    }

    void pci::scan_hit(pci_function_t fn, uint32_t dev, void *extra)
    {
        auto vendor = static_cast<int>(read_field(dev, PCI_VENDOR_ID, 2));
        auto dev_id = static_cast<int>(read_field(dev, PCI_DEVICE_ID, 2));

        fn(dev, vendor, dev_id, extra);
    }

    void pci::scan_func(pci_function_t fn, int type, int bus, int slot, int func, void *extra)
    {
        auto dev = device(bus, slot, func);
        if (type == -1 || type == find_type(dev)) {
            scan_hit(fn, dev, extra);
        }

        if (find_type(dev) == PCI_TYPE_BRIDGE) {
            scan_bus(fn, type, read_field(dev, PCI_SECONDARY_BUS, 1), extra);
        }
    }

    void pci::scan_slot(pci_function_t fn, int type, int bus, int slot, void *extra)
    {
        auto dev = device(bus, slot, 0);
        if (read_field(dev, PCI_VENDOR_ID, 2) == PCI_NONE) {
            return;
        }

        scan_func(fn, type, bus, slot, 0, extra);
        if (!read_field(dev, PCI_HEADER_TYPE, 1)) {
            return;
        }

        for (int func = 1; func < 8; func++) {
            dev = device(bus, slot, func);
            if (read_field(dev, PCI_VENDOR_ID, 2) != PCI_NONE) {
                scan_func(fn, type, bus, slot, func, extra);
            }
        }
    }

    void pci::scan_bus(pci_function_t fn, int type, int bus, void *extra)
    {
        for (int slot = 0; slot < 32; slot++) {
            scan_slot(fn, type, bus, slot, extra);
        }
    }
}
