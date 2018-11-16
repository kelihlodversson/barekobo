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
          Direction::Stopped, 1,
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

    if(input.GetButtonState(Input::ButtonA) == Input::ButtonPressed)
    {
        Fire();
    }

    // Update position based on current heading
    UpdatePosition();

    // Move the view with the player
    //stage.SetCenterOffset(GetPosition()+GetImage().GetSize()/2);
}

void Player::Fire()
{
    const int shotSpeed = 3;
    Direction shotDirection = static_cast<Direction>(GetImageIndex());
    Vector<s16> shotPosition = stage.WrapCoordinate(GetPosition() + shotDirection.ToDelta(16));

    world.SpawnMissile(shotPosition, shotDirection, shotSpeed);
}
