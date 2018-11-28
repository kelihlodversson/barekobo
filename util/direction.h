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

        // Subtracting two directions returns difference in the range -3 to 4
        int operator-(const Direction& other) const
        {
            if (value == Stopped || other.value == Stopped)
            {
                return 0;
            }
            else
            {
                int value = (int(*this) - int(other)) & 7;
                if (value > 3)
                {
                    value -= 8;
                }
                return value;
            }
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
