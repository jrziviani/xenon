#ifndef STRING_H
#define STRING_H

#include <klib/stdint.h>
#include "move.h"
#include "cmemory.h"

inline size_t strlen(const char *s)
{
    size_t ret = 0;
    while (*s++ != '\0') {
        ret++;
    }

    return ret;
}

inline char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    for (; i < n; i++) {
        dest[i] = '\0';
    }

    return dest;
}

inline bool isdigit(char c)
{
    return (c >= '0' && c <= '9');
}

inline void *memset(void *dest, int ch, size_t count)
{
    char *tmp = reinterpret_cast<char*>(dest);
    while (count > 0) {
        *tmp++ = ch;
        --count;
    }

    return dest;
}

inline void *memcpy(void *dest, const void *src, size_t count)
{
    char *cdest = static_cast<char*>(dest);
    const char *csrc = static_cast<const char*>(src);

    while (count > 0) {
        *cdest++ = *csrc++;
        --count;
    }

    return dest;
}

inline void *memmove(void *dest, const void *src, size_t count)
{
    char *tmp = new char[count];
    memcpy(tmp, src, count);
    memcpy(dest, tmp, count);

    delete []tmp;
    return dest;
}

class string
{
    size_t capacity_;
    size_t size_;
    unique_ptr<char[]> data_;

public:
    string(const char *str);
    string(size_t capacity);
    string(const string &that);
    string(string &&that);
    ~string();

    size_t size() const
    {
        return size_;
    }

    size_t capacity() const
    {
        return capacity_;
    }

    char at(size_t index) const
    {
        if (index < size_) {
            return data_[index];
        }

        return '\0';
    }

    const char *c_str() const
    {
        return data_.get();
    }

    string &operator=(const string &that) = delete;

    string &operator=(string &&that)
    {
        data_ = move(that.data_);
        capacity_ = that.capacity_;
        size_ = that.size_;
        return *this;
    }
};

#endif // STRING_H
