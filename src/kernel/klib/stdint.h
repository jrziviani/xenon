#ifndef STDINT_H
#define STDINT_H

using int8_t        = signed char;
using uint8_t       = unsigned char;
using int16_t       = signed short;
using uint16_t      = unsigned short;
using int32_t       = signed int;
using uint32_t      = unsigned int;
using int64_t       = signed long int;
using uint64_t      = unsigned long int;

using size_t        = uint64_t;
using uintptr_t     = unsigned long int;
using vaddr_t       = void*;
using paddr_t       = void*;

using pid_t         = int;

template <typename T>
inline T ptr_to(uintptr_t addr)
{
    return reinterpret_cast<T>(addr);
}

template <typename T>
inline uintptr_t ptr_from(T addr)
{
    return reinterpret_cast<uintptr_t>(addr);
}

#endif // STDINT_H
