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

    inline uintptr_t movfrsp()
    {
        uint64_t rsp;
        asm volatile ("mov %%rsp, %0"
                      : "=r"(rsp));
        return rsp;
    }

    inline uintptr_t movfrbp()
    {
        uint64_t rbp;
        asm volatile ("mov %%rbp, %0"
                      : "=r"(rbp));
        return rbp;
    }

    inline uintptr_t movfrip()
    {
        uint64_t rip;
        asm volatile ("lea (%%rip), %0"
                      : "=r"(rip));
        return rip;
    }

    inline paddr_t get_current_page()
    {
        uintptr_t cr3;
        asm volatile("mov %%cr3, %0"
                     : "=r"(cr3));

        return ptr_to<paddr_t>(cr3);
    }

    inline void tlb_flush(paddr_t addr)
    {
        asm volatile("invlpg (%0)"
                     :
                     : "r"(ptr_from(addr))
                     : "memory");
    }

    inline void switch_page(uintptr_t addr)
    {
        asm volatile("mov %0, %%cr3"
                     :
                     : "r"(addr));
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
