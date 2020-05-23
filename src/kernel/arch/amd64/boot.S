#include "config.h"

/*****************************************************
 * Multiboot header
 ****************************************************/
.code32
.set MAGIC, 0x1BADB002
.set FLAGS, 0x00000003
.set CHECKSUM, -(MAGIC + FLAGS)
.section .multiboot
.p2align 4
multiboot_start:
    .long MAGIC                          // magic multiboot header
    .long FLAGS                          // flags
    .long CHECKSUM                       // header checksum

/*
This is the initial paging scheme used in this kernel:

  PML4               PDPT                PDE                 PTE
+-------+          +-------+          +-------+           +-------+
|       +----+     |       +----+     |       +---------->| 2MiB  |
+-------+    |     +-------+    |     +-------+           |       |
|   0   |    |     |   0   |    |     |       +-----+     +-------+
+-------+    |     +-------+    +---->+-------+     |        PTE
|  ...  |    +---->|  ...  |    |     |  ...  |     |     +-------+
+-------+    |     +-------+    |     +-------+     +---->| 2MiB  |
|   0   |    |     |       +----+     |   0   +           |       |
+-------+    |     +-------+          +-------+           +-------+
|       +----+     |   0   |          |   0   +
+-------+          +-------+          +-------+

Why does it need to be like that?

 Because I'll move the kernel to address 0xffffffff80000000 (-2GiB).
 Note that it's the kernel virtual address, so usermode applications will
 be able to start at virtual address 0x0.

Why 0xffffffff80000000?

 I'll need to investigate more but this address works fine with R_X86_64_32S
 and R_X86_64_32 relocation type, that truncates the value to 32-bit sign
 extend and must matches the original value. If we truncate that value to
 32-bit we get 0x80000000, which is exactly 0xffffffff80000000 signed extended.

 Other values should work as well, but Linux uses the same scheme, so...

Why 4MB mapped pages?

 This is the kernel maximum size that I intend to use.

Why are there two repeated entries on each directory table?

 The first entry is known as identity paging, which basically maps virtual
 pages to physical pages 1:1. The second entry, which covers those
 0xffffffff80000000 + 4M pages, points to that map. The first one is needed
 because we cannot jump to that address immediately:
  1 - create the identity paging
  2 - maps the higher address to the identity paging
  3 - jump to enable long-mode (64-bit), identity paging will be use
  4 - jump to higher address map
  5 - unmap the first entry (identity)

Why the scheme pml4[511]->pdpt[510]->pde[0..1] ?

 Because the address tells us where to map them:
 * pml4: addr >> 39 & 0x1ff
   0xffffffff80000000 >> 39 & 0x1ff = 511
 * pdpt: addr >> 30 & 0x1ff
   0xffffffff80000000 >> 30 & 0x1ff = 510
 * pde: (first 2MiB) addr >> 21 & 0x1ff
   0xffffffff80000000 >> 21 & 0x1ff = 0
 * pde: (last 2MiB): addr + 2MiB >> 21 & 0x1ff
   0xffffffff80000000 + 0x200000 >> 21 & 0x1ff = 1
*/
.section .data
.balign 4096
// each entry maps up to 512GiB. Total is 512 * 512GiB = 256TiB
pml4:
    // maps from 0x0 to 0x7fffffffff [512 GiB]
    .quad pdpt - KVIRTUAL_ADDRESS + 0x3
    // reserve 510 empty entries
    .fill 510, 8, 0
    // maps from 0xff8000000000 [255.5TiB] to 0xffffffffffff [256TiB]
    .quad pdpt - KVIRTUAL_ADDRESS + 0x3

// each entry maps up to 1GiB. Total is 512 * 1GiB = 512GiB
pdpt:
    // maps from 0x0 to 0x3fffffff [1 GiB]
    .quad pde - KVIRTUAL_ADDRESS + 0x3
    // reserve 509 entries
    .fill 509, 8, 0
    // 0x7f80000000 [510GiB] to 0x7fbfffffff [511GiB]
    .quad pde - KVIRTUAL_ADDRESS + 0x3
    // reserve (and zero) the last entry
    .fill 1, 8, 0

// each entry maps up to 2MiB. Total is 2MiB * 512 = 1GiB
pde:
    // maps from 0x0 to 0x1fffff [2MiB]
    .quad pte_2M - KVIRTUAL_ADDRESS + 0x3
    // maps from 0x200000 [2MiB] to 0x3fffff [4MiB]
    .quad pte_4M - KVIRTUAL_ADDRESS + 0x3

    .quad pte_6M - KVIRTUAL_ADDRESS + 0x3
    .quad pte_8M - KVIRTUAL_ADDRESS + 0x3
    // fill the rest with zeroes
    .fill 508, 8, 0

// each entry maps 4KiB. Total is 4KiB * 512 = 2MiB
pte_2M:
    // reserve 512 * 8bytes = 4KiB for page table
    .fill 512, 8, 0

pte_4M:
    // reserve 512 * 8bytes = 4KiB for page table
    .fill 512, 8, 0

pte_6M:
    // reserve 512 * 8bytes = 4KiB for page table
    .fill 512, 8, 0

pte_8M:
    // reserve 512 * 8bytes = 4KiB for page table
    .fill 512, 8, 0

ptes:
    // reserve more 510 tables of 4KiB = 2040KiB
    .fill 260096, 8, 0

pte_end:
    .fill 4096, 8, 0

/*****************************************************
 * GDT table entry
 ****************************************************/
.section ".rodata","a"
.align 4
gdt64:
    // NULL SEGMENT (must exist)
    .quad 0x0000000000000000

gdt64_code:
    // CODE SEGMENT
    .quad (1<<53) | (1<<47) | (1<<44) | (1<<43) | (1<<41)
    //.quad 0x00af9a000000ffff
    //       |         |         |         |         +- readable
    //       |         |         |         +- code segment
    //       |         |         +- must be 1
    //       |         +- present
    //       +- 64-bit

gdt64_end:

// GDT header
init_gdt64_ptr:
    .word gdt64_end - gdt64 - 1
    .long gdt64 - KVIRTUAL_ADDRESS

/*****************************************************
 *  32-bit protect-mode start
 *  Code runs in 32-bit, virtual mem is not enabled
 ****************************************************/
.section .text
.globl _start32
.type _start32, @function
_start32:
    // turn off interrupts
    cli

    // load LDT
    lgdt (init_gdt64_ptr - KVIRTUAL_ADDRESS)

    // setup a stack
    movl $(bootstack - KVIRTUAL_ADDRESS), %esp

    // reset EFLAGS
    pushl $0
    popf

    // eax and ebx points to data stored by Grub, we
    // must save them before messing with
    movl %eax, %edx

    // zero bss section
    xorl %eax, %eax
    movl $(_bss - KVIRTUAL_ADDRESS), %edi
    movl $(_end - KVIRTUAL_ADDRESS), %ecx
    subl %edi, %ecx
    cld
    rep stosb

    // get the multiboot info back
    movl %edx, %esi
    movl %ebx, %edi

    // fill the first 8MiB page table entries
    movl $2048, %eax
    movl $(pte_2M - KVIRTUAL_ADDRESS), %ebx
    movl $0x7, %ecx // 0x3 => page is R/W and present
1:  movl %ecx, 0(%ebx)
    addl $4096, %ecx
    addl $8, %ebx
    decl %eax
    jnz 1b

    // set PAE (physical address extension)
    movl %cr4, %eax
    orl $X86_CR4_PAE, %eax
    movl %eax, %cr4

    // point CR3 to PML4
    movl $(pml4 - KVIRTUAL_ADDRESS), %eax
    mov %eax, %cr3

    // read from EFER register ...
    movl $X86_MSR_EFER, %ecx
    rdmsr
    // ... and set the long mode control bit
    btsl $_EFER_LME, %eax
    wrmsr

    // set up Paging and Protected mode (activating long mode)
    movl %cr0, %eax
    orl $(X86_CR0_PG | X86_CR0_PE), %eax
    movl %eax, %cr0

    // jump to the flat code segment to enable paging
    ljmp $0x8, $(trampoline64 - KVIRTUAL_ADDRESS)

.code64
.globl _start64
.type _start64, @function
.p2align 8
trampoline64:
    // paging enabled, using identity paging mode. Now jump
    // to the higher address mapped (0xffffffff80000000)
    movabsq $_start64, %rax
    jmp *%rax

_start64:
    // setup a stack
    movq $bootstack, %rbp
    movq %rbp, %rsp

    // zero data segments
    // CS: L(ong), D(efault oper. size) and DPL attributes
    // are used in 64-bit mode
    movq $0, %rax
    movq %rax, %ds
    movq %rax, %es
    movq %rax, %ss
    movq %rax, %fs
    movq %rax, %gs

    // set multiboot memory to higher address
    addq $KVIRTUAL_ADDRESS, %rdi

    // unmap page 0
    movq $0x0, pdpt
    movq $0x0, pml4
    invlpg pml4

    // finally go to C++
    call _Z5kmainP14multiboot_infom

    // unexpected :-(
    cli
1:  hlt
    jmp 1b

.section .bss
.balign 4096
    .fill 0x10000, 1, 0
bootstack:
