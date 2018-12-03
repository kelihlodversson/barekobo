#pragma once
#include <type_traits>
#include <circle/types.h>
#include <machine/endian.h>

/** simple template wrapppers around the GCC builtin byte swap functions */

namespace hfh3
{
    template<typename T, typename U, typename Ret=T>
    using enable_if_same_size_t = std::enable_if_t<
        (std::is_arithmetic<T>::value || std::is_enum<T>::value) && sizeof(T) == sizeof(U),
        Ret
    >;


    // Single byte values don't need swapping
    template<typename T>
    enable_if_same_size_t<T, u8>
    ByteSwap(T value)
    {
        return value;
    }

    template<typename T>
    enable_if_same_size_t<T, u16>
    ByteSwap(T value)
    {
        return __builtin_bswap16(*reinterpret_cast<u16*>(&value));
    }

    template<typename T>
    enable_if_same_size_t<T, u32>
    ByteSwap(T value)
    {
        return __builtin_bswap32(*reinterpret_cast<u32*>(&value));
    }

    template<typename T>
    enable_if_same_size_t<T, u64>
    ByteSwap(T value)
    {
        return __builtin_bswap64(*reinterpret_cast<u64*>(&value));
    }


#if BYTE_ORDER == LITTLE_ENDIAN
    template<typename T>
    T BigEndian(T value)
    {
        return ByteSwap(value);
    }

    template<typename T>
    T LittleEndian(T value)
    {
        return value;
    }
#elif BYTE_ORDER == BIG_ENDIAN
    template<typename T>
    T BigEndian(T value)
    {
        return value;
    }

    template<typename T>
    T LittleEndian(T value)
    {
        return ByteSwap(value);
    }
#else
#error "Unsupported byte order."
#endif


}