#ifndef CONFIG_H
#define CONFIG_H

#define KPHYSICAL_ADDRESS    0x400000
#define KVIRTUAL_ADDRESS     0xffffffff80000000ULL

#ifndef __ASSEMBLER__
    #include <klib/stdint.h>

    constexpr size_t   MAX_KERNEL_SIZE = 32_MB;

    constexpr uint64_t KSTACK_ADDR = 0xffffffff80326000;
    constexpr size_t   KSTACK_SIZE = 64_KB;
    constexpr size_t   USTACK_SIZE = 256_KB;

    constexpr size_t   FRAME_SIZE = 4_KB;

    constexpr size_t   MAX_CPUS = 16;

    constexpr uint64_t TICKS_PER_SEC = 10'000'000;
    constexpr uint64_t NANOSECS_PER_TICK = 1'000'000'000ull / TICKS_PER_SEC;

    constexpr uintptr_t VGA_VIRTUAL_ADDRESS = (KVIRTUAL_ADDRESS + 0xb8000);

    constexpr bool IS_ALIGNED(uintptr_t address)
    {
        return ((address & (FRAME_SIZE - 1)) == 0);
    }

    constexpr uintptr_t ALIGN_DOWN(uintptr_t address)
    {
        return address & -FRAME_SIZE;
    }

    constexpr uintptr_t ALIGN_UP(uintptr_t address)
    {
        return ALIGN_DOWN(address + FRAME_SIZE - 1);
    }
#else
    #define X86_CR0_PE           (1UL)       /* Protected */
    #define X86_CR0_PG           (1UL << 31) /* Paging */
    #define X86_CR4_PAE          (1UL << 5)  /* Physical address extension */
    #define X86_MSR_EFER         0xc0000080  /* Extended feature register */
    #define X86_MSR_FS_BASE      0xc0000100  /* 64bit FS base */
    #define X86_MSR_EFER_LME     (1 << 8)    /* Long mode enable */
    #define _EFER_LME            8
#endif // __ASSEMBLY__

#endif // CONFIG_H
