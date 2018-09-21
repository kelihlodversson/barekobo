#include "game/mover.h"
#include "game/stage.h"

using namespace hfh3;

Mover::Mover(class GameServer &inWorld, u8 inImageGroup, u8 inImageCount,
             Direction inDirection, int inSpeed,
             CollisionMask inCollisionTargetMask, CollisionMask inCollisionSourceMask)
    : Sprite(inWorld, inImageGroup, inImageCount, inCollisionTargetMask, inCollisionSourceMask)
    , direction(inDirection)
    , speed(inSpeed)
{
    if(static_cast<unsigned>(direction) < GetImageCount())
    {
        SetImageIndex(static_cast<unsigned>(direction));
    }
    else
    {
        SetImageIndex(0);
    }
}

void Mover::UpdatePosition()
{
    Vector<s16> delta = direction.ToDelta(speed);
    SetPosition(GetPosition() + delta);
}

void Mover::Update()
{
    UpdatePosition();
}

void Mover::SetDirection(const Direction& dir)
{
    direction = dir;
    if(static_cast<unsigned>(dir) < GetImageCount())
    {
        SetImageIndex(static_cast<unsigned>(dir));
    }
}
