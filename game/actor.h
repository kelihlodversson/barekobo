#pragma once
#include "util/rect.h"
#include "util/dlinklist.h"
#include "util/dlinkitem.h"
#include "game/world.h"

namespace hfh3
{
    /** Base class for game actors.
      * game actors represent object in the world that optionally perform actions.
      */
    class Actor : public DLinkItem<Actor, World>
    {
    public:
        Actor(class Stage& inStage)
            : stage(inStage)
        {}

        /** Since we're using virtual methods, the destructor needs to be virtual. */
        virtual ~Actor() = default;

        /** Update is called on each actor once per frame. */
        virtual void Update() = 0;

        /** After updating all actors, each will get a chance to render itself to screen */
        virtual void Draw() = 0;

        /** Return the bounding rectangle of the actor */
        virtual Rect<int> GetBounds() = 0;

        /** Called when a hit test identifies an overlap beween two actors.
          */
        virtual void OnCollision(class Actor* other) {}

        /** Returns the current world */
        World* GetWorld() const
        {
            return parent;
        }
    protected:
        class Stage& stage;
    };
}
