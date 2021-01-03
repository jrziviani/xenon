#include "amd64_ide.h"

#include <arch/amd64/instructions.h>
#include <klib/logger.h>
#include <drivers/controller/ahci.h>
#include <memory/manager.h>
#include <config.h>

// xv6 - bootmain.c
void wait_disk()
{
    while ((inb(0x1f7) & 0xc0) != 0x40) ;
}

void read_sector(void *dest, uint16_t offset)
{
    (void)dest;
    wait_disk();

    outb(0x1f2, 1);
    outb(0x1f3, offset);
    outb(0x1f3, offset >> 8);
    outb(0x1f3, offset >> 16);
    outb(0x1f3, (offset >> 24) | 0xe0);
    outb(0x1f3, 0x20);

    wait_disk();
    //inl(0x1f0, dest, 512 / 4);
}

void amd64_ide::on_disk_event()
{

}
