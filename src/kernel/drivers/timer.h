#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

class timer
{
public:
    virtual void wait_for(uint64_t miliseconds) = 0;
};

#endif // TIMER_H
