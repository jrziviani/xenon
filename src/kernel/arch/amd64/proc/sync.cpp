#include "sync.h"

#include <arch/amd64/instructions.h>

// idea and code from https://wiki.osdev.org/Brendan%27s_Multi-tasking_Tutorial
int IRQ_disable_counter = 0;

void lock_scheduler()
{
    cli();
    IRQ_disable_counter++;
}

void unlock_scheduler()
{
    IRQ_disable_counter--;
    if (IRQ_disable_counter == 0) {
        sti();
    }
}
