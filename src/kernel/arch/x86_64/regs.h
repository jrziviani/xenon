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
}

#endif // REGS_H
