#include "exception.h"
#include "instructions.h"

const uint8_t MAX_INTERRUPT_HANDLERS = 16;

namespace xenon
{
    xenon_base *interrupt_handlers[MAX_INTERRUPT_HANDLERS];

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
        logger::instance().log("ERROR: %d, ERROR CODE: 0b%5b", r.int_no, r.error_code);
        logger::instance().log("    RIP: 0x%16x", r.rip);
        logger::instance().log(" RFLAGS: 0x%16x", r.rflags);
        logger::instance().log("USERRSP: 0x%16x", r.userrsp);
        logger::instance().log(" CS: 0x%16x     SS: 0x%16x", r.cs, r.ss);

        //print_registers(r.registers);

        uint64_t faddr;
        asm volatile ("mov %%cr2, %0" : "=r"(faddr));
        logger::instance().log("fault addr: 0x%x", faddr);
        logger::instance().log("Page is present: %s", (r.error_code & 0x1) ? "yes" : "no");

        halt();
    }

    void interrupt_handler(const fault &r)
    {
        if (r.int_no >= 40) {
            outb(0xa0, 0x20);
        }
        outb(0x20, 0x20);

        if (interrupt_handlers[r.int_no] == 0) {
            //logger::instance().log("DEBUG: No handler to IRQ#%d", r.int_no);
            return;
        }

        switch (r.int_no) {
            case 0: // timer
                interrupt_handlers[0]->on_time(0);
                break;

            default:
                break;
        }
    }

    void assign_irq(uint8_t irq, xenon_base *handler)
    {
        if (irq > 15) {
            logger::instance().log("PANIC: Invalid IRQ %d", irq);
            return;
        }

        cli();
        interrupt_handlers[irq] = handler;
        sti();
    }

    void unassign_irq(uint8_t irq)
    {
        cli();
        interrupt_handlers[irq] = nullptr;
        sti();
    }
}
