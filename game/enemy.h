#pragma once
#include "game/mover.h"
#include "util/direction.h"

namespace hfh3
{
    class Enemy : public Mover
    {
    public:
        Enemy(class GameServer& inWorld, class ImageSheet& imageSheet, class Random& inRandom);

        virtual void Update() override;
        virtual void OnCollision(class Actor* other) override;

    private:
        class Random& random;
        int relaxed;
    };
}
