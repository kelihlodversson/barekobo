#include "game/player.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/direction.h"
#include "input/input.h"

using namespace hfh3;

Player::Player(class Stage& inStage, class ImageSheet& imageSheet, class Input& inInput) :
    Mover(inStage,
          imageSheet[(int)ImageSet::Player0],
          imageSheet.GetGroupSize(),
          Direction::Stopped),
    input(inInput)
{
    SetImageIndex(0);
    position = (stage.GetSize() - GetImage().GetSize())/2;
}

void Player::Update()
{
    // Update player direction based on input
    SetDirection(input.GetPlayerDirection());

    // Update position based on current heading
    UpdatePosition();

    // Move the view with the player
    stage.SetCenterOffset(position+GetImage().GetSize()/2);
}
