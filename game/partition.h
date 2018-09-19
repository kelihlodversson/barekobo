#pragma once
#include "util/rect.h"
#include "util/vector.h"
#include "util/list.h"

namespace hfh3
{
    /** A partition represents a subrectangle of the world
      * Each game actor belongs to a single partition. This reduces
      * the amount of objects to consider when rendering or performing
      * hit tests, as one only has to consider actors in partitions
      * that overlap the area of interest.
      */
    class Partition : public List<class Actor*>
    {
    public:

        Partition()
            : bounds()
            , extendedBounds()
        {}

        void SetBounds(const Rect<s16>& inBounds)
        {
            bounds = inBounds;
            extendedBounds = inBounds.IsValid()?inBounds.Inflate(16):inBounds;
        }

        // Returns the nominal bounds for the partition
        const Rect<s16>& GetBounds() const { return bounds; }

        // Returns the extended bounds, ie the nominal bounds
        // plus a padding to cover children that extend beyond the inner bounds
        const Rect<s16>& GetExtendedBounds() const { return extendedBounds; }

    private:
        Rect<s16> bounds;
        Rect<s16> extendedBounds;
    };
}
