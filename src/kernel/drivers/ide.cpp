#include "ide.h"

namespace xenon
{
    void ide::detect_ide(pci_info_t info)
    {
        // not an ahci sata device, return
        if (info.klass != 0x1 || info.subclass != 0x6) {
            return;
        }

        manager::instance().mapio(info.bars[5], 0, 0);
        manager::instance().mapio(0xfebd4000, 0, 0);

        char *tmp = reinterpret_cast<char*>(PCI_VIRTUAL_ADDRESS + 0xd4000);

        uintptr_t abar_tmp = PCI_VIRTUAL_ADDRESS + (info.bars[5] & ~0xfeb00000);
        auto abar = reinterpret_cast<ahci::hba_memory*>(abar_tmp);
        ahci::probe(abar);

        auto controller = new ahci::ahci_controller(abar);
        controller->sata_identify();

    }

    int find_cmdslot(ahci::hba_port *port)
    {
        // sata_active and command_issue issue are used to retrieve the
        // all command slots active or not.
        uint32_t slots = (port->sata_active | port->command_issue);
        for (int i = 0; i < 32; i++) {
            if ((slots & 0x1) == 0) {
                return i;
            }

            slots >>= 1;
        }
        return -1;
    }

    void ide::read(ahci::hba_port *port)
    {
        port->interrupt_status = static_cast<uint32_t>(-1);
        int slot = find_cmdslot(port);
        if (slot == 1) {
            return;
        }

        auto cmd_header = reinterpret_cast<ahci::hba_cmd_header*>(port->comm_list_base_addr_hi << 31 |
                                                                  port->comm_list_base_addr_lo);
        cmd_header += slot;
        cmd_header->cfl = 4;
        cmd_header->w = 0;
    }
}
