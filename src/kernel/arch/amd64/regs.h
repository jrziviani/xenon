#ifndef REGS_H
#define REGS_H

#include <klib/stdint.h>

namespace xenon
{
    struct regs
    {
        uint64_t ds;

        // general use registers
        uint64_t rax;
        uint64_t rbx;
        uint64_t rcx;
        uint64_t rdx;
        uint64_t r8;
        uint64_t r9;
        uint64_t r10;
        uint64_t r11;
        uint64_t r12;
        uint64_t r13;
        uint64_t r14;
        uint64_t r15;

        uint64_t rbp;    // stack base pointer
        uint64_t rsi;    // source index
        uint64_t rdi;    // destination index
    };

    struct fault
    {
        regs registers;

        uint64_t int_no;
        uint64_t error_code;
        uint64_t rip;
        uint64_t cs;
        uint64_t rflags;
        uint64_t userrsp;
        uint64_t ss;
    };

    /*
    AMD document - page 330

    Each task has its own executions space:
        * code segment and instruction pointer
        * data segment
        * stack segment for each priviledge level
        * general purpose registers
        * rfrags register
        * local descriptor table
        * task register, link to the previously executed task
        * io permission and interrupt permission bitmaps
        * pointer to CR3

    HW support only for legacy mode. 64-bit mode does by software but
    need to initializes some resources:
        * Task-State Segments (TSS) - segment holds processor state associated with a task
        * TSS Descriptor - segment descriptor that defines the task-state segment
        * TSS Selector - segment selector that references the TSS descriptor located in GDT
        * Task Register - register holds TSS selector and TSS descriptor for current task

    */
    struct task_state
    {
        uint32_t ign_1;

        uint64_t rsp0; // kernel stack (ring 0)
        uint64_t rsp1; // kernel stack (ring 1)
        uint64_t rsp2; // kernel stack (ring 2)

        uint64_t ign_2;

        uint64_t ist1; // interrupt stack tables 1 to 7
        uint64_t ist2;
        uint64_t ist3;
        uint64_t ist4;
        uint64_t ist5;
        uint64_t ist6;
        uint64_t ist7;

        uint64_t ign_3;

        uint16_t ign_4;
        uint16_t iomap_base; // io bitmap offset
    } __attribute__((packed));
}

#endif // REGS_H
