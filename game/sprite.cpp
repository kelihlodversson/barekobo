#include "game/sprite.h"
#include "game/stage.h"
#include "game/commandlist.h"

#include "render/image.h"

using namespace hfh3;

Sprite::Sprite(class GameServer &inWorld, 
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

void Sprite::Draw(CommandList& commands)
{
    commands.DrawSprite(GetPosition(), imageGroup, current);
}

Rect<s16> Sprite::GetBounds()
{
    return {GetPosition(), size};
}
