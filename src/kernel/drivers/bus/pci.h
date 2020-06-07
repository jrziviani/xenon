#ifndef PCI_H
#define PCI_H

#include <klib/stdint.h>
#include <klib/llist.h>

namespace xenon
{
    struct pci_address_t
    {
        uint8_t bus;
        uint8_t slot;
        uint8_t function;

        pci_address_t(uint8_t b, uint8_t s, uint8_t f) :
            bus(b), slot(s), function(f)
        {
        }

    public:
        uint32_t get_address(uint8_t field) const
        {
            return static_cast<uint32_t>(0x80000000 | (bus << 16u)
                                                    | (slot << 11u)
                                                    | (function << 8u)
                                                    | (field & 0xfc));
        }
    };

    struct pci_info_t
    {
        pci_address_t address;

        uint8_t klass;
        uint8_t subclass;
        uint16_t vendor;
        uint16_t device;
        uint32_t bars[6];
    };

    using pci_function_t = void (*)(const pci_address_t &addr);

    class pci
    {
        using pci_container_t = llist<pci_info_t>;
        pci_container_t devices_;

    public:
        virtual ~pci();

        void scan_hw();

    public:
        auto begin() const
        {
            return devices_.begin();
        }

        auto end() const
        {
            return devices_.end();
        }

    protected:
        virtual uint8_t read8_field(const pci_address_t &address, uint32_t field) const = 0;
        virtual uint16_t read16_field(const pci_address_t &address, uint32_t field) const = 0;
        virtual uint32_t read32_field(const pci_address_t &address, uint32_t field) const = 0;

        virtual void write8_field(const pci_address_t &address, uint32_t field, uint8_t value) const = 0;
        virtual void write16_field(const pci_address_t &address, uint32_t field, uint16_t value) const = 0;
        virtual void write32_field(const pci_address_t &address, uint32_t field, uint32_t value) const = 0;

    private:
        static uint32_t device(int bus, int slot, int func)
        {
            return static_cast<uint32_t>((bus << 16) | (slot << 8) | func);
        }

    private:
        void scan(pci_function_t cb);
        void scan_bus(pci_function_t cb, int type, uint8_t bus);
        void scan_slot(pci_function_t cb, int type, uint8_t bus, uint8_t slot);
        void scan_function(pci_function_t cb, int type, uint8_t bus, uint8_t slot, uint8_t function);

        uint16_t get_type(pci_address_t address) const;
    };
}

#endif // PCI_H
