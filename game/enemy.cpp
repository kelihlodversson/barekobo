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

    // Change direction more often when in locked state
    if (Rand() % relaxed > (state == Locked ? 1 : 0))
    {
        return;
    }

    Vector<s16> delta;
    Actor* player;
    player = world.FindPlayer(GetPosition(), state == Roaming ? 128 : 256, delta);
    int directionAdjustment = 0;

    Direction currentDirection = GetDirection();
    // If a player is within range, change direction towards him
    if (player)
    {
        if (state == Roaming)
        {
            state = Locked;
        }
        Direction targetDirection(delta);

        if (state == Fleeing)
        {
            targetDirection = targetDirection + 4; // When fleeing try to move away instead of towards the player
        }
        directionAdjustment = targetDirection-currentDirection;

        // Limit turning to max 90 degree turns
        if (Abs(directionAdjustment) > 2) 
        {
            directionAdjustment = Sign(directionAdjustment) * 2;
        }
    }
    // else change direction randomly +/- 45 degrees.
    else
    {
        if (state == Locked)
        {
            state = Roaming;
        }

        //                     _ 0 or 1 _
        //                    ____ 0 or 2 ____
        //                    _____ -1 or +1 _____
        directionAdjustment = (Rand() % 2) * 2 - 1; // Chose -1 or +1 at random
    }

    // Apply the selected direction adjustment to the current direction:
    SetDirection(currentDirection + directionAdjustment); 
}

void Enemy::OnCollision(class Actor* other)
{
    SetKiller(other->GetOwner());
    Destroy();
    world.SpawnExplosion(GetPosition(), GetDirection(), GetSpeed());
}

void Enemy::OnBaseDestroyed(int basesRemaining)
{
    if(basesRemaining == 0)
    {
        state = Fleeing;
    }
    else if (state == Roaming)
    {
        state = Locked; // Increases the search range for the next direction change.
    }
}