#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <klib/stdint.h>

namespace xenon
{
    inline void outb(uint16_t port, uint8_t val)
    {
        asm volatile("outb %1, %0"
                     :
                     : "dN"(port), "a"(val));
    }

    inline uint8_t inb(uint16_t port)
    {
        uint8_t ret;
        asm volatile("inb %1, %0"
                     : "=a"(ret)
                     : "dN"(port));

        return ret;
    }

    inline uint16_t inw(uint16_t port)
    {
        uint16_t ret;
        asm volatile("inw %1, %0"
                     : "=a"(ret)
                     : "dN"(port));

        return ret;
    }

    inline void lidt(uint64_t idt)
    {
        asm volatile("lidt (%0)"
                     :
                     : "r"(idt));
    }

    inline void lgdt(uint64_t gdt)
    {
        asm volatile("lgdt (%0)"
                     :
                     : "r"(gdt));

    }

    inline void init_segments(uint64_t value)
    {
        asm volatile("movq %0, %%ds \t\n \
                      movq %0, %%es \t\n \
                      movq %0, %%ss \t\n \
                      movq %0, %%fs \t\n \
                      movq %0, %%gs \t\n"
                      :
                      : "r"(value));
    }

    inline void sti()
    {
        asm volatile("sti" : : );
    }

    inline void cli()
    {
        asm volatile("cli" : : );
    }

    inline void halt()
    {
        asm volatile("hlt" : : );
    }
}

#endif // INSTRUCTIONS_H
