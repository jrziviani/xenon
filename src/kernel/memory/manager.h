#ifndef MANAGER_H
#define MANAGER_H

#include "kmalloc.h"
#include "buddy.h"
#include "libs/multiboot.h"

const unsigned int ZONES = 1;
const unsigned int ZONE_GPA = 0;

namespace xenon
{
    class manager
    {
        buddy physical_[ZONES];

    public:
        manager();

        void init(multiboot_info_t *info);
    };
}

#endif // MANAGER_H
