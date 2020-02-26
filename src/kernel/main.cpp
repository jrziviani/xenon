#include <klib/stdint.h>
#include <klib/multiboot.h>
#include <klib/logger.h>

// TODO: this dependency shouldn't exist
#include <arch/x86_64/isr.h>

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

    logger::instance().log("Initializing IDT");
    init_idt();
}
