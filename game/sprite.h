#pragma once
#include "game/actor.h"
#include "util/rect.h"
#include "util/vector.h"

namespace hfh3
{
    /** Base class for game actors that consist of a single sprite on-screen.
      */
    class Sprite : public Actor 
    {
    public:
      Sprite(class World &inWorld, 
             u8 inImageGroup, u8 inImageCount, 
             CollisionMask inCollisionTargetMask, 
             CollisionMask inCollisionSourceMask = CollisionMask::None,
             const Vector<int> inSize = {16,16});

      /** After updating all actors, each will get a chance to render itself to screen
          */
      virtual void Draw(class CommandBuffer& view) override;
      virtual Rect<int> GetBounds() override;

    protected:

        void SetImageIndex(unsigned newCurrent)
        {
            current = newCurrent;
        }

        unsigned GetImageIndex() const
        {
            return current;
        }

        unsigned GetImageCount() const
        {
            return imageCount;
        }

    private: 
        u8 imageGroup;
        u8 imageCount;
        u8 current;

        Vector<int> size;
    };
}
