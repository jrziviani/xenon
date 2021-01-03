#ifndef AMD64_INTERFACE_H
#define AMD64_INTERFACE_H

#include "arch_interface.h"

#include <klib/logger.h>
#include <proc/context.h>
#include <drivers/keyboard.h>

class amd64_interface : public arch_interface
{
public:
    int init_interrupts()           final;
    int init_paging()               final;
    int init_timer()                final;
    int init_processes()            final;
    int init_pci()                  final;
    void assign_irq(irq_handler*)   final;

    keyboard *create_keyboard()     final;
    ide *create_ide()               final;

    void cpu_halt()                 final;
};

#endif // AMD64_INTERFACE_H
