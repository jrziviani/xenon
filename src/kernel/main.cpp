#include <klib/multiboot.h>
#include <klib/logger.h>
#include <memory/manager.h>

#include "set_arch.h"
#include "config.h"

using namespace xenon;

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
    auto arch = set_architecture(ARCHITECTURES::X86_64);
    if (arch == nullptr) {
        logger::instance().log("PANIC: error setting x86_64 architecture");
    }

    logger::instance().log("Booting x86_64");
#else
    logger::instance().log("PANIC: architecture not supported");
    return;
#endif

    logger::instance().log("Initializing IDT");
    arch->init_interrupts();

    logger::instance().log("Mapping kernel pages");
    arch->init_paging();

    logger::instance().log("Initializing physical memory allocator");
    manager memory_manager(bootinfo);
}
