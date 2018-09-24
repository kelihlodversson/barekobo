#pragma once
#include <circle/types.h>
#include "util/vector.h"

namespace hfh3
{
    /** Simple pseudorandom generator using a linear congruential generator
     *  Uses the same constants as drand48() [Roberts, 1982].
     */
    class Random
    {
    public:
        Random(u64 inSeed = -1);

        u32 Get();

        template <typename T>
        Vector<T> GetVector()
        {
            return {static_cast<T>(Get()), static_cast<T>(Get())};
        }

        void Reset(u64 inSeed);
    private:
        u64 seed;
        static const u64 multiplier = 0x5deece66d;
        static const u32 increment = 11;
        static const u64 mask = 0xffffffffffff; // 2**48 - 1
    };
}
