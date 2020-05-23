#include "segments.h"

#include <arch/amd64/instructions.h>
#include <klib/logger.h>


/*
 * Constants
 */
const uint8_t LONG_MODE_GDT_GATES     =  5;
const uint8_t LONG_MODE_IDT_GATES     = 64;


/*
 * Structs
 */
struct idt_entry
{
    uint64_t offset_lo      : 16; // offset to handler entry point
    uint64_t selector       : 16;
    uint64_t ist            :  3; // interrupt-stack table (index) 
    uint64_t reserved_ign_1 :  5; // not used, init with 0
    uint64_t type           :  4; // 0xe (0b1110) 64-bit interrupt gate
    uint64_t zero           :  1; // must be zero
    uint64_t dpl            :  2; // descriptor priviledge-level
    uint64_t present        :  1; // segment present in memory
    uint64_t offset_hi      : 48; // offset to handler entry point
    uint64_t reserved_ign_2 : 32; // not used, init with 0
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

union gdt_code_entry
{
    struct {
        uint64_t ign_1          : 42; // fields ignored in long mode
        uint64_t conforming     :  1; // 4.11.1 Direct Control Transfers, page 100
        uint64_t one            :  2; // must be filed with 1s
        uint64_t dpl            :  2; // descriptor priviledge-devel
        uint64_t present        :  1; // segment present in memory
        uint64_t ign_2          :  5; // fields ignored in long mode
        uint64_t long_mode      :  1; // long or compatibility mode
        uint64_t default_size   :  1; // default operand size (must be 0 in long mode)
        uint64_t ign_3          :  9; // fields ignored in long mode
    } fields;

    uint64_t ulong;
};

union gdt_data_entry
{
    struct {
        uint64_t ign_1          : 41; // fields ignored in long mode
        uint64_t writable       :  1; // segment is writable or readonly
        uint64_t zero           :  2; // must be zeroes
        uint64_t one            :  1; // must be one
        uint64_t ign_2          :  2; // fields ignored in long mode
        uint64_t present        :  1; // segment present in memory
        uint64_t ign_3          : 16; // fields ignored in long mode
    } fields;

    uint64_t ulong;
};

union gdt_tss_entry
{
    // [1] types
    //      0b0010 - 64-bit LDT        0b1001 - Available 64-bit TSS
    //      0b1011 - Busy 64-bit TSS   0b1100 - 64-bit call gate
    //      0b1110 - 64-bit int. gate  0b1111 - 64-bit trap gate
    struct {
        uint64_t limit_lo       : 16; // segment limit
        uint64_t base_addr_lo   : 24; // base address
        uint64_t type           :  4; // segment type[1]
        uint64_t zero           :  1; // must be zero
        uint64_t dpl            :  2; // descriptor priviledge-level (ring)
        uint64_t present        :  1; // segment present in memory
        uint64_t limit_hi       :  4; // segment limit (higher address)
        uint64_t avl            :  1; // availability
        uint64_t ign            :  2; // ignored
        uint64_t granularity    :  1; // granularity
        uint64_t base_addr_md   :  8; // base address
    } fields_lo;

    struct {
        uint64_t base_addr_hi   : 32; // base address
        uint64_t ign_1          :  8; // ignored fields
        uint64_t zero           :  5; // must be zeroes
        uint64_t ign_2          : 19; // ignored fields
    } fields_hi;

    uint64_t ulong[2];
} __attribute__((packed));

struct gdt_entry
{
    uint64_t entry;
} __attribute__((packed));

struct gdt_ptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));


/*
 * Definitions
 */
void irq_remap();
void idt_add_gate(idt_entry *entries, uint8_t gate, void (*function_ptr)());

extern "C"
{
    void gdt_reload(gdt_ptr *);
}

gdt_entry g_entries[LONG_MODE_GDT_GATES];
idt_entry i_entries[LONG_MODE_IDT_GATES];


/*
 * Implementations
 */
namespace xenon
{
    void segments::gdt_setup()
    {
        gdt_ptr gdt;
        gdt_code_entry code;
        gdt_data_entry data;

        code.fields.ign_1        = 0x0;
        code.fields.conforming   = 0x0;
        code.fields.one          = 0x3;
        code.fields.dpl          = 0x0;
        code.fields.present      = 0x1;
        code.fields.ign_2        = 0x0;
        code.fields.long_mode    = 0x1;
        code.fields.default_size = 0x0;
        code.fields.ign_3        = 0x0;

        data.fields.ign_1        = 0x0;
        data.fields.writable     = 0x1;
        data.fields.zero         = 0x0;
        data.fields.one          = 0x1;
        data.fields.ign_2        = 0x0;
        data.fields.present      = 0x1;
        data.fields.ign_3        = 0x0;

        g_entries[0].entry = 0;
        g_entries[1].entry = code.ulong;
        g_entries[2].entry = data.ulong;
        g_entries[3].entry = 0; // TSS
        g_entries[4].entry = 0; // TSS

        gdt.limit = sizeof(g_entries[0]) * LONG_MODE_GDT_GATES - 1;
        gdt.base = reinterpret_cast<uint64_t>(&g_entries);

        gdt_reload(&gdt);
    }

    void tss_set_gate(paddr_t addr)
    {
        gdt_tss_entry tss;
        tss.ulong[0] = g_entries[3].entry;
        tss.ulong[1] = g_entries[4].entry;

        //uint16_t limit    = sizeof(g_entries[0]) * LONG_MODE_GDT_GATES - 1;
        uintptr_t address = ptr_from(addr);

        tss.fields_lo.base_addr_lo = address & 0xffffff;
        tss.fields_lo.base_addr_md = (address >> 24) & 0xff;
        tss.fields_hi.base_addr_hi = address >> 32;
        tss.fields_lo.limit_lo     = 0xffff;
        tss.fields_lo.limit_hi     = 0xf;
        tss.fields_lo.avl          = 0x0;
        tss.fields_lo.dpl          = 0x3;
        tss.fields_lo.present      = 0x1;
        tss.fields_lo.zero         = 0x0;
        tss.fields_lo.type         = 0x9; // 0b1001 - see above
        tss.fields_hi.zero         = 0x0;

        g_entries[3].entry = tss.ulong[0];
        g_entries[4].entry = tss.ulong[1];
    }

    void segments::idt_setup()
    {
        static_assert(sizeof(idt_entry) == 16, "sizeof(idt_entry) != 16");

        idt_ptr idt_ptr;

        idt_ptr.limit = (sizeof(idt_entry) * LONG_MODE_IDT_GATES) - 1;
        idt_ptr.base = reinterpret_cast<uint64_t>(i_entries);
        lidt(reinterpret_cast<uintptr_t>(&idt_ptr));

        idt_add_gate(i_entries, 0, &div);
        idt_add_gate(i_entries, 1, &dbg);
        idt_add_gate(i_entries, 2, &nmi);
        idt_add_gate(i_entries, 3, &bkp);
        idt_add_gate(i_entries, 4, &ovf);
        idt_add_gate(i_entries, 5, &bnd);
        idt_add_gate(i_entries, 6, &ill);
        idt_add_gate(i_entries, 7, &cop);
        idt_add_gate(i_entries, 8, &dbl);

        idt_add_gate(i_entries, 9, &ovr);
        idt_add_gate(i_entries, 10, &tss);
        idt_add_gate(i_entries, 11, &seg);
        idt_add_gate(i_entries, 12, &stk);
        idt_add_gate(i_entries, 13, &gen);
        idt_add_gate(i_entries, 14, &pgf);
        idt_add_gate(i_entries, 15, &unk);
        idt_add_gate(i_entries, 16, &cft);
        idt_add_gate(i_entries, 17, &aln);
        idt_add_gate(i_entries, 18, &mch);
        idt_add_gate(i_entries, 19, &rs0);
        idt_add_gate(i_entries, 20, &rs1);
        idt_add_gate(i_entries, 21, &rs2);
        idt_add_gate(i_entries, 22, &rs3);
        idt_add_gate(i_entries, 23, &rs4);
        idt_add_gate(i_entries, 24, &rs5);
        idt_add_gate(i_entries, 25, &rs6);
        idt_add_gate(i_entries, 26, &rs7);
        idt_add_gate(i_entries, 27, &rs8);
        idt_add_gate(i_entries, 28, &rs9);
        idt_add_gate(i_entries, 29, &rsA);
        idt_add_gate(i_entries, 30, &rsB);
        idt_add_gate(i_entries, 31, &rsC);

        logger::instance().log(" > Set all exception handlers");

        irq_remap();
        logger::instance().log(" > IRQ remapped");

        idt_add_gate(i_entries, 32, &irq0);
        idt_add_gate(i_entries, 33, &irq1);
        idt_add_gate(i_entries, 34, &irq2);
        idt_add_gate(i_entries, 35, &irq3);
        idt_add_gate(i_entries, 36, &irq4);
        idt_add_gate(i_entries, 37, &irq5);
        idt_add_gate(i_entries, 38, &irq6);
        idt_add_gate(i_entries, 39, &irq7);
        idt_add_gate(i_entries, 40, &irq8);
        idt_add_gate(i_entries, 41, &irq9);
        idt_add_gate(i_entries, 42, &irq10);
        idt_add_gate(i_entries, 43, &irq11);
        idt_add_gate(i_entries, 44, &irq12);
        idt_add_gate(i_entries, 45, &irq13);
        idt_add_gate(i_entries, 46, &irq14);
        idt_add_gate(i_entries, 47, &irq15);
        logger::instance().log(" > Set all IRQs");

        sti();
    }
}

void idt_add_gate(idt_entry *entries, uint8_t gate, void (*function_ptr)())
{
    uintptr_t function = reinterpret_cast<uintptr_t>(function_ptr);
    entries[gate].offset_lo = function & 0xffff;
    entries[gate].selector = 0x8;
    entries[gate].ist = 0;
    entries[gate].reserved_ign_1 = 0;
    entries[gate].type = 0xe;
    entries[gate].zero = 0;
    entries[gate].dpl = 0;
    entries[gate].present = 1;
    entries[gate].offset_hi = (function >> 16) & 0xffffffffffff;
    entries[gate].reserved_ign_2 = 0;
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
    //
    //    xenon::outb(PIC1_DATA, 0x04); = 0b00000100
    //    xenon::outb(PIC2_DATA, 0x02);     |
    //                        +------+  +-------...
    //               +--------V--+      | 8295A
    //  -------+     | 8259A  0 0|      | Slave
    //   80x86 |     | Master 0 1|      |
    //         |     |        1 2|<----+| intr
    //         |<----+ intr   0 3|      | id 2
    //         |     |        0 4|      |
    //        ...   ...         ...    ...
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
    auto a1 = xenon::inb(PIC1_DATA);
    auto a2 = xenon::inb(PIC2_DATA);

    // ICW1: to PIC1 and PIC2. In the initialization mode, 8259A will
    // wait for 3 ICWs
    xenon::outb(PIC1_COMM, ICW1_INIT);
    xenon::outb(PIC2_COMM, ICW1_INIT);

    // ICW2: to PIC1 and PIC2, setting the vector offset for each.
    // PIC1 -> 0x20 and PIC2 -> 0x28.
    xenon::outb(PIC1_DATA, OFFSET_PIC1);
    xenon::outb(PIC2_DATA, OFFSET_PIC2);
    //
    // ICW3: Tell master (PIC1) there's slave PIC at IRQ2 (starts 0)
    //       Tell slave (PIC2) its cascade identity number (2 here)
    xenon::outb(PIC1_DATA, 0x04);
    xenon::outb(PIC2_DATA, 0x02);

    // ICW4: set 8086 mode
    xenon::outb(PIC1_DATA, ICW4_8086);
    xenon::outb(PIC2_DATA, ICW4_8086);

    // restore saved masks
    xenon::outb(PIC1_DATA, a1);
    xenon::outb(PIC2_DATA, a2);
}
