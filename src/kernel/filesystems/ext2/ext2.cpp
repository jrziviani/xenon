#include "ext2.h"
#include "superblock.h"

#include <config.h>
#include <klib/logger.h>

#include <drivers/controller/ahci.h>

ext2::ext2(device_interface *device) :
    device_(device)
{
    /*
    auto superblock = device.read(1024, 1024);
    if (superblock.size() != 1024) {
        klib::logger::instance().log("EXT2: Error reading the superblock");
        return;
    }
    */

    auto superblk = new ext2fs::superblock;
    auto controller_hack = static_cast<ahci::ahci_controller*>(device);

    // superblock is located at byte 1024 from the beginning of the volume and is exactly
    // 1024 bytes length. If disk uses 512-byte sectors, superblock begins at LBA 2 and
    // occupies sectors 2 and 3
     controller_hack->sata_read(2, 0, 24, reinterpret_cast<uintptr_t>(superblk) - KVIRTUAL_ADDRESS);
}
