#pragma once
#include "util/rect.h"

namespace hfh3
{
    /** Base class for game actors.
      * game actors represent object on screen that optionally perform actions.
      *
      */
    class IActor {
    public:

        IActor(class ScreenManager& inScreen) : screenManager(inScreen)
        {}

        /** Since we're using virtual methods, the destructor needs to be virtual. */
        virtual ~IActor() {}
        /** Update is called on each actor once per frame. */
        virtual void Update() = 0;

        /** After updating all actors, each will get a chance to render itself to screen
          * @param screenManager the current screen manager to render to
          */
        virtual void Draw() = 0;
        virtual Rect<int> GetBounds() = 0;
    protected:
        class ScreenManager& screenManager;
    };
}
