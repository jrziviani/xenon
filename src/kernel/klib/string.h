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

inline void *memset(void* dest, int ch, size_t count)
{
    char *tmp = reinterpret_cast<char*>(dest);
    while (count > 0) {
        *tmp++ = ch;
        --count;
    }

    return dest;
}

#endif // STRING_H
