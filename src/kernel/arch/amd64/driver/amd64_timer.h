#ifndef AMD64_TIMER_H
#define AMD64_TIMER_H

#include <klib/stdint.h>
#include <klib/timer.h>

namespace xenon
{
    class amd64_timer : public timer
    {
        uint64_t frequency_;
        uint64_t tick_;

    public:
        amd64_timer(uint64_t frequency);
        ~amd64_timer();

        void on_time(uint64_t freq) final;
    };
}

#endif // AMD64_TIMER_H
