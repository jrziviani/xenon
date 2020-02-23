#ifndef KMALLOC_H
#define KMALLOC_H

#include "stdint.h"

namespace xenon
{
    uintptr_t kmalloc(size_t size, uintptr_t &physical) noexcept;
    uintptr_t kmalloc(size_t size) noexcept;
    void kfree(uintptr_t addr) noexcept;
}

#endif // KMALLOC_H
