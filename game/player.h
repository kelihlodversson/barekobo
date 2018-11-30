#pragma once
#include "game/mover.h"

namespace hfh3
{
    class Player : public Mover
    {
    public:
        Player(class GameServer& world, int index, 
               class ImageSheet& imageSheet, class Input& inInput,
               const Vector<s16>& position, const Direction& heading);

        virtual void Draw(class CommandBuffer& view) override;
        virtual void Update() override;
        virtual void OnCollision(class Actor* other) override;

        virtual int GetOwner() const override
        {
            return playerIndex;
        }
    private:
        void Fire(bool repeat);
        int fireRateCounter;
        bool firePressed; 

        int playerIndex;
        class Input& input;
        class ImageSheet& imageSheet;
        int invincibleDelay;
    };
}
