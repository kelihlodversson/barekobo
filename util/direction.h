#pragma once
#include <circle/types.h>

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

        Direction() : value(Stopped)
        {}

        Direction(Value v) : value(v)
        {}

        Direction(int i) : value(static_cast<Value>(i))
        {}

        Direction(unsigned i) : value(static_cast<Value>(i))
        {}

        explicit Direction(Vector<s16> delta);

        operator Value ()
        {
            return value;
        }

        explicit operator int () const
        {
            return static_cast<int>(value);
        }

        explicit operator unsigned int () const
        {
            return static_cast<unsigned>(value);
        }

        // Adding to a direction will rotate
        // clockwise 45 degrees.
        Direction operator+(int other) const
        {
            if(value != Stopped)
            {
                return Direction((value + other) & 7);
            }
            else
            {
                return *this;
            }
        }

        // Subtracting will rotate counter-clockwise
        Direction operator-(int other) const
        {
            return *this + (-other);
        }

        bool operator==(Value other) const
        {
            return value == other;
        }

        bool operator==(const Direction& other) const
        {
            return value == other.value;
        }

        Vector<s16> ToDelta(s16 speed=1);
    private:
        Value value;
    };
}
