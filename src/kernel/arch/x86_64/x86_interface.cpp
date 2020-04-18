#include "x86_interface.h"
#include "segments.h"
#include "memory/pagetable.h"

namespace xenon
{
    int x86_interface::init_interrupts()
    {
        segments::idt_setup();
        segments::gdt_setup();

        return 0;
    }

    int x86_interface::init_paging()
    {
        map_kernel_memory();

        return 0;
    }
}
