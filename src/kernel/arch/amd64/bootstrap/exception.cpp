#include "exception.h"

#include <config.h>
#include <arch/amd64/instructions.h>

namespace xenon
{
    uint64_t nanoseconds_since_boot_;
    irq_handler *interrupt_handlers_;

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

        if (r.int_no == 0) {
            nanoseconds_since_boot_ += NANOSECS_PER_TICK;
        }

        if (interrupt_handlers_ == nullptr) {
            return;
        }

        switch (r.int_no) {
            case 0: // timer
                interrupt_handlers_->trigger_timer();
                break;

            case 1: // keyboard
                interrupt_handlers_->trigger_keyboard();
                break;

            case 3: // serial port 1 COM2
                break;

            case 4: // serial port 0 COM1
                break;

            case 6: // floppy driver
                break;

            case 7: // parallel port LPT1
                break;

            case 12: // PS/2 mouse port
                break;

            case 14: // Primary IDE
                interrupt_handlers_->trigger_ide();
                break;

            case 15: // Secondary IDE
                break;

            default:
                logger::instance().log("Interrupt %d ignored", r.int_no);
                break;
        }
    }

    void assign_irq(uint8_t irq, irq_handler *handler)
    {
        if (irq > 15) {
            logger::instance().log("PANIC: Invalid IRQ %d", irq);
            return;
        }

        interrupt_handlers_ = handler;
    }

    void unassign_irq(uint8_t irq)
    {
        (void)irq;
    }

    void clock::wait_for(uint64_t miliseconds)
    {
        auto ticks = miliseconds + nanoseconds_since_boot_;

        do {
            sti();
            halt();
            cli();
        } while (ticks > nanoseconds_since_boot_);
    }
}
