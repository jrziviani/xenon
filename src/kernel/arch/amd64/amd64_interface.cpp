#include "amd64_interface.h"
#include "memory/pagetable.h"
#include "memory/amd64_paging.h"
#include "proc/amd64_context.h"
#include "proc/amd64_process_controller.h"
#include "driver/amd64_timer.h"
#include "bootstrap/segments.h"

#include <timer.h>
#include <memory/allocators.h>

namespace xenon
{
    int amd64_interface::init_interrupts()
    {
        segments::idt_setup();
        segments::gdt_setup();

        return 0;
    }

    int amd64_interface::init_paging()
    {
        map_kernel_memory();

        auto place = reinterpret_cast<paging*>(placement_kalloc(sizeof(amd64_paging)));
        paging_ = new (place) amd64_paging();

        return 0;
    }

    int amd64_interface::init_timer()
    {
        auto place = reinterpret_cast<timer*>(placement_kalloc(sizeof(amd64_timer)));
        timer_ = new (place) amd64_timer(10);

        return 0;
    }

    int amd64_interface::create_context()
    {
        auto place = reinterpret_cast<context*>(placement_kalloc(sizeof(amd64_context)));
        context_ = new (place) amd64_context();

        return 0;
    }

    int amd64_interface::create_process_controller(manager &memory_manager)
    {
        auto place = reinterpret_cast<process_controller*>(placement_kalloc(sizeof(amd64_process_controller)));
        process_controller_ = new (place) amd64_process_controller(memory_manager);

        return 0;
    }
}
