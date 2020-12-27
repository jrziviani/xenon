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

        uintptr_t abar_tmp = PCI_VIRTUAL_ADDRESS + (info.bars[5] & ~0xfeb00000);
        auto abar = reinterpret_cast<ahci::hba_memory*>(abar_tmp);

        auto controller = new ahci::ahci_controller(abar);
        controller->sata_identify();
    }
}
