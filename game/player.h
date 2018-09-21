#pragma once
#include "game/mover.h"

namespace hfh3
{
    class Player : public Mover
    {
    public:
        Player(class GameServer& world, class ImageSheet& imageSheet, class Input& inInput);

        virtual void Update() override;

    private:
        void Fire();

        class Input& input;
        class ImageSheet& imageSheet;
    };
}
