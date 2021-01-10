#include <klib/multiboot.h>
#include <klib/logger.h>
#include <klib/cmemory.h>
#include <klib/map.h>
#include <memory/manager.h>
#include <proc/process_controller.h>
#include <proc/scheduler.h>
#include <drivers/keyboard.h>
#include <drivers/irq_handler.h>
#include <drivers/bus/pci.h>
#include <drivers/controller/ahci.h>
#include <drivers/device_interface.h>
#include <filesystems/ext2/ext2.h>

#include "arch_factory.h"
#include "config.h"

void kmain(multiboot_info_t *bootinfo, unsigned long magic)
{
    auto &arch = arch_factory::instance();

    klib::logger::instance().log("Booting XenonOS\n");

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        klib::logger::instance().log("Invalid magic number 0x%x", magic);
        return;
    }

    if (bootinfo->flags & MULTIBOOT_INFO_CMDLINE) {
        uintptr_t cmdline = bootinfo->cmdline + KVIRTUAL_ADDRESS;
        klib::logger::instance().log("[multiboot] cmdline: %s",
                               reinterpret_cast<char*>(cmdline));
    }

    //klib::logger::instance().log("Initializing IDT");
    arch->init_interrupts();

    //klib::logger::instance().log("Mapping kernel pages");
    arch->init_paging();

    //klib::logger::instance().log("Initializing memory manager");
    manager::instance().initialize(bootinfo, arch->get_paging());

    //klib::logger::instance().log("Initializing processes");
    arch->init_processes();

    //klib::logger::instance().log("Initializing timers");
    arch->init_timer();

    //klib::logger::instance().log("Initializing scheduler");
    irq_handler irqs;
    arch->assign_irq(&irqs);
    //scheduler simple_scheduler(*arch->get_process_controller());

    //irqs.register_me<scheduler>(&simple_scheduler);
    irqs.register_me<keyboard>(arch->create_keyboard());
    //irqs.register_me<ide>(arch->create_ide());

    /*
    process_controller *p = arch->get_process_controller();
    p->create_dummy_processes();
    */

    klib::logger::instance().log("Initializing PCI");
    arch->init_pci();
    pci *devices = arch->get_pci();
    devices->scan_hw();

    //klib::map<pci_info_t, klib::unique_ptr<device_interface>> device_map;
    for (const auto device : *devices) {
        auto dev = ahci::detect(device);
        if (dev) {
            //device_map.insert(device, klib::move(dev));
            ext2 fs(dev.get());
        }
    }


    while (true) {
        arch->cpu_halt();
    }

    /*
    p->set_running_from_queue();
    p->set_running_from_queue();
    p->set_running_from_queue();
    p->set_running_from_queue();
    */
}
