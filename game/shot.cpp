#include "game/shot.h"
#include "game/imagesets.h"
#include "game/gameserver.h"
#include "game/stage.h"
#include "game/collisionmask.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"

using namespace hfh3;

Shot::Shot(class GameServer &inWorld, ImageSheet &imageSheet, ImageSet imageSet,
           const Vector<s16> &inPosition, Direction direction, int speed)
    : Mover(inWorld, (u8)imageSet, imageSheet.GetGroupSize(),
            direction, speed,
            CollisionMask::None, imageSet == ImageSet::MiniShot?CollisionMask::Player:CollisionMask::Enemy),
      rotator(imageSet == ImageSet::MiniShot), ttl(800 / speed)
{
    SetPosition(inPosition);
}

void Shot::Update()
{
    if(--ttl)
    {
        UpdatePosition();

        // rotating shots switch image each frame instead of following direction
        if (rotator)
        {
            SetImageIndex((GetImageIndex()+1) % GetImageCount());
        }
    }
    else
    {
        Destroy();
    }
}

void Shot::Draw(class CommandBuffer& commands)
{
    // Flash the shot the last 15 frames of its lifetime
    if ( ttl > 15 || ttl % 2)
    {
        Mover::Draw(commands);
    }
}

void Shot::OnCollision(class Actor* other)
{
    Destroy();
}

Rect<s16> Shot::GetBounds()
{
    if(rotator)
    {
        return {GetPosition(), {6,6}};
    }
    else
    {
        return {GetPosition()+Vector<s16>(4,4), {8,8}};
    }
}