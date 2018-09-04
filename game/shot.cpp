#include "game/shot.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"

using namespace hfh3;

Shot::Shot(Stage& inStage, ImageSheet& imageSheet, ImageSet imageSet, const Vector<int>& inPosition, Direction direction, int speed) :
    Mover(inStage, imageSheet[(int)imageSet], imageSheet.GetGroupSize(), direction, speed),
    rotator(imageSet == ImageSet::MiniShot)
{
    position = inPosition;
}

void Shot::Update()
{
    UpdatePosition();

    // rotating shots switch image each frame instead of following direction
    if (rotator)
    {
        SetImageIndex((GetImageIndex()+1) % GetImageCount());
    }
}
