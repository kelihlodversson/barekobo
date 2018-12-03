#pragma once
#include <circle/types.h>

#include "util/vector.h"
#include "util/rect.h"
#include "util/callback.h"
#include "game/partition.h"
#include "game/collisionmask.h"

namespace hfh3
{
    /** Base class for game actors.
      * game actors represent object in the world that optionally perform actions.
      */
    class Actor
    {
    public:
        Actor(class GameServer& inWorld,
              CollisionMask inCollisionTargetMask = CollisionMask::Any, 
              CollisionMask inCollisionSourceMask = CollisionMask::None);

        /** Since we're using virtual methods, the destructor needs to be virtual.
        */
        virtual ~Actor() = default;

        /** Update is called on each actor once per frame. */
        virtual void Update() = 0;

        /** After updating all actors, each will get a chance to render itself to screen */
        virtual void Draw(class CommandList& commands) = 0;

        /** Return the bounding rectangle of the actor */
        virtual Rect<s16> GetBounds() = 0;

        /** Performs a collision check with the passed in actor object.
          * The collision source mask has to match bits in the other object's target mask 
          * before the objects' bounds are checked for an overlap.
          */
        virtual bool CollisionCheck(class Actor* other);

        /** Called when a hit test identifies an overlap beween two actors.
          */
        virtual void OnCollision(class Actor* other) {}

        /** Called on all actors when a base actor has been destroyed.
          * The number of base Actors left is passed in as argument.
          */
        virtual void OnBaseDestroyed(int basesLeft)
        {
        }

        void Destroy();

        bool IsDestroyed()
        {
            return shouldDestruct;
        }

        const Vector<s16>& GetPosition() const
        {
            return position;
        }

        void SetPosition(const Vector<s16>& newPosition);

        void SetKiller(int player)
        {
            killer = player;
        }

        int GetKiller() const
        {
            return killer;
        }

        /** The player index of the owner of this object.
         */
        virtual int GetOwner() const
        {
            return -1;
        }

        /** Should return the score for destroying this object.
          */
        virtual int GetScore() const
        {
            return 0;
        }

        template<typename T>
        void SetDestructionHandler(T callable)
        {
            destructionHandler = callable;
        }

    protected:
        class GameServer& world;
        class Stage& stage;

    private:
        Vector<s16> position;
        // Store the current partition iterator for easy removal
        // should only be modified by the World class
        Partition::Iterator partitionIterator;

        // This flag is used by the World class to schedule destruction of actors
        bool shouldDestruct;

        // This flag is set whenever an actor has changed its position.
        // used to indicate to the World main loop that it needs to check
        // whether the actor needs to be moved to another partition.
        bool positionDirty;

        // The index of the player that should be awarded points for destroying this object.
        // May be -1 if no player caused the destruction or if the object has not been destroyed.
        int killer;

        // Specifies which collision layers this object can be the target of.
        // Set this to 0 to make the object immune to collisions from other objects.
        const CollisionMask collisionTargetMask;

        // Specifies which layers this object can generate a collision events with.
        const CollisionMask collisionSourceMask;

        Callback<void()> destructionHandler;
        friend class GameServer;
    };
}
