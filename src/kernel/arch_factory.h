#ifndef ARCH_FACTORY_H
#define ARCH_FACTORY_H

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

    /*
     * set_architecture
     *
     * A simple factory to build architecture dependent code in compile time.
     *
     * +----------+           +----------------+      +------------------+ |
     * | main.cpp |           | arch_factory.h |      | <arch>_interface | |
     * +----+-----+           +--------+-------+      +---------+--------+ |
     *     | |   set_architecture      |     ARCH_interface     |          |
     *     | |----------------------->| |----------------------| |         |
     *     | |                         X                       | |         |
     *     | |                                                 | |         |
     *     | |<------------------------------------------------| |         |
     *     | |                                                 | |         |
     *     | |------------------------------------------------>| |---...-->|
     *     | |           arch dependent interaction            | |         |
     *     ...                                                 ...         |
     */
    arch_interface *set_architecture(ARCHITECTURES arch)
    {
        if (arch == ARCHITECTURES::X86_64) {
            x86_interface *place = reinterpret_cast<x86_interface*>(placement_kalloc(sizeof(x86_interface)));
            return new (place) x86_interface();
        }

        return nullptr;
    }
}

#endif // ARCH_FACTORY_H
