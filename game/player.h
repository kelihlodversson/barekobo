#pragma once
#include "game/mover.h"

namespace hfh3
{
    class Player : public Mover
    {
    public:
        Player(class GameServer& world, class ImageSheet& imageSheet, class Input& inInput,
               const Vector<s16>& position, const Direction& heading);

        virtual void Update() override;

    private:
        void Fire(bool repeat);
        int fireRateCounter;

        class Input& input;
        class ImageSheet& imageSheet;
    };
}
