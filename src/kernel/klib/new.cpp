#include "new.h"

void *operator new(size_t size)
{
    return kalloc(size);
}

void *operator new[](size_t size)
{
    return kalloc(size);
}

void operator delete(void *p)
{
    return kfree(p);
}

void operator delete[](void *p)
{
    return kfree(p);
}

void operator delete(void*, void*) noexcept
{
}

void operator delete[](void*, void*) noexcept
{
}

void operator delete(void*, unsigned long) noexcept
{
}

void operator delete[](void*, unsigned long) noexcept
{
}
