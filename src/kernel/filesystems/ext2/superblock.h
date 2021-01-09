#ifndef EXT2_SUPERBLOCK_H
#define EXT2_SUPERBLOCK_H

#include <stdint.h>

namespace ext2fs
{
    struct superblock
    {
        uint32_t inodes_total;
        uint32_t blocks_total;
        uint32_t superuser_blocks;
        uint32_t free_blocks;
        uint32_t free_inodes;
        uint32_t superblock_id;
        uint32_t block_size;            // log₂(block_size) - 10
        uint32_t fragment_size;         // log₂(fragment_size) - 10
        uint32_t blocks_per_group;
        uint32_t fragments_per_group;
        uint32_t inodes_per_group;
        uint32_t last_mount_time;
        uint32_t last_written_time;
        uint16_t mounts_count_after_fsck;
        uint16_t mounts_allowed_fsck;
        uint16_t signature;
        uint16_t state;
        uint16_t error_handling;
        uint16_t minor_version;
        uint32_t last_fsck_check;
        uint32_t interval_fsck_check;
        uint32_t creator_os_id;
        uint32_t major_version;
        uint16_t uid_reserved_blocks;
        uint16_t gid_reserved_blocks;

        void read();
        void write();
    };

    struct superblock_ext
    {
        uint32_t first_inode;
        uint16_t inode_size;
        uint16_t superblock_id;
        uint32_t optional_features;
        uint32_t required_features;
        uint32_t readonly_features;
        uint64_t filesystem_id[2];
        char     volume_name[16];
        char     last_mount_path[64];
        uint32_t compression_algorithm;
        uint8_t  file_preallocated_blocks;
        uint8_t  dir_preallocated_blocks;
        uint16_t unused1;
        uint64_t journal_id[2];
        uint32_t journal_inode;
        uint32_t journal_device;
        uint32_t orphan_list;
        uint8_t  unused2[788];               // padding to 1024
    };

    struct block_group_descriptor
    {
        uint32_t block_bitmap_address;
        uint32_t inode_bitmap_address;
        uint32_t inode_table_address;
        uint16_t free_blocks;
        uint16_t free_inodes;
        uint16_t directories;
        uint8_t  unused[14];
    };

    // log₂(number) - 10 == number << 1024

    enum class STATES : uint16_t
    {
        CLEAN = 1,
        ERRORS,
    };

    enum class ERROR_HANDLING : uint16_t
    {
        IGNORE = 1,
        REMOUNT,
        KERNEL_PANIC,
    };

    enum class OS_ID : uint32_t
    {
        LINUX = 0,
        HURD,
        MASIX,
        FREEBSD,
        BSD_LITE,
    };

    enum class OPTIONAL_FEATURES : uint32_t
    {
        PREALLOCATED_BLOCKS     = 0x0001,
        AFS_SERVER              = 0x0002,
        JOURNAL                 = 0x0004, // ext3
        EXTENDED_ATTRS          = 0x0008,
        LARGE_PARTITIONS        = 0x0010,
        HASH_INDEX              = 0x0020,
    };

    enum class REQUIRED_FEATURES : uint32_t
    {
        COMPRESSION             = 0x0001,
        DIRECTORY_TYPE          = 0x0002,
        REPLAY_JOURNAL          = 0x0004,
        JOURNAL_DEVICE          = 0x0008,
    };

    enum class READONLY_FEATURES : uint32_t
    {
        SPARSE_SUPERBLOCKS      = 0x0001,
        FILE_SIZE_64BIT         = 0x0002,
        BINARY_TREE_DIRECTORY   = 0x0004,
    };
}

#endif // EXT2_SUPERBLOCK_H
