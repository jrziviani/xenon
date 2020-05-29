#include <klib/multiboot.h>
#include <klib/logger.h>
#include <klib/timer.h>
#include <memory/manager.h>
#include <proc/process_controller.h>
#include <proc/scheduler.h>

#include "arch_factory.h"
#include "config.h"

using namespace xenon;

void kmain(multiboot_info_t *bootinfo, unsigned long magic)
{
    logger::instance().log("Booting XenonOS\n");

#ifdef __x86_64__

    auto arch = set_architecture(ARCHITECTURES::AMD64);
    if (arch == nullptr) {
        logger::instance().log("PANIC: error setting AMD64 architecture");
        return;
    }

    logger::instance().log("Booting AMD64");

#else // not __x86_64__

    logger::instance().log("PANIC: architecture not supported");
    return;

#endif // __x86_64__

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        logger::instance().log("Invalid magic number 0x%x", magic);
        return;
    }

    if (bootinfo->flags & MULTIBOOT_INFO_CMDLINE) {
        uintptr_t cmdline = bootinfo->cmdline + KVIRTUAL_ADDRESS;
        logger::instance().log("[multiboot] cmdline: %s",
                               reinterpret_cast<char*>(cmdline));
    }

    logger::instance().log("Initializing IDT");
    arch->init_interrupts();

    logger::instance().log("Mapping kernel pages");
    arch->init_paging();

    logger::instance().log("Initializing memory manager");
    manager::instance().initialize(bootinfo, arch->get_paging());

    logger::instance().log("Initializing processes");
    arch->init_processes();

    logger::instance().log("Initializing timers");
    arch->init_timer();

    logger::instance().log("Initializing scheduler");
    scheduler simple_scheduler(arch->get_timer(),
                               *arch->get_process_controller());

    process_controller *p = arch->get_process_controller();
    p->create_dummy_processes();

    /*
    p->set_running_from_queue();
    p->set_running_from_queue();
    p->set_running_from_queue();
    p->set_running_from_queue();
    */

    while (true) {
        // ...
    }
}
