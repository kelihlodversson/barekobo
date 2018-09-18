#include "game/enemy.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "game/world.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"

using namespace hfh3;

static const ImageSet enemyImages[7] = {
    ImageSet::Bomb0, ImageSet::Bomb1, ImageSet::Bomb2, ImageSet::Bomb3,
    ImageSet::Arch0, ImageSet::Arch1, ImageSet::Arch2
};

Enemy::Enemy(World& inWorld, ImageSheet& imageSheet, Random& inRandom) :
    Mover(inWorld,
          (u8)enemyImages[inRandom.Get() % 7],
          imageSheet.GetGroupSize(),
          static_cast<Direction>(inRandom.Get() % 8), 1,
          CollisionMask::Enemy, CollisionMask::None),
    random(inRandom),
    relaxed(inRandom.Get() % 100 + 10)
{
    SetPosition(random.GetVector<int>());
}

void Enemy::Update()
{
    UpdatePosition();

    // Change direction at random intervals by + or - 45 degrees.
    if (random.Get() % relaxed == 0)
    {
        //       ___ 0 or 1  ___
        //      _______ 0 or 2 _______
        //      ________ -1 or +1 ________
        int r = (random.Get() % 2) * 2 - 1; // Chose -1 or +1 at random
        SetDirection(GetDirection() + r); // Add it to the current direction to rotate it
    }
}

void Enemy::OnCollision(class Actor* other)
{
    Destroy();
}
