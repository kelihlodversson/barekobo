#pragma once
#include "util/vector.h"

namespace hfh3
{

    class Direction
    {
    public:
        enum Value
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

        Direction(Value v) : value(v)
        {}

        Direction(int i) : value(static_cast<Value>(i))
        {}

        operator Value ()
        {
            return value;
        }

        explicit operator int ()
        {
            return static_cast<int>(value);
        }

        Vector<int> ToDelta(int speed=1);
    private:
        Value value;
    };
}
