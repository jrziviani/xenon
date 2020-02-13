#include <stdint.h>
#include <multiboot.h>

#include "logger.h"
#include "isr.h"

using namespace xenon;

void kmain(multiboot_info_t *bootinfo, unsigned long magic)
{
    (void)bootinfo;
    (void)magic;

    logger::instance().log("Booting XenonOS");
    logger::instance().log("Initializing IDT");
    init_idt();
}
