OUTPUT_FORMAT(elf64-x86-64)

KERNEL_LMA = 0x100000;
KERNEL_VMA = 0xffffffff80000000;

ENTRY(_start32)
SECTIONS
{
    /* This is where the bootloader will load the code */
    . = KERNEL_VMA + KERNEL_LMA;

    _start = .;

    /* 4K of code */
    .text ALIGN (4K) : AT(ADDR(.text) - KERNEL_VMA)
    {
        *(.multiboot)
        *(.start)
        *(.text)
        *(.text.*)
    }

    /* 4K of Read-only data */
    .rodata ALIGN (4K) : AT(ADDR(.rodata) - KERNEL_VMA)
    {
        *(.rodata)
    }

    .got ALIGN (4K) : AT(ADDR(.got) - KERNEL_VMA)
    {
        _got = .;
        KEEP(*(.got.plt))
        KEEP(*(.got))
        _egot = .;
    }

    /* 4K of Read-write initialized data. */
    .data ALIGN (4K) : AT(ADDR(.data) - KERNEL_VMA)
    {
        *(.data)
    }

    .eh_frame : AT(ADDR(.eh_frame) - KERNEL_VMA) {
        _eh_frame = .;
        *(.eh_frame)
        *(.eh_frame_hdr)
    }

    _edata = .;

    /* 4K of Read-write uninitialized data. */
    .bss ALIGN (4K) : AT(ADDR(.bss) - KERNEL_VMA)
    {
        _bss = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }

    _end = .;
}

