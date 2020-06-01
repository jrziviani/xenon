#ifndef PCI_H
#define PCI_H

#include <klib/stdint.h>

namespace xenon
{
    using pci_function_t = void (*)(uint32_t device,
                                    uint16_t vendor_id,
                                    uint16_t device_id,
                                    void *extra);
    class pci
    {
        uint32_t pci_isa_ = 0;
        uint8_t pci_remaps_[4] = { 0 };

    public:
        virtual ~pci();

    public:
        void remap();
        void scan(pci_function_t fn, int type, void *extra);
        void scan_hit(pci_function_t fn, uint32_t dev, void *extra);
        void scan_func(pci_function_t fn, int type, int bus, int slot, int func, void *extra);
        void scan_slot(pci_function_t fn, int type, int bus, int slot, void *extra);
        void scan_bus(pci_function_t fn, int type, int bus, void *extra);

        uint16_t find_type(uint32_t dev);

    public:
        virtual uint32_t read_field(uint32_t device, int field, int size)              = 0;
        virtual void write_field(uint32_t device, int field, int size, uint32_t value) = 0;

    public:
        static uint32_t device(int bus, int slot, int func)
        {
            return static_cast<uint32_t>((bus << 16) | (slot << 8) | func);
        }
    };
}

#endif // PCI_H
