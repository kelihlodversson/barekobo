#pragma once
#include "game/sprite.h"

namespace hfh3
{
    class Player : public Sprite
    {
    public:
        Player(class Stage& inStage, class ImageSheet& imageSheet, class Input& inInput);

        virtual void Update() override;
    private:
        class Input& input;
    };
}
