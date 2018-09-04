#pragma once
#include "game/mover.h"
#include "util/direction.h"

namespace hfh3
{
    class Enemy : public Mover
    {
    public:
        Enemy(class Stage& inStage, class ImageSheet& imageSheet, class Random& inRandom);

        virtual void Update() override;
    private:
        class Random& random;
        int relaxed;
    };
}
