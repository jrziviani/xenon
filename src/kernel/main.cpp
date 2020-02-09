#include <stdint.h>
#include <multiboot.h>

#include "isr.h"

void kmain(multiboot_info_t *bootinfo, unsigned long magic)
{
    (void)bootinfo;
    (void)magic;

    xenon::init_idt();
}
