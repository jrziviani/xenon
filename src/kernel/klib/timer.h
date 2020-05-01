#ifndef TIMER_H
#define TIMER_H

#include "xenon_base.h"
#include "llist.h"

namespace xenon
{
    class timer : public xenon_base
    {
    protected:
        llist<xenon_base*> registered_;

    public:
        virtual ~timer()
        {
            registered_.clear();
        }

        virtual void on_time(uint64_t freq) = 0;

        void register_me(xenon_base *obj)
        {
            registered_.push_back(obj);
        }
    };
}

#endif // TIMER_H
