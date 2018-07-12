#include "util/random.h"

using namespace hfh3;

u32 Random::Get()
{
    // instead of a modulo 2**48, we mask with 2**48-1, which gives the same result
    seed = (multiplier * seed + increment) & mask;
    return static_cast<u32>((seed >> 16) & 0xffffffff);
}

u32 Random::Reset(u64 inSeed)
{
    seed = inSeed;
}

Random::Random(u64 inSeed)
    : seed(inSeed)
{
    // TODO: select an external entropy source for initializing the seed
}
