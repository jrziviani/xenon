#include <stdint.h>
#include <multiboot.h>

#include "config.h"
#include "logger.h"
#include "isr.h"
#include "memory/manager.h"

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

    logger::instance().log("Initializing IDT");
    init_idt();

    manager memory_manager;
    memory_manager.init(bootinfo);
}
