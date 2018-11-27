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

Player::Player(class GameServer& inWorld, class ImageSheet& inImageSheet, class Input& inInput,
               const Vector<s16>& position, const Direction& heading) :
    Mover(inWorld,
          (u8)ImageSet::Player0,
          inImageSheet.GetGroupSize(),
          Direction::Stopped, 2,
          CollisionMask::Player, CollisionMask::Enemy),
    input(inInput),
    imageSheet(inImageSheet)
{
    SetImageIndex(heading == Direction::Stopped ? 0 : (int) heading);
    SetPosition(position);
}

void Player::Update()
{
    // Update player direction based on input
    SetDirection(input.GetPlayerDirection());

    auto state = input.GetButtonState(Input::ButtonA);
    if(state & Input::ButtonPressed)
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
    Vector<s16> shotPosition = stage.WrapCoordinate(GetPosition() + shotDirection.ToDelta(16));
    world.SpawnMissile(shotPosition, shotDirection, shotSpeed);

    shotDirection = shotDirection + 4;
    shotPosition = stage.WrapCoordinate(GetPosition() + shotDirection.ToDelta(16));
    world.SpawnMissile(shotPosition, shotDirection, shotSpeed);
}
