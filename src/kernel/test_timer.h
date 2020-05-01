#ifndef blabla
#define blabla

#include <klib/xenon_base.h>
#include <klib/logger.h>
#include <klib/timer.h>

namespace xenon
{
    class my_class : public xenon_base
    {
    public:
        my_class(timer *t)
        {
            t->register_me(this);
        }

        ~my_class()
        {

        }

        void on_time(uint64_t freq) final
        {
            (void)freq;
            logger::instance().log("good...");
        }
    };
}

#endif
