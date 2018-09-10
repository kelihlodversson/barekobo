#pragma once
#include "util/vector.h"
#include "util/rect.h"
#include "game/partition.h"

namespace hfh3
{
    /** Base class for game actors.
      * game actors represent object in the world that optionally perform actions.
      */
    class Actor
    {
    public:
        Actor(class World& inWorld);

        /** Since we're using virtual methods, the destructor needs to be virtual.
        */
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

        void Destroy()
        {
            shouldDestruct = true;
        }

    protected:
        class World& world;
        class Stage& stage;

        const Vector<int>& GetPosition() const
        {
            return position;
        }

        void SetPosition(const Vector<int>& newPosition);

    private:
        Vector<int> position;
        // Store the current partition iterator for easy removal
        // should only be modified by the World class
        Partition::Iterator partitionIterator;

        // This flag is used by the World class to schedule destruction of actors
        bool shouldDestruct;

        // This flag is set whenever an actor has changed its position.
        // used to indicate to the World main loop that it needs to check
        // whether the actor needs to be moved to another partition.
        bool positionDirty;

        friend class World;
    };
}
