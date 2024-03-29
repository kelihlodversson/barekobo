#include "game/player.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "game/shot.h"
#include "game/partition.h"
#include "game/gameserver.h"
#include "game/collisionmask.h"

#include "render/image.h"
#include "render/imagesheet.h"
#include "util/direction.h"
#include "input/input.h"

using namespace hfh3;

Player::Player(class GameServer& inWorld, int index, 
               class ImageSheet& inImageSheet, class Input& inInput,
               const Vector<s16>& position, const Direction& heading) :
    Mover(inWorld,
          (u8)ImageSet::Player0,
          inImageSheet.GetGroupSize(),
          Direction::Stopped, 3,
          CollisionMask::Player, CollisionMask::Enemy),
    fireRateCounter(0),
    firePressed(false),
    playerIndex(index),
    input(inInput),
    imageSheet(inImageSheet),
    invincibleDelay(150)
{
    SetImageIndex(heading == Direction::Stopped ? 0 : (int) heading);
    SetPosition(position);
}

void Player::Draw(class CommandList& view) 
{
    // If the player is invincible, blink the sprite 4 times a second
    if(!invincibleDelay || (invincibleDelay % 15) < 10)
    {
        Mover::Draw(view);
    }
}

void Player::Update()
{
    if(invincibleDelay > 0)
    {
        invincibleDelay--;
    }
    // Update player direction based on input
    SetDirection(input.GetPlayerDirection());

    auto state = input.GetButtonState(Input::ButtonA);
    if (state == Input::ButtonPressed)
    {
        firePressed = true;
    }
    else if (!(state & Input::ButtonPressed))
    {
        firePressed = false;
    }

    if(firePressed)
    {
        Fire(state != Input::ButtonPressed);
    }

    // Update position based on current heading
    UpdatePosition();
}

void Player::Fire(bool repeat)
{
    const int shotSpeed = 6;
    const int fireRate = 8;

    if (repeat && --fireRateCounter > 0)
    {
        return;
    }

    fireRateCounter = fireRate;

    Direction shotDirection = static_cast<Direction>(GetImageIndex());
    world.SpawnMissile(playerIndex, shotDirection, shotSpeed);

    shotDirection = shotDirection + 4;
    world.SpawnMissile(playerIndex, shotDirection, shotSpeed);
}

void Player::OnCollision(Actor* other)
{
    if (! invincibleDelay)
    {
        Destroy();
    }
}