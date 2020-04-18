#ifndef MANAGER_H
#define MANAGER_H

#include "physical.h"

#include <klib/multiboot.h>

namespace xenon
{
    class manager
    {
        free_list physical_;

    public:
        manager(multiboot_info_t *info);
    };
}

#endif // MANAGER_H
