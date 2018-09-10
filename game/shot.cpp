#include "game/shot.h"
#include "game/imagesets.h"
#include "game/world.h"
#include "game/stage.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"

using namespace hfh3;

Shot::Shot(class World& inWorld, ImageSheet& imageSheet, ImageSet imageSet,
           const Vector<int>& inPosition, Direction direction, int speed)
    : Mover(inWorld, imageSheet[(int)imageSet], imageSheet.GetGroupSize(), direction, speed)
    , rotator(imageSet == ImageSet::MiniShot)
    , ttl(inWorld.GetStage().GetScreen().GetWidth() / speed)
{
    SetPosition(inPosition);
}

void Shot::Update()
{
    if(--ttl)
    {
        UpdatePosition();
        world.CollisionCheck(this);

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

void Shot::Draw()
{
    // Flash the shot the last 15 frames of its lifetime
    if ( ttl > 15 || ttl % 2)
    {
        Mover::Draw();
    }
}

void Shot::OnCollision(class Actor* other)
{
    Destroy();
}
