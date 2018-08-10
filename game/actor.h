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

        IActor(class Stage& inStage) : stage(inStage)
        {}

        /** Since we're using virtual methods, the destructor needs to be virtual. */
        virtual ~IActor() {}
        /** Update is called on each actor once per frame. */
        virtual void Update() = 0;

        /** After updating all actors, each will get a chance to render itself to screen */
        virtual void Draw() = 0;

        /** Return the bouding rectangle of the actor */
        virtual Rect<int> GetBounds() = 0;
    protected:
        class Stage& stage;
    };
}
