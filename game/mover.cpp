#include "game/mover.h"
#include "game/stage.h"

using namespace hfh3;

Mover::Mover(class World& inWorld, class Image* inImages, unsigned inImageCount,
             Direction inDirection, int inSpeed) :
    Sprite(inWorld,
           inImages,
           inImageCount),
    direction(inDirection),
    speed(inSpeed)
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
    Vector<int> delta = direction.ToDelta(speed);
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
