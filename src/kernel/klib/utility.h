#ifndef UTILITY_H
#define UTILITY_H

#include "move.h"

namespace klib
{
    template <class T>
    constexpr void swap(T &a, T &b) noexcept
    {
        T tmp = move(a);
        a = move(b);
        b = move(tmp);
    }

    template <class T, size_t N>
    constexpr void swap(T (&a)[N], T (&b)[N]) noexcept
    {
        for (size_t i = 0; i < N; i++) {
            swap(a[i], b[i]);
        }
    }

    template <typename T>
    T min(const T &a, const T &b) noexcept
    {
        return (a < b) ? a : b;
    }
}

#endif // UTILITY_H
