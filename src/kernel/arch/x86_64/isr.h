#ifndef ISR_H
#define ISR_H

#include "regs.h"

namespace xenon
{
    void excpetion_handler(unsigned int code, const regs &r);
    void interrupt_handler(unsigned int code, const regs &r);

    template <typename F>
    void register_handler(F &&fn);

    void init_idt();

    struct idt_entry
    {
        uint64_t offset_lo : 16;      // offset to handler entry point
        uint64_t selector : 16;
        uint64_t ist : 3;             // interrupt-stack table (index) 
        uint64_t reserved_ign_1 : 5;  // not used, init with 0
        uint64_t type : 4;            // 0xe (0b1110) 64-bit interrupt gate
        uint64_t zero : 1;            // must be zero
        uint64_t dpl : 2;             // descriptor priviledge-level
        uint64_t present : 1;         // segment present in memory
        uint64_t offset_hi : 48;      // offset to handler entry point
        uint64_t reserved_ign_2 : 32; // not used, init with 0
    } __attribute__((packed));

    struct idt_ptr
    {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed));

    extern "C"
    {
        void div();
        void dbg();
        void nmi();
        void bkp();
        void ovf();
        void bnd();
        void ill();
        void cop();
        void dbl();
        void ovr();
        void tss();
        void seg();
        void stk();
        void gen();
        void pgf();
        void unk();
        void cft();
        void aln();
        void mch();
        void rs0();
        void rs1();
        void rs2();
        void rs3();
        void rs4();
        void rs5();
        void rs6();
        void rs7();
        void rs8();
        void rs9();
        void rsA();
        void rsB();
        void rsC();

        void irq0();
        void irq1();
        void irq2();
        void irq3();
        void irq4();
        void irq5();
        void irq6();
        void irq7();
        void irq8();
        void irq9();
        void irq10();
        void irq11();
        void irq12();
        void irq13();
        void irq14();
        void irq15();
    }
}

#endif // ISR_H
