#include "ext2.h"
#include "superblock.h"

#include <config.h>
#include <klib/logger.h>
#include <klib/string.h>

#include <drivers/controller/ahci.h>

constexpr uint16_t EXT2_SIGNATURE = 0xef53;

ext2::ext2(block_device &device) :
    device_(device)
{
    /*
    auto superblock = device.read(1024, 1024);
    if (superblock.size() != 1024) {
        klib::logger::instance().log("EXT2: Error reading the superblock");
        return;
    }
    */
    static_assert(sizeof(ext2fs::superblock) == 1_KB, "superblock struct must be 1KB");
    static_assert(sizeof(ext2fs::block_group_descriptor) == 32, "block group descriptor must be 32 bytes");
    static_assert(sizeof(ext2fs::inode) == 128, "inode struct must be 128 bytes");

    //auto superblk = new ext2fs::superblock;
    //auto controller_hack = static_cast<ahci::ahci_controller*>(&device);

    // superblock is located at byte 1024 from the beginning of the volume and is exactly
    // 1024 bytes length. If disk uses 512-byte sectors, superblock begins at LBA 2 and
    // occupies sectors 2 and 3
     //controller_hack->sata_read(2, 32, reinterpret_cast<uintptr_t>(superblk) - KVIRTUAL_ADDRESS);
    auto buffer = device.read(2, 1024);
    auto superblk = reinterpret_cast<ext2fs::superblock*>(buffer.get());

    // make sure the device has a ext2 filesystem
    if (superblk->signature != EXT2_SIGNATURE) {
        klib::logger::instance().log("No ext2 filesystem found in this device");
        return;
    }

    // ext2 major version >= 1 has extended superblock fields, but I won't bother
    // to support old ext2...
    if (superblk->major_version < 1) {
        klib::logger::instance().log("Too old ext2 filesystem");
        return;
    }

     auto block_size = 1024 << superblk->block_size;
     auto inodes_per_block = block_size / sizeof(ext2fs::inode);
     auto sectors_per_block = block_size / 512;
     /*
     klib::logger::instance().log("Block size: %d, Volume size: %d, Block groups: %d",
                                  block_size,
                                  block_size * superblk->blocks_total,
                                  superblk->blocks_total / superblk->blocks_per_group);
     klib::logger::instance().log("ID: %x", superblk->extended.filesystem_id[1] << 32 |
                                            superblk->extended.filesystem_id[0]);
    */
     //ad83d446-e81b-4768-8dde-0d163286cda2

     // block descriptor table is located after the superblock, so after the first 1024
     // bytes
     auto block_index = (2 - 1) / superblk->inodes_per_group;
     auto inode_index = (2 - 1) % superblk->inodes_per_group;
     auto start_lba = (block_size == 1_KB) ? 4 : 3;
     auto buffer1 = device.read(start_lba + block_index, sizeof(ext2fs::block_group_descriptor));
     auto block_descriptor = reinterpret_cast<ext2fs::block_group_descriptor*>(buffer1.get());

     // set the block_descriptor to the required index
     //block_descriptor += block_index * sizeof(ext2fs::block_group_descriptor);

     // the group that the inode belongs was found, now we need to to inode itself, now
     // we're going to locate the inode itself
     auto block = (inode_index * sizeof(ext2fs::inode)) / block_size;
     auto buffer2 = device.read(block_descriptor->inode_table_address * sectors_per_block,
                                sizeof(ext2fs::inode) * sizeof(ext2fs::inode) / 512);
     auto inode = reinterpret_cast<ext2fs::inode*>(buffer2.get());

     klib::logger::instance().log("%d", (inode[inode_index].type_permission & 0xf000) == 0x4000);

     for (int i = 0; i < 12; i++) {
        auto b = inode[inode_index].direct_block_pointer[i];
        if (b == 0) {
            break;
        }

        // ext2 dir names cannot span to multiple blocks, so read the whole block
        auto buffer3 = device.read(b * sectors_per_block, block_size);
        auto dir = reinterpret_cast<ext2fs::directory_entry*>(buffer3.get());

        auto counter = block_size;
        while (counter > 0 && dir->inode != 0) {
            auto name = klib::string(&dir->name, dir->name_len);
            klib::logger::instance().log("%s", name.c_str());
            counter -= dir->size;
            dir = (ext2fs::directory_entry*)((uintptr_t)dir + dir->size);
        }
     }
}

void ext2::list_directory(const ext2fs::inode &inode)
{

}
