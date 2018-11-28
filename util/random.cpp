#include "util/random.h"

using namespace hfh3;

u32 Random::Get()
{
    // instead of a modulo 2**48, we mask with 2**48-1, which gives the same result
    seed = (multiplier * seed + increment) & mask;

    // only use the upper 32 bits of the seed, as since using a power of two modulus
    // means that the low order bits have a very short period. [L'Ecuyer, 2017]
    return static_cast<u32>((seed >> 16) & 0xffffffff);
}

void Random::Reset(u64 inSeed)
{
    seed = inSeed;
}

Random::Random(u64 inSeed)
    : seed(inSeed)
{
}

Random& Random::Instance()
{
    return instance;
}

Random Random::instance;
