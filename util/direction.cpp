#include "util/direction.h"

using namespace hfh3;

Vector<int> hfh3::ToDelta(Direction dir, int speed)
{
    // Note when travelling diagonally, the speed will be approx. 41% too fast,
    // as the magnitude of the vector will actually be SQRT(2*(speed**2))
    // this is already visible with speed==1, but acceptable for this game, as
    // all positions are represented by integers.
    switch(dir)
    {
    case North:
        return { 0,     -speed};
    case NorthEast:
        return { speed, -speed};
    case East:
        return { speed,  0    };
    case SouthEast:
        return { speed,  speed};
    case South:
        return { 0,      speed};
    case SouthWest:
        return {-speed,  speed};
    case West:
        return {-speed,  0    };
    case NorthWest:
        return {-speed, -speed};
    default:
    case Stopped:
        return { 0,      0    };
    }
}
