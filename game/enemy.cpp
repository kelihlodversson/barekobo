#include "game/enemy.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"

using namespace hfh3;

static const ImageSet enemyImages[7] = {
    ImageSet::Bomb0, ImageSet::Bomb1, ImageSet::Bomb2, ImageSet::Bomb3,
    ImageSet::Arch0, ImageSet::Arch1, ImageSet::Arch2
};

Enemy::Enemy(Stage& inStage, ImageSheet& imageSheet, Random& inRandom) :
    Mover(inStage,
          imageSheet[(int)enemyImages[inRandom.Get() % 7]],
          imageSheet.GetGroupSize(),
          static_cast<Direction>(inRandom.Get() % 8)),
    random(inRandom),
    relaxed(inRandom.Get() % 100 + 10)
{
    position = stage.WrapCoordinate(random.GetVector<int>());
}

void Enemy::Update()
{
    UpdatePosition();

    // Change direction at random intervals by + or - 45 degrees.
    if (random.Get() % relaxed == 0)
    {
        //      __________ -1 or +1 ______
        //      _______ 0 or 2 _______
        //      ____ 0 or 1  ____
        int r = (random.Get() % 2) * 2 - 1; // Chose -1 or +1 at random
        SetDirection(GetDirection() + r); // Add it to the current direction to rotate it
    }
}
