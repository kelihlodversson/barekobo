#pragma once
#include "game/actor.h"
#include "util/rect.h"
#include "util/vector.h"
#include "render/image.h"

namespace hfh3
{
    /** Base class for game actors that consist of a single sprite on-screen.
      */
    class Sprite : public Actor {
    public:
      Sprite(class World &inWorld, class Image *inImages,
             unsigned inImageCount, 
             CollisionMask inCollisionTargetMask, CollisionMask inCollisionSourceMask = CollisionMask::None);

      /** After updating all actors, each will get a chance to render itself to screen
          */
      virtual void Draw() override;
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

        class Image& GetImage()
        {
            return images[current];
        }

    private:
        class Image* images;
        unsigned imageCount;
        unsigned current;
    };
}
