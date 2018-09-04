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
        Shot(class Stage& inStage, class ImageSheet& imageSheet, ImageSet imageSet, const Vector<int>& inPosition, Direction direction, int speed = 1);

        virtual void Update() override;
    private:
        bool rotator;
    };
}
