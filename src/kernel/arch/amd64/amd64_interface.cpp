#include "x86_interface.h"
#include "x86_timer.h"
#include "segments.h"
#include "memory/pagetable.h"
#include "memory/x86_context.h"
#include "memory/x86_paging.h"

#include <timer.h>
#include <memory/allocators.h>
#include <memory/context.h>

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

        auto place = reinterpret_cast<paging*>(placement_kalloc(sizeof(x86_paging)));
        paging_ = new (place) x86_paging();

        return 0;
    }

    int x86_interface::init_timer()
    {
        auto place = reinterpret_cast<timer*>(placement_kalloc(sizeof(x86_timer)));
        timer_ = new (place) x86_timer(10);

        return 0;
    }

    int x86_interface::create_context()
    {
        auto place = reinterpret_cast<context*>(placement_kalloc(sizeof(x86_context)));
        context_ = new (place) x86_context();

        return 0;
    }
}