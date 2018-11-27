#include "util/direction.h"
#include "util/math.h"
#include "util/tmath.h"
#include <limits.h>

using namespace hfh3;


Vector<s16> Direction::ToDelta(s16 speed)
{

    // When the direction is diagonal (odd numbered directions),
    // the speed is the hypotenuse of an equal sided, right angled
    // triangle. To get the correct x and y components, we use
    // Pythagoras theorem to derive them.
    //
    //  (1) x ** 2 + y ** 2 = speed ** 2,
    //      since x = y:
    //  (2) 2 * (x ** 2) = speed ** 2,
    //  (3) (x ** 2) = (speed ** 2) / 2,
    //  (4) x = SQRT((speed ** 2) / 2)
    //
    // Note. For the speed of 2, the rounding of 1.4 down to 1
    // makes the diagonal speed feel too slow, and therefore
    // we only perform this correction for speeds of 3 or above.
    // 
    if (value & 1 && Abs(speed) > 2)
    {
        double d = Abs(speed);
        speed = Sign(speed)*s16(Round(Sqrt(d*d/2.0)));
    }
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

/** Convert a delta vector back to a direction.
  */
Direction::Direction(Vector<s16> delta)
{
    // Usually when finding the angle of a vector, one would 
    // use the arc tangent taking into account the sign of both components.
    // (Eg. by using the atan2 function.)
    // But since we don't have the full standard library available
    // and only need the angle in 45 degree increments, we can use the following
    // approximation:
    //  tan(-90,0°) = -Inf 
    //  tan(-67,5°) ~ -2.41 
    //  tan(-45,0°) = -1
    //  tan(-22,5°) ~ -0.42
    //  tan(  0,0°) =  0
    //  tan( 22,5°) ~  0.42
    //  tan( 45,0°) =  1
    //  tan( 67,5°) ~  2.41 
    //  tan( 90,0°) = +Inf 

    if ( delta.IsZero() )
    {
        value = Stopped;
    }
    else
    {
        // Calculate the ratio of y to x with two decimals. 
        // When x is 0, use INT_MAX and INT_MIN as stand-ins for +/-infinite
        s32 ratio = delta.x != 0 ? s32(delta.y) * 100 / s32(delta.x)
                                 : delta.y < 0 ? INT_MIN : INT_MAX;

        if (ratio < -241) 
        {
            value = delta.x >= 0 ? North : South;
        }
        else if (ratio <= -042)
        {
            value = delta.x > 0 ? NorthEast : SouthWest;
        }
        else if (ratio < 042)
        {
            value = delta.x > 0 ? East : West;
        }
        else if (ratio <= 241)
        {
            value = delta.x > 0 ? SouthEast : NorthWest;
        }
        else
        {
            value = delta.x >= 0 ? South : North;
        }
    }

}