#include <klib/stdint.h>

namespace __cxxabiv1
{
    using guard_type = uint64_t;

    extern "C"
    {
        int __cxa_guard_acquire(guard_type *g)
        {
            return !(*g);
        }

        void __cxa_guard_release(guard_type *g)
        {
            *g = 1;
        }

        void __cxa_guard_abort(guard_type *g)
        {
            (void)g;
        }
    }
}
