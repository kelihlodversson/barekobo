#pragma once
#include "util/vector.h"

namespace hfh3
{
    enum Direction
    {
        North,
        NorthEast,
        East,
        SouthEast,
        South,
        SouthWest,
        West,
        NorthWest,
        Stopped
    };

    Vector<int> ToDelta(Direction dir, int speed=1);
}
