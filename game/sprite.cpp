#include "game/sprite.h"
#include "game/stage.h"
#include "game/view.h"

#include "render/image.h"

using namespace hfh3;

Sprite::Sprite(class World &inWorld, class Image *inImages, 
               unsigned inImageCount, CollisionMask inCollisionTargetMask, CollisionMask inCollisionSourceMask) 
    : Actor(inWorld, inCollisionTargetMask, inCollisionSourceMask)
    , images(inImages)
    , imageCount(inImageCount)
    , current(0)
{}

void Sprite::Draw(class View& view)
{
    view.DrawImage(GetPosition(), GetImage());
}

Rect<int> Sprite::GetBounds()
{
    return {GetPosition(), GetImage().GetSize()};
}
