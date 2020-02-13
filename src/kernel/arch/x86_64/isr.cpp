#include "isr.h"
#include "instructions.h"
#include "logger.h"

namespace xenon
{
    const uint8_t LONG_MODE_IDT_GATES = 64;

    idt_ptr idt_ptr_;
    idt_entry idt_entries[LONG_MODE_IDT_GATES];

    void excpetion_handler(const regs &r)
    {
        (void)r;
        logger::instance().log("[ Exception handler called ]");
    }

    void interrupt_handler(const regs &r)
    {
        (void)r;
        logger::instance().log("[ IRQ handler called ]");
    }

    void irq_remap()
    {
        // remap the IRQ table
        outb(0x20, 0x11);
        outb(0xA0, 0x11);
        outb(0x21, 0x20);
        outb(0xA1, 0x28);
        outb(0x21, 0x04);
        outb(0xA1, 0x02);
        outb(0x21, 0x01);
        outb(0xA1, 0x01);
        outb(0x21, 0x0);
        outb(0xA1, 0x0);
    }

    void idt_set_gate(uint8_t gate, void (*fn)())
    {
        uint64_t fnv = reinterpret_cast<uint64_t>(fn);
        idt_entries[gate].offset_lo = fnv & 0xffff;
        idt_entries[gate].selector = 0x8;
        idt_entries[gate].ist = 0;
        idt_entries[gate].reserved_ign_1 = 0;
        idt_entries[gate].type = 0xe;
        idt_entries[gate].zero = 0;
        idt_entries[gate].dpl = 0;
        idt_entries[gate].present = 1;
        idt_entries[gate].offset_hi = (fnv >> 16) & 0xffffffffffff;
        idt_entries[gate].reserved_ign_2 = 0;
    }

    void init_idt()
    {
        static_assert(sizeof(idt_entry) == 16, "sizeof(idt_entry) != 16");

        idt_ptr_.limit = (sizeof(idt_entry) * LONG_MODE_IDT_GATES) - 1;
        idt_ptr_.base = (uint64_t)&idt_entries;
        lidt(reinterpret_cast<uint64_t>(&idt_ptr_));

        idt_set_gate(0, &div);
        idt_set_gate(1, &dbg);
        idt_set_gate(2, &nmi);
        idt_set_gate(3, &bkp);
        idt_set_gate(4, &ovf);
        idt_set_gate(5, &bnd);
        idt_set_gate(6, &ill);
        idt_set_gate(7, &cop);
        idt_set_gate(8, &dbl);
        idt_set_gate(9, &ovr);
        idt_set_gate(10, &tss);
        idt_set_gate(11, &seg);
        idt_set_gate(12, &stk);
        idt_set_gate(13, &gen);
        idt_set_gate(14, &pgf);
        idt_set_gate(15, &unk);
        idt_set_gate(16, &cft);
        idt_set_gate(17, &aln);
        idt_set_gate(18, &mch);
        idt_set_gate(19, &rs0);
        idt_set_gate(20, &rs1);
        idt_set_gate(21, &rs2);
        idt_set_gate(22, &rs3);
        idt_set_gate(23, &rs4);
        idt_set_gate(24, &rs5);
        idt_set_gate(25, &rs6);
        idt_set_gate(26, &rs7);
        idt_set_gate(27, &rs8);
        idt_set_gate(28, &rs9);
        idt_set_gate(29, &rsA);
        idt_set_gate(30, &rsB);
        idt_set_gate(31, &rsC);

        logger::instance().log(" > Set all exception handlers");

        irq_remap();
        logger::instance().log(" > IRQ remapped");

        idt_set_gate(32, &irq0);
        idt_set_gate(33, &irq1);
        idt_set_gate(34, &irq2);
        idt_set_gate(35, &irq3);
        idt_set_gate(36, &irq4);
        idt_set_gate(37, &irq5);
        idt_set_gate(38, &irq6);
        idt_set_gate(39, &irq7);
        idt_set_gate(40, &irq8);
        idt_set_gate(41, &irq9);
        idt_set_gate(42, &irq10);
        idt_set_gate(43, &irq11);
        idt_set_gate(44, &irq12);
        idt_set_gate(45, &irq13);
        idt_set_gate(46, &irq14);
        idt_set_gate(47, &irq15);
        logger::instance().log(" > Set all IRQs");

        sti();
    }
}
