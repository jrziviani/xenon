#include "isr.h"
#include "instructions.h"
#include "logger.h"

namespace xenon
{
    const uint8_t LONG_MODE_IDT_GATES = 64;

    idt_ptr idt_ptr_;
    idt_entry idt_entries[LONG_MODE_IDT_GATES];

    void print_registers(const regs &r)
    {
        logger::instance().log("Registers ----------");
        logger::instance().log("RAX: 0x%16x    RBX: 0x%16x", r.rax, r.rbx);
        logger::instance().log("RCX: 0x%16x    RDX: 0x%16x", r.rcx, r.rdx);
        logger::instance().log(" R8: 0x%16x     R9: 0x%16x", r.r8, r.r9);
        logger::instance().log("R10: 0x%16x    R11: 0x%16x", r.r10, r.r11);
        logger::instance().log("R12: 0x%16x    R13: 0x%16x", r.r12, r.r13);
        logger::instance().log("R14: 0x%16x    R15: 0x%16x", r.r14, r.r15);
        logger::instance().log("RDI: 0x%16x    RSI: 0x%16x", r.rdi, r.rsi);
        logger::instance().log(" DS: 0x%16x    RBP: 0x%16x", r.ds, r.rbp);
    }

    void excpetion_handler(const fault &r)
    {
        logger::instance().log("[ Exception handler called ]");
        logger::instance().log("ERROR: %d, ERROR CODE: 0b%5b", r.error_no, r.error_code);
        logger::instance().log("    RIP: 0x%16x", r.rip);
        logger::instance().log(" RFLAGS: 0x%16x", r.rflags);
        logger::instance().log("USERRSP: 0x%16x", r.userrsp);
        logger::instance().log(" CS: 0x%16x     SS: 0x%16x", r.cs, r.ss);

        print_registers(r.registers);
        halt();
    }

    void interrupt_handler(const fault &r)
    {
        (void)r;
        logger::instance().log("[ IRQ handler called ]");
    }

    template <typename F>
    void register_handler(F &&fn)
    {

    }

    void irq_remap()
    {
        // https://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interrupt_Controller
        // https://wiki.osdev.org/8259_PIC
        //
        // NOTE: Not sure if it's still required for modern APIC chips
        //
        // this function will remaps the interrupt numbers these IRQs sends to
        // CPU to avoid conflicts between HW and SW interrupts, because low 32
        // interrupts are used for exceptions (page fault, div 0, etc).
        // PIC1 will issue IRQ OFFSET_PIC1 .. OFFSET_PIC1 + 7
        // PIC2 will issue IRQ OFFSET_PIC2 .. OFFSET_PIC2 + 7
        const int OFFSET_PIC1 = 0x20;
        const int OFFSET_PIC2 = 0x28;

        // PIC interrupt controllers
        const int PIC1_COMM = 0x20;
        const int PIC1_DATA = 0x21;
        const int PIC2_COMM = 0xA0;
        const int PIC2_DATA = 0xA1;

        // ICWs (initialization Command Word)
        const int ICW1_INIT = 0x11; // INIT, cascade mode, ICW4 required
        const int ICW4_8086 = 0x01;

        // save masks
        auto a1 = inb(PIC1_DATA);
        auto a2 = inb(PIC2_DATA);

        // ICW1: to PIC1 and PIC2. In the initialization mode, 8259A will
        // wait for 3 ICWs
        outb(PIC1_COMM, ICW1_INIT);
        outb(PIC2_COMM, ICW1_INIT);

        // ICW2: to PIC1 and PIC2, setting the vector offset for each.
        // PIC1 -> 0x20 and PIC2 -> 0x28.
        outb(PIC1_DATA, OFFSET_PIC1);
        outb(PIC2_DATA, OFFSET_PIC2);
        //
        // ICW3: Tell master (PIC1) there's slave PIC at IRQ2 (starts 0)
        //       Tell slave (PIC2) its cascade identity number (2 here)
        outb(PIC1_DATA, 0x04);
        outb(PIC2_DATA, 0x02);

        // ICW4: set 8086 mode
        outb(PIC1_DATA, ICW4_8086);
        outb(PIC2_DATA, ICW4_8086);

        // restore saved masks
        outb(PIC1_DATA, a1);
        outb(PIC2_DATA, a2);

        /*
        outb(PIC1_DATA, 0x04); = 0b00000100
        outb(PIC2_DATA, 0x02);     |
                            +------+  +-------...
                   +--------V--+      | 8295A
      -------+     | 8259A  0 0|      | Slave
       80x86 |     | Master 0 1|      |
             |     |        1 2|<----+| intr
             |<----+ intr   0 3|      | id 2
             |     |        0 4|      |
            ...   ...         ...    ...
        */
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
