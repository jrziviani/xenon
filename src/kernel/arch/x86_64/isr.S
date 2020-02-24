#define GET_MACRO(_1, _2, _3, NAME, ...) NAME
#define ISR(...) GET_MACRO(__VA_ARGS__, ISR1, ISR0)(__VA_ARGS__)

#define FN_HEADER(name)     \
    .globl name;            \
    .type name,@function;   \
    name:

#define ISR0(name, num)     \
    FN_HEADER(name)         \
    cli;                    \
    push $0x0;              \
    push $num;              \
    jmp isr_handler

#define ISR1(name, num, x)  \
    FN_HEADER(name)         \
    cli;                    \
    push $num;              \
    jmp isr_handler

#define IRQ(num)            \
    FN_HEADER(irq##num)     \
    cli;                    \
    push $0x0;              \
    push $num;              \
    jmp irq_handler

#define ISR_ADDR(name) &name

ISR(div, 0);
ISR(dbg, 1);
ISR(nmi, 2);
ISR(bkp, 3);
ISR(ovf, 4);
ISR(bnd, 5);
ISR(ill, 6);
ISR(cop, 7);
ISR(dbl, 8, 0);
ISR(ovr, 9);
ISR(tss, 10, 0);
ISR(seg, 11, 0);
ISR(stk, 12, 0);
ISR(gen, 13, 0);
ISR(pgf, 14, 0);
ISR(unk, 15);
ISR(cft, 16);
ISR(aln, 17);
ISR(mch, 18);
ISR(rs0, 19);
ISR(rs1, 20);
ISR(rs2, 21);
ISR(rs3, 22);
ISR(rs4, 23);
ISR(rs5, 24);
ISR(rs6, 25);
ISR(rs7, 26);
ISR(rs8, 27);
ISR(rs9, 28);
ISR(rsA, 29);
ISR(rsB, 30);
ISR(rsC, 31);

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

.macro save_context
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

.macro restore_context
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
    mov $1, %r15
    save_context

    mov %ds, %ax
    push %rax

    mov %rsp, %rdi
    call _ZN5xenon17excpetion_handlerERKNS_5faultE

    pop %rax
    mov %ax, %ds
    mov %ax, %ds

    restore_context
    add $16, %rsp
    iretq

FN_HEADER(irq_handler)
    save_context

    mov %ds, %ax
    push %rax

    mov %rsp, %rdi
    call _ZN5xenon17interrupt_handlerERKNS_5faultE

    pop %rax
    mov %ax, %ds
    mov %ax, %ds

    restore_context
    add $16, %rsp
    iretq
