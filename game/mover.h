#pragma once
#include "game/sprite.h"
#include "util/vector.h"
#include "util/direction.h"

namespace hfh3
{
    /** Base class for game actors that consist of a single sprite on-screen
      * that can move speed pixels in a certain direction every frame.
      */
    class Mover : public Sprite
    {
    public:
      Mover(class GameServer &inWorld, u8 inImageGroup, u8 inImageCount,
            Direction inDir = Direction::Stopped, int inSpeed = 1,
            CollisionMask inCollisionTargetMask = CollisionMask::Any, CollisionMask inCollisionSourceMask = CollisionMask::None);

      /** The default update will simply call UpdatePosition
          */
      virtual void Update() override;

    protected:

        /** Subclasses should insert a call to UpdatePosition somewhere
          * withing Update() if they override it.
          */
        void UpdatePosition();

        void SetSpeed(int s)
        {
            speed = s;
        }

        int GetSpeed() const
        {
            return speed;
        }

        void SetDirection(const Direction& dir);

        const Direction& GetDirection() const
        {
            return direction;
        }

    private:
        Direction direction;
        int     speed;
    };
}
