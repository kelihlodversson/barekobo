#pragma once
#include "util/direction.h"
#include "util/vector.h"

#include "game/mover.h"
#include "game/imagesets.h"


namespace hfh3
{
    class Shot : public Mover
    {
    public:
        Shot(class GameServer& inWorld, class ImageSheet& imageSheet, ImageSet imageSet,
             const Vector<s16>& inPosition, Direction direction, int speed = 1,
             int inOwner = -1);

        virtual void Update() override;
        virtual void Draw(class CommandBuffer& commands) override;
        virtual void OnCollision(class Actor* other) override;
        virtual Rect<s16> GetBounds() override;
        
        virtual int GetOwner() const override
        {
            return owner;
        }
    private:
        bool rotator;
        unsigned ttl;
        int owner;
    };
}
