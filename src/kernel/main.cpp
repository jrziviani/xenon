#include <klib/multiboot.h>
#include <klib/logger.h>
#include <klib/timer.h>
#include <memory/manager.h>
#include <proc/process_controller.h>

#include "arch_factory.h"
#include "config.h"

#include "test_timer.h"

using namespace xenon;

struct test
{
    int a;
    bool b;
};

void kmain(multiboot_info_t *bootinfo, unsigned long magic)
{
    logger::instance().log("Booting XenonOS\n");

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        logger::instance().log("Invalid magic number 0x%x", magic);
        return;
    }

    if (bootinfo->flags & MULTIBOOT_INFO_CMDLINE) {
        uintptr_t cmdline = bootinfo->cmdline + KVIRTUAL_ADDRESS;
        logger::instance().log("[multiboot] cmdline: %s", reinterpret_cast<char*>(cmdline));
    }

#ifdef __x86_64__
    auto arch = set_architecture(ARCHITECTURES::AMD64);
    if (arch == nullptr) {
        logger::instance().log("PANIC: error setting AMD64 architecture");
    }

    logger::instance().log("Booting AMD64");
#else
    logger::instance().log("PANIC: architecture not supported");
    return;
#endif

    logger::instance().log("Initializing IDT");
    arch->init_interrupts();

    logger::instance().log("Initializing Timers");
    arch->init_timer();

    logger::instance().log("Mapping kernel pages");
    arch->init_paging();

    logger::instance().log("Initializing physical memory allocator");
    manager memory_manager(bootinfo, arch->get_paging());

    logger::instance().log("Initializing tasking");
    arch->create_context();

    logger::instance().log("Creating initial process");
    arch->create_process_controller(memory_manager);
    process_controller *processes = arch->get_process_controller();
    processes->create_process(arch->get_context());
    processes->set_running_from_queue();

    memory_manager.mmap(0x0, 1, 0);
    *((uintptr_t*)0x0) = 55;
    logger::instance().log("Content 0x0: 0x%x", *reinterpret_cast<uintptr_t*>(0x0));
    memory_manager.unmap(0x0, 1);
    //logger::instance().log("Content 0x0: 0x%x", *reinterpret_cast<uintptr_t*>(0x0));

    test *t = static_cast<test*>(memory_manager.kalloc(sizeof(test)));
    t->a = 80;
    t->b = true;
    logger::instance().log("%d, %s", t->a, (t->b) ? "true" : "false");

        /*
        auto phy = physical_.alloc();
        pages_.map(0x0, phy, 0);
        *((uintptr_t*)0x0) = 83;
        logger::instance().log("0x0: 0x%x", *reinterpret_cast<uintptr_t*>(0x0));
        */
}
