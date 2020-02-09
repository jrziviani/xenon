#ifndef STDLIB_H
#define STDLIB_H

#include "stdint.h"

enum base
{
    bin = 2,
    oct = 8,
    dec = 10,
    hex = 16,
};

static void itoa(char *buffer, size_t size, int64_t value, base b = base::dec)
{
    bool is_negative = false;
    uint64_t uvalue;

    if (size <= 1) {
        return;
    }

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    if (b == base::dec && value < 0) {
        is_negative = true;
        value = -value;
    }
    uvalue = static_cast<uint64_t>(value);

    size_t idx = 0;
    while (uvalue) {
        uint8_t tmp = uvalue % b;
        buffer[idx++] = (tmp > 9) ? tmp - 10 + 'a' : tmp + '0';
        uvalue /= b;
    }

    if (is_negative) {
        buffer[idx++] = '-';
    }
    buffer[idx] = '\0';

    idx--;
    for (size_t i = 0; i < idx; idx--, i++) {
        char tmp = buffer[idx];
        buffer[idx] = buffer[i];
        buffer[i] = tmp;
    }
}

#endif // STDLIB_H
