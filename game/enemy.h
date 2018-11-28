#pragma once
#include "game/mover.h"
#include "util/direction.h"

namespace hfh3
{
    class Enemy : public Mover
    {
    public:
        Enemy(class GameServer& inWorld, class ImageSheet& imageSheet);

        virtual void Update() override;
        virtual void OnCollision(class Actor* other) override;

    private:
        int relaxed;
    };
}
