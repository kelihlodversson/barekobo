#pragma once
#include "game/sprite.h"
#include "util/vector.h"
#include "util/direction.h"

namespace hfh3
{
    /** Base class for game actors that consist of a single sprite on-screen.
      */
    class Mover : public Sprite {
    public:

        Mover(class Stage& inStage, class Image* inImages, unsigned inImageCount,
              Direction inDir = Direction::Stopped, int inSpeed = 1);

        /** The default update will simply call UpdatePosition
          */
        virtual void Update() override;

    protected:

        void UpdatePosition();

        void SetSpeed(int s)
        {
            speed = s;
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
