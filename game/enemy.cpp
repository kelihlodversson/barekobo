#include "game/enemy.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "sprite/image.h"
#include "sprite/imagesheet.h"
#include "util/random.h"

using namespace hfh3;

static const ImageSet enemyImages[9] = {
    ImageSet::Bomb0, ImageSet::Bomb1, ImageSet::Bomb2, ImageSet::Bomb3,
    ImageSet::Arch0, ImageSet::Arch1, ImageSet::Arch2,
    ImageSet::Missile, ImageSet::MiniShot
};

Enemy::Enemy(Stage& inStage, ImageSheet& imageSheet, Random& inRandom) :
    Sprite(inStage,
           imageSheet[(int)enemyImages[inRandom.Get() % 9]],
           imageSheet.GetGroupSize()),
    random(inRandom),
    direction(static_cast<Direction>(inRandom.Get() % 8)),
    relaxed(inRandom.Get() % 100)
{
    SetImageIndex(static_cast<unsigned>(direction));
    position = stage.WrapCoordinate(random.GetVector<int>());
}

void Enemy::Update()
{
    Vector<int> delta = ToDelta(direction);
    position = stage.WrapCoordinate(position + delta);

    // Change direction at random intervals.
    if (random.Get() % relaxed == 0)
    {
        unsigned r = random.Get();
        direction = static_cast<Direction>((direction + (r%3)-1) % 8);
        SetImageIndex(static_cast<unsigned>(direction));
    }
}
