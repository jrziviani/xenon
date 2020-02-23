#ifndef NEW_H
#define NEW_H

#include "stdint.h"
#include "memory/kmalloc.h"

void *operator new(size_t size)
{
    return reinterpret_cast<void*>(xenon::kmalloc(size));
}

void operator delete(void *blk) noexcept
{
    xenon::kfree(reinterpret_cast<uintptr_t>(blk));
}

void operator delete(void *blk, long unsigned int)
{
    xenon::kfree(reinterpret_cast<uintptr_t>(blk));
}

#endif // NEW_H
