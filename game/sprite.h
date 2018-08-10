#pragma once
#include "game/actor.h"
#include "util/rect.h"
#include "util/vector.h"
#include "sprite/image.h"

namespace hfh3
{
    /** Base class for game actors that consist of a single sprite on-screen.
      */
    class Sprite : public IActor {
    public:

        Sprite(class Stage& inStage, class Image* inImages, unsigned inImageCount);

        /** After updating all actors, each will get a chance to render itself to screen
          * @param screenManager the current screen manager to render to
          */
        virtual void Draw() override;

        virtual Rect<int> GetBounds() override;
    protected:

        void SetImageIndex(unsigned newCurrent)
        {
            current = newCurrent;
        }

        unsigned GetImageIndex()
        {
            return current;
        }

        class Image& GetImage()
        {
            return images[current];
        }

        Vector<int> position;
    private:
        class Image* images;
        unsigned imageCount;
        unsigned current;
    };
}
