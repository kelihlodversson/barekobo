#include "game/enemy.h"
#include "game/imagesets.h"
#include "sprite/image.h"
#include "sprite/imagesheet.h"
#include "sprite/screen_manager.h"
#include "util/random.h"

using namespace hfh3;

static const ImageSet enemyImages[9] = {
    ImageSet::Bomb0, ImageSet::Bomb1, ImageSet::Bomb2, ImageSet::Bomb3,
    ImageSet::Arch0, ImageSet::Arch1, ImageSet::Arch2,
    ImageSet::Missile, ImageSet::MiniShot
};

Enemy::Enemy(ScreenManager& inScreen, ImageSheet& imageSheet, Random& inRandom) :
    Sprite(inScreen,
           imageSheet[enemyImages[inRandom.Get() % 9]],
           imageSheet.GetGroupSize()),
    random(inRandom),
    direction(static_cast<Direction>(inRandom.Get() % 8)),
    relaxed(inRandom.Get() % 100)
{
    SetImageIndex(static_cast<unsigned>(direction));
    position = Vector<int>(random.Get() % screenManager.GetWidth(), random.Get() % screenManager.GetHeight());
}

void Enemy::Update()
{
    const int width = screenManager.GetWidth();
    const int height = screenManager.GetHeight();
    Vector<int> delta = ToDelta(direction);
    position += delta;
    if (position.x < -16) position.x += width + 32;
    else if (position.x > width+16) position.x -= width + 32;
    if (position.y < -16) position.y += height + 32;
    else if (position.y > height+16) position.y -= height + 32;

    // Change direction at random intervals.
    if (random.Get() % relaxed == 0)
    {
        unsigned r = random.Get();
        direction = static_cast<Direction>((direction + (r%3)-1) % 8);
        SetImageIndex(static_cast<unsigned>(direction));
    }
}
