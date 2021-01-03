#ifndef MOVE_H
#define MOVE_H

#include "type_traits.h"

namespace klib
{
    template <typename _Tp>
    constexpr _Tp &&forward(typename remove_reference<_Tp>::type &__t) noexcept
    {
        return static_cast<_Tp&&>(__t);
    }

    template <typename _Tp>
    using remove_reference_t = typename remove_reference<_Tp>::type;

    template <typename _Tp>
    constexpr _Tp &&forward(remove_reference_t<_Tp> &&__t) noexcept
    {
        return static_cast<_Tp&&>(__t);
    }

    template <typename _Tp>
    constexpr remove_reference_t<_Tp> &&move(_Tp &&__t) noexcept
    {
        return static_cast<typename remove_reference<_Tp>::type&&>(__t);
    }
}

#endif // MOVE_H
