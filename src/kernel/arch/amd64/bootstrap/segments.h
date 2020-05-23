#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <klib/stdint.h>

namespace xenon
{
    namespace segments
    {
        void gdt_setup();
        void idt_setup();
    }
}

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

#endif // SEGMENTS_H
