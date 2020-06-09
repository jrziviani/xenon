#ifndef AMD64_TIMER_H
#define AMD64_TIMER_H

#include <klib/stdint.h>
#include <drivers/timer.h>

namespace xenon
{
    class amd64_timer : public timer
    {
        uint64_t frequency_;

    public:
        amd64_timer(uint64_t frequency);
        ~amd64_timer();

        void wait_for(uint64_t miliseconds) final;
    };
}

#endif // AMD64_TIMER_H
