#include "game/shot.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"

using namespace hfh3;

Shot::Shot(Stage& inStage, ImageSheet& imageSheet, ImageSet imageSet, const Vector<int>& inPosition, Direction direction, int speed) :
    Mover(inStage, imageSheet[(int)imageSet], imageSheet.GetGroupSize(), direction, speed),
    rotator(imageSet == ImageSet::MiniShot),
    ttl(inStage.GetScreen().GetWidth() / speed)
{
    position = inPosition;
}

void Shot::Update()
{
    if(--ttl)
    {
        UpdatePosition();
        GetWorld()->CollisionCheck(this);

        // rotating shots switch image each frame instead of following direction
        if (rotator)
        {
            SetImageIndex((GetImageIndex()+1) % GetImageCount());
        }
    }
    else
    {
        delete this;
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
    delete this;
}
