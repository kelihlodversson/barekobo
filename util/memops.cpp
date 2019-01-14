#include "util/memops.h"
#include <circle/types.h>

#if CONFIG_OWN_MEMSET
void *hfh3::MemSet(void *mem, int val, size_t len)
{
    u8* dest = reinterpret_cast<u8*>(mem);
    val &= 0xFF;

    // Repeat value 8 times in a 64 bit unsigned integer.
    u64 fill = val * 0x0101010101010101LL;

    // copy any initial unaligned data first
    while ((reinterpret_cast<intptr>(dest) & 0x7) && len)
    {
        *dest = val;
        dest++; len--;
    }

    // Now write 8 bytes at a time
    u64* aligned = reinterpret_cast<u64*>(dest);
    while(len >= 8)
    {
        *aligned = fill;
        aligned++; len-=8;
    }

    // copy any remaining bytes
    u8* tail = reinterpret_cast<u8*>(aligned);
    while (len)
    {
        *tail = val;
        tail++; len--;
    }

    return mem;
}
#endif