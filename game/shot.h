#pragma once
#include "game/mover.h"
#include "util/direction.h"
#include "util/vector.h"
#include "game/imagesets.h"

namespace hfh3
{
    class Shot : public Mover
    {
    public:
        Shot(class World& inWorld, class ImageSheet& imageSheet, ImageSet imageSet,
             const Vector<s16>& inPosition, Direction direction, int speed = 1);

        virtual void Update() override;
        virtual void Draw(class CommandBuffer& commands) override;
        virtual void OnCollision(class Actor* other) override;
        virtual Rect<s16> GetBounds() override;

    private:
        bool rotator;
        unsigned ttl;
    };
}
