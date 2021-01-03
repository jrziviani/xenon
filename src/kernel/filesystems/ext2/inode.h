#ifndef EXT2_INODE_H
#define EXT2_INODE_H

#include <stdint.h>

namespace ext2
{
    struct inode
    {
        uint16_t type_permission;
        uint16_t userid;
        uint32_t size_lo;
        uint32_t last_access_time;
        uint32_t creation_time;
        uint32_t last_modification_time;
        uint32_t deletion_time;
        uint16_t groupid;
        uint16_t hardlinks_count;
        uint32_t sectors_count;
        uint32_t flags;
        uint32_t os_specific1;
        uint32_t direct_block_pointer[12];
        uint32_t singly_indirect_block_pointer;
        uint32_t doubly_indirect_block_pointer;
        uint32_t triply_indirect_block_pointer;
        uint32_t generation_number;
        uint32_t file_acl;
        union {
            uint32_t size_hi;
            uint32_t dir_acl;
        };
        uint32_t fragment_block_address;
        uint8_t  os_specific2[12];

        void read();
        void write();
    };

    struct directory_entry
    {
        uint32_t inode;
        uint16_t size;           // total size of this entry (including subfields)
        uint8_t  name_len;       // least-significant 8 bits
        uint8_t  type_indicator; // basically most-significant name len
        char*    name;           // directory name

        void read();
        void list();
    };

    enum class TYPES : uint16_t
    {
        FIFO         = 0x1000,
        CHAR_DEVICE  = 0x2000,
        DIRECTORY    = 0x4000,
        BLOCK_DEVICE = 0x6000,
        REGULAR_FILE = 0x8000,
        SYMLINK      = 0xA000,
        UNIX_SOCKET  = 0xC000,
    };

    enum class DIR_TYPES : uint16_t
    {
        UNKNOW       = 0x0,
        REGULAR_FILE = 0x1,
        DIRECTORY    = 0x2,
        CHAR_FILE    = 0x3,
        BLOCK_DEVICE = 0x4,
        FIFO         = 0x5,
        SOCKET       = 0x6,
        SYMLINK      = 0x7,
    };

    enum class PERMISSIONS : uint16_t
    {
        OTHER_EXECUTE   = 0x001,
        OTHER_WRITE     = 0x002,
        OTHER_READ      = 0x004,
        GROUP_EXECUTE   = 0x008,
        GROUP_WRITE     = 0x010,
        GROUP_READ      = 0x020,
        USER_EXECUTE    = 0x040,
        USER_WRITE      = 0x080,
        USER_READ       = 0x100,
        STICKY_BIT      = 0x200,
        SET_GROUP_ID    = 0x400,
        SET_USER_ID     = 0x800,
    };

    enum class FLAGS : uint32_t
    {
        SECURE_DELETION     = 0x0000'0001,
        KEEP_DELETED_COPY   = 0x0000'0002,
        FILE_COMPRESSION    = 0x0000'0004,
        SYNC_UPDATE         = 0x0000'0008,
        IMMUTABLE_FILE      = 0x0000'0010,
        APPEND_ONLY         = 0x0000'0020,
        FILE_NOT_IN_DUMP    = 0x0000'0040,
        LAST_ACC_TIME_LOCK  = 0x0000'0080,
        HASH_INDEX_DIR      = 0x0001'0000,
        AFS_DIR             = 0x0002'0000,
        JOURNAL_FILE_DATA   = 0x0004'0000,
    };
}

#endif // EXT2_INODE_H
