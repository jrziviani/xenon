#ifndef MOVE_H
#define MOVE_H

#include "type_traits.h"

template <typename _Tp>
constexpr _Tp &&forward(typename remove_reference<_Tp>::type &__t) noexcept
{
    return static_cast<_Tp&&>(__t);
}

template <typename _Tp>
constexpr _Tp &&forward(typename remove_reference<_Tp>::type &&__t) noexcept
{
    return static_cast<_Tp&&>(__t);
}

template <typename _Tp>
constexpr typename remove_reference<_Tp>::type &&move(_Tp &&__t) noexcept
{
    return static_cast<typename remove_reference<_Tp>::type&&>(__t);
}

#endif // MOVE_H
