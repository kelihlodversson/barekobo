#include "game/enemy.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "game/gameserver.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"
#include "util/tmath.h"

using namespace hfh3;

static const ImageSet enemyImages[7] = {
    ImageSet::Bomb0, ImageSet::Bomb1, ImageSet::Bomb2, ImageSet::Bomb3,
    ImageSet::Arch0, ImageSet::Arch1, ImageSet::Arch2
};

Enemy::Enemy(GameServer& inWorld, ImageSheet& imageSheet) :
    Mover(inWorld,
          (u8)enemyImages[Rand() % 7],
          imageSheet.GetGroupSize(),
          static_cast<Direction>(Rand() % 8), 1,
          CollisionMask::Enemy, CollisionMask::None),
    relaxed(Rand() % 50 + 10)
{
}

void Enemy::Update()
{
    UpdatePosition();

    // Change direction at random intervals 
    if (Rand() % relaxed == 0)
    {
        Vector<s16> delta;
        Actor* player = world.FindPlayer(GetPosition(), 256, delta);
        
        int r = 0;
        // If a player is within range, change direction towards him
        if (player)
        {
            r = Direction(delta)-GetDirection();

            // Limit turning to max 90 degree turns
            if (Abs(r) > 2) 
            {
                r = Sign(r) * 2;
            }
        }
        // else change direction randomly +/- 45 degrees.
        else
        {
            //    _ 0 or 1 _
            //   ____ 0 or 2 ____
            //   _____ -1 or +1 _____
             r = (Rand() % 2) * 2 - 1; // Chose -1 or +1 at random
        }
        SetDirection(GetDirection() + r); // Add it to the current direction to rotate it
    }
}

void Enemy::OnCollision(class Actor* other)
{
    Destroy();
    world.SpawnExplosion(GetPosition(), GetDirection(), GetSpeed());
}
