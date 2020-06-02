#include "amd64_pci.h"

#include <arch/amd64/instructions.h>

constexpr unsigned PCI_ADDRESS_PORT = 0xcf8;
constexpr unsigned PCI_VALUE_PORT   = 0xcfc;

namespace xenon
{
    uint8_t amd64_pci::read8_field(const pci_address_t &address, uint32_t field) const
    {
        outl(PCI_ADDRESS_PORT, address.get_address(field));
        return inb(PCI_VALUE_PORT + (field & 0x3));
    }

    uint16_t amd64_pci::read16_field(const pci_address_t &address, uint32_t field) const
    {
        outl(PCI_ADDRESS_PORT, address.get_address(field));
        return inw(PCI_VALUE_PORT + (field & 0x2));
    }

    uint32_t amd64_pci::read32_field(const pci_address_t &address, uint32_t field) const
    {
        outl(PCI_ADDRESS_PORT, address.get_address(field));
        return inl(PCI_VALUE_PORT);
    }

    void amd64_pci::write8_field(const pci_address_t &address, uint32_t field, uint8_t value) const
    {
        outl(PCI_ADDRESS_PORT, address.get_address(field));
        outb(PCI_VALUE_PORT, value);
    }

    void amd64_pci::write16_field(const pci_address_t &address, uint32_t field, uint16_t value) const
    {
        outl(PCI_ADDRESS_PORT, address.get_address(field));
        outw(PCI_VALUE_PORT, value);
    }

    void amd64_pci::write32_field(const pci_address_t &address, uint32_t field, uint32_t value) const
    {
        outl(PCI_ADDRESS_PORT, address.get_address(field));
        outl(PCI_VALUE_PORT, value);
    }
}
