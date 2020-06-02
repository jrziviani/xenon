#ifndef AMD64_PCI_H
#define AMD64_PCI_H

#include <drivers/bus/pci.h>

namespace xenon
{
    class amd64_pci : public pci
    {
    public:
        uint8_t read8_field(const pci_address_t &address, uint32_t field) const final;
        uint16_t read16_field(const pci_address_t &address, uint32_t field) const final;
        uint32_t read32_field(const pci_address_t &address, uint32_t field) const final;

        void write8_field(const pci_address_t &address, uint32_t field, uint8_t value) const final;
        void write16_field(const pci_address_t &address, uint32_t field, uint16_t value) const final;
        void write32_field(const pci_address_t &address, uint32_t field, uint32_t value) const final;
    };
}

#endif // AMD64_PCI_H
