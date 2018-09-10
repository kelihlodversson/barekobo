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
    class Partition
        : public List<class Actor*>
    {
    public:

        Partition(class World& inWorld, const Rect<int>& inBounds)
            : world(inWorld)
            , bounds(inBounds)
            , extendedBounds(inBounds.IsValid()?inBounds.Inflate(16):inBounds)
        {}

        // Returns the nominal bounds for the partition
        const Rect<int>& GetBounds() const { return bounds; }

        // Returns the extended bounds, ie the nominal bounds
        // plus a padding to cover children that extend beyond the inner bounds
        const Rect<int>& GetExtendedBounds() const { return extendedBounds; }

        class World& GetWorld() { return world; }

    private:
        class World& world;
        Rect<int> bounds;
        Rect<int> extendedBounds;
    };
}
