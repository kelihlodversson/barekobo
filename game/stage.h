#pragma once
#include "util/log.h"
#include "util/vector.h"

namespace hfh3
{

    /** Represents a stage of a given size, which must be a power of two
      * for performance reasons.
      * The stage will wrap around at the edges.
      */
    class Stage
    {
    public:
        /** Creates a new stage of size width x height
          * The width and heights must be power of two.
          */
        Stage(s16 width, s16 height);


        /** Modifies the vector reference passed in so it is within
          * the play area.
          */
        Vector<s16> WrapCoordinate(const Vector<s16>& vector) const
        {
            return Vector<s16>(vector.x & maskX, vector.y & maskY);
        }

        s16 GetWidth() { return size.x; }
        s16 GetHeight() { return size.y; }
        const Vector<s16>& GetSize() { return size; }

    private:
        Vector<s16> size;
        s16 maskX;
        s16 maskY;
    };
}
