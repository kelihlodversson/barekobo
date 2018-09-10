#include "game/player.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "game/shot.h"
#include "game/partition.h"
#include "game/world.h"

#include "render/image.h"
#include "render/imagesheet.h"
#include "util/direction.h"
#include "input/input.h"

using namespace hfh3;

Player::Player(class World& inWorld, class ImageSheet& inImageSheet, class Input& inInput) :
    Mover(inWorld,
          inImageSheet[(int)ImageSet::Player0],
          inImageSheet.GetGroupSize(),
          Direction::Stopped),
    input(inInput),
    imageSheet(inImageSheet)
{
    SetImageIndex(0);
    position = {0,0};//(stage.GetSize() - GetImage().GetSize())/2;
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
    stage.SetCenterOffset(position+GetImage().GetSize()/2);
}

void Player::Fire()
{
    const int shotSpeed = 3;
    Direction shotDirection = static_cast<Direction>(GetImageIndex());
    Vector<int> shotPosition = stage.WrapCoordinate(position + shotDirection.ToDelta(16));

    world.SpawnMissile(shotPosition, shotDirection, shotSpeed);
}
