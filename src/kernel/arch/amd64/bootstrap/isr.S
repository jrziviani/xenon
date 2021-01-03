#define GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define ISR(...) GET_MACRO(__VA_ARGS__, ISR1, ISR0)(__VA_ARGS__)

#define FN_HEADER(name)     \
    .globl name;            \
    .type name,@function;   \
    name:

#define ISR0(name, num)     \
    FN_HEADER(name)         \
    cli;                    \
    push $num;              \
    jmp isr_handler

#define ISR1(name, num, x)  \
    FN_HEADER(name)         \
    cli;                    \
    push x;                 \
    push $num;              \
    jmp isr_handler

#define IRQ(num)            \
    FN_HEADER(irq##num)     \
    cli;                    \
    push $0x0;              \
    push $num;              \
    jmp irq_handler

#define ISR_ADDR(name) &name

ISR(div,  0, 0);
ISR(dbg,  1, 0);
ISR(nmi,  2, 0);
ISR(bkp,  3, 0);
ISR(ovf,  4, 0);
ISR(bnd,  5, 0);
ISR(ill,  6, 0);
ISR(cop,  7, 0);
ISR(dbl,  8);
ISR(ovr,  9, 0);
ISR(tss, 10);
ISR(seg, 11);
ISR(stk, 12);
ISR(gen, 13);
ISR(pgf, 14);
ISR(unk, 15, 0);
ISR(cft, 16, 0);
ISR(aln, 17);
ISR(mch, 18, 0);
ISR(rs0, 19, 0);
ISR(rs1, 20, 0);
ISR(rs2, 21);
ISR(rs3, 22, 0);
ISR(rs4, 23, 0);
ISR(rs5, 24, 0);
ISR(rs6, 25, 0);
ISR(rs7, 26, 0);
ISR(rs8, 27, 0);
ISR(rs9, 28, 0);
ISR(rsA, 29, 0);
ISR(rsB, 30, 0);
ISR(rsC, 31, 0);

IRQ(0);
IRQ(1);
IRQ(2);
IRQ(3);
IRQ(4);
IRQ(5);
IRQ(6);
IRQ(7);
IRQ(8);
IRQ(9);
IRQ(10);
IRQ(11);
IRQ(12);
IRQ(13);
IRQ(14);
IRQ(15);

.macro save_regs
    push %rdi;
    push %rsi;
    push %rbp;
    push %r15;
    push %r14;
    push %r13;
    push %r12;
    push %r11;
    push %r10;
    push %r9;
    push %r8;
    push %rdx;
    push %rcx;
    push %rbx;
    push %rax;
.endm

.macro restore_regs
    pop %rax;
    pop %rbx;
    pop %rcx;
    pop %rdx;
    pop %r8;
    pop %r9;
    pop %r10;
    pop %r11;
    pop %r12;
    pop %r13;
    pop %r14;
    pop %r15;
    pop %rbp;
    pop %rsi;
    pop %rdi;
.endm

FN_HEADER(isr_handler)
    mov  $1, %r15
    save_regs

    mov  %ds, %ax
    push %rax

    mov  %rsp, %rdi
    call _Z17exception_handlerRK5fault

    pop  %rax
    mov  %ax, %ds
    mov  %ax, %ds

    restore_regs
    add  $16, %rsp
    iretq

FN_HEADER(irq_handler)
    save_regs

    mov  %ds, %ax
    push %rax

    mov  %rsp, %rdi
    call _Z17interrupt_handlerRK5fault

    pop  %rax
    mov  %ax, %ds
    mov  %ax, %ds

    restore_regs
    add  $16, %rsp
    iretq
