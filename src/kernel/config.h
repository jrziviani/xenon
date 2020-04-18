#ifndef CONFIG_H
#define CONFIG_H

#define KPHYSICAL_ADDRESS    0x400000
#define KVIRTUAL_ADDRESS     0xffffffff80000000

#define MAX_KERNEL_SIZE       32 * 1024 * 1024

#define MAX_CPUS             16

#define VGA_VIRTUAL_ADDRESS  (KVIRTUAL_ADDRESS + 0xb8000)

#define X86_CR0_PE           (1UL)       /* Protected */
#define X86_CR0_PG           (1UL << 31) /* Paging */

#define X86_CR4_PAE          (1UL << 5)  /* Physical address extension */

#define X86_MSR_EFER         0xc0000080  /* Extended feature register */
#define X86_MSR_FS_BASE      0xc0000100  /* 64bit FS base */

#define X86_MSR_EFER_LME     (1 << 8)    /* Long mode enable */
#define _EFER_LME            8

#define FRAME_SIZE 4096

#define IS_ALIGNED(address) ((address & (FRAME_SIZE - 1)) == 0)
#define ALIGN_DOWN(address) ((address) & (-FRAME_SIZE))
#define ALIGN_UP(address)   ALIGN_DOWN(address + FRAME_SIZE - 1)

#endif // CONFIG_H
