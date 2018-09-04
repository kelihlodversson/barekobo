#pragma once
#include "game/mover.h"

namespace hfh3
{
    class Player : public Mover
    {
    public:
        Player(class Stage& inStage, class ImageSheet& imageSheet, class Input& inInput);

        virtual void Update() override;
    private:
        class Input& input;
    };
}
