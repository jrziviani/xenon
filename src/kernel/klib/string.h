#ifndef STRING_H
#define STRING_H

#include <klib/stdint.h>

inline size_t strlen(const char *s)
{
    size_t ret = 0;
    while (*s++ != '\0') {
        ret++;
    }

    return ret;
}

inline bool isdigit(char c)
{
    return (c >= '0' && c <= '9');
}

#endif // STRING_H
