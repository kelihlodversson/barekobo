#pragma once
#include "game/sprite.h"
#include "util/direction.h"

namespace hfh3
{
    class Enemy : public Sprite
    {
    public:
        Enemy(class ScreenManager& inScreen, class ImageSheet& imageSheet, class Random& inRandom);

        virtual void Update() override;
    private:
        class Random& random;
        Direction direction;
        int relaxed;
    };
}
