#ifndef TABLE_H
#define TABLE_H

#include "stdint.h"

namespace klib
{
    class table
    {
        const int BUCKETS = 256;

        uint64_t hash(uint64_t val)
        {
            return (val * 11400714819323198549ul) >> (64 - BUCKETS);
        }
    };
}

#endif // TABLE
