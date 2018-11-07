#pragma once
#include "game/mover.h"
#include "util/direction.h"

namespace hfh3
{
    class Explosion : public Mover
    {
    public:
        Explosion(class GameServer& inWorld, class ImageSheet& imageSheet, const Vector<s16>& position, Direction inDir = Direction::Stopped, int inSpeed = 1);

        virtual void Update() override;

    private:
        int ttl;
    };
}
