#include "util/direction.h"

using namespace hfh3;

Vector<s16> Direction::ToDelta(s16 speed)
{
    // Note when travelling diagonally, the speed will be approx. 41% too fast,
    // as the magnitude of the vector will actually be SQRT(2*(speed**2))
    // this is already visible with speed==1, but acceptable for this game, as
    // all positions are represented by integers.
    switch(value)
    {
    case North:
        return Vector<s16>( 0,     -speed);
    case NorthEast:
        return Vector<s16>( speed, -speed);
    case East:
        return Vector<s16>( speed,  0    );
    case SouthEast:
        return Vector<s16>( speed,  speed);
    case South:
        return Vector<s16>( 0,      speed);
    case SouthWest:
        return Vector<s16>(-speed,  speed);
    case West:
        return Vector<s16>(-speed,  0    );
    case NorthWest:
        return Vector<s16>(-speed, -speed);
    default:
    case Stopped:
        return Vector<s16>( 0,      0    );
    }
}
