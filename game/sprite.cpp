#include "game/sprite.h"
#include "game/stage.h"
#include "game/commandbuffer.h"

#include "render/image.h"

using namespace hfh3;

Sprite::Sprite(class World &inWorld, 
                u8 inImageGroup, u8 inImageCount, 
                CollisionMask inCollisionTargetMask, 
                CollisionMask inCollisionSourceMask,
                const Vector<s16> inSize) 
    : Actor(inWorld, inCollisionTargetMask, inCollisionSourceMask)
    , imageGroup(inImageGroup)
    , imageCount(inImageCount)
    , current(0)
    , size(inSize)
{}

void Sprite::Draw(CommandBuffer& commands)
{
    commands.DrawSprite(GetPosition(), imageGroup, current);
}

Rect<s16> Sprite::GetBounds()
{
    return {GetPosition(), size};
}
