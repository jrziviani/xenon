#ifndef ARCH_FACTORY_H
#define ARCH_FACTORY_H

#include "arch_interface.h"

#include <klib/new.h>

#include <memory/allocators.h>
#include <arch/amd64/amd64_interface.h>

namespace xenon
{
    enum class ARCHITECTURES
    {
        AMD64,
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
        if (arch == ARCHITECTURES::AMD64) {
            amd64_interface *place = reinterpret_cast<amd64_interface*>(placement_kalloc(sizeof(amd64_interface)));
            return new (place) amd64_interface();
        }

        return nullptr;
    }
}

#endif // ARCH_FACTORY_H
