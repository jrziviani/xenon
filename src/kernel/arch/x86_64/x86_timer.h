#ifndef X86_TIMER_H
#define X86_TIMER_H

#include <klib/stdint.h>
#include <klib/timer.h>

namespace xenon
{
    class x86_timer : public timer
    {
        uint64_t frequency_;
        uint64_t tick_;

    public:
        x86_timer(uint64_t frequency);
        ~x86_timer();

        void on_time(uint64_t freq) final;
    };
}

#endif // X86_TIMER_H
