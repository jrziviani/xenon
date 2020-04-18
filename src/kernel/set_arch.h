#ifndef SET_ARCH_H
#define SET_ARCH_H

#include "arch_interface.h"

#include <klib/new.h>

#include <memory/allocators.h>
#include <arch/x86_64/x86_interface.h>

namespace xenon
{
    enum class ARCHITECTURES
    {
        X86_64,
        ARM,
        PPC64LE,
    };

    arch_interface *set_architecture(ARCHITECTURES arch)
    {
        if (arch == ARCHITECTURES::X86_64) {
            x86_interface *iface = reinterpret_cast<x86_interface*>(placement_kalloc(sizeof(x86_interface)));
            return new (iface) x86_interface();
        }

        return nullptr;
    }
}

#endif // SET_ARCH_H
