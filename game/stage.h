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
        Stage(int width, int height);


        /** Modifies the vector reference passed in so it is within
          * the play area.
          */
        Vector<int> WrapCoordinate(const Vector<int>& vector) const
        {
            return {vector.x & maskX, vector.y & maskY};
        }

        int GetWidth() { return size.x; }
        int GetHeight() { return size.y; }
        const Vector<int>& GetSize() { return size; }

    private:
        Vector<int> size;
        int maskX;
        int maskY;
    };
}
