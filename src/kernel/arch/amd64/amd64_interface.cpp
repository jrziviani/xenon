#include "amd64_interface.h"
#include "memory/pagetable.h"
#include "memory/amd64_paging.h"
#include "proc/sync.h"
#include "proc/amd64_context.h"
#include "proc/amd64_process_controller.h"
#include "bootstrap/segments.h"
#include "bootstrap/exception.h"
#include "driver/amd64_timer.h"
#include "driver/amd64_pci.h"
#include "driver/amd64_keyboard.h"

#include <klib/new.h>

namespace xenon
{
    int amd64_interface::init_pci()
    {
        pci_ = new amd64_pci();

        return 0;
    }

    int amd64_interface::init_interrupts()
    {
        segments::idt_setup();
        segments::gdt_setup();

        return 0;
    }

    int amd64_interface::init_paging()
    {
        map_kernel_memory();
        paging_ = new amd64_paging();

        return 0;
    }

    int amd64_interface::init_timer()
    {
        timer_ = new amd64_timer(10);

        return 0;
    }

    void amd64_interface::assign_irq(irq_handler *hdl)
    {
        xenon::assign_irq(0, hdl);
    }

    int amd64_interface::init_processes()
    {
        // https://forum.osdev.org/viewtopic.php?f=1&t=15622
        // - create a new page directory (mapped into kernel space)
        // - copy kernel page tables into the new page directory
        // - setup scheduler data (thread priority, etc), and set initial RIP to a start_process kernel function
        // - (optional) wait or block until the new thread some sort of status to return
        // - return to caller
        process_controller_ = new amd64_process_controller();
        auto pid = process_controller_->create_process(KSTACK_ADDR,
                                                       KSTACK_SIZE,
                                                       0,
                                                       "[kernel]");
        process_controller_->set_running(pid);

        return 0;
    }

    keyboard *amd64_interface::create_keyboard()
    {
        return new amd64_keyboard();
    }

    void amd64_interface::cpu_halt()
    {
        sti();
        halt();
        cli();
    }
}
