.code64
.section .text
.globl gdt_reload
.type gdt_reload, @function
gdt_reload:
    lgdt (%rdi)
    jmp 1f
    nop
1:
    movl $0x10, %eax
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %ss
    movl %eax, %fs
    movl %eax, %gs

    popq %rax
    pushq $0x8
    pushq %rax
    lretq
