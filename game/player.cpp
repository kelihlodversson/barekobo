#include "game/player.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "sprite/image.h"
#include "sprite/imagesheet.h"
#include "util/direction.h"
#include "input/input.h"

using namespace hfh3;

Player::Player(class Stage& inStage, class ImageSheet& imageSheet, class Input& inInput) :
    Sprite(inStage,
           imageSheet[(int)ImageSet::Player0],
           imageSheet.GetGroupSize()),
    input(inInput)
{
    SetImageIndex(0);
    position = (stage.GetSize() - GetImage().GetSize())/2;
}

void Player::Update()
{
    Direction direction = input.GetPlayerDirection();
    // The view should move with the player (delayed by one frame)
    stage.SetCenterOffset(position+GetImage().GetSize()/2);
    if(direction != Stopped)
    {
        SetImageIndex(static_cast<int>(direction));
        Vector<int> delta = ToDelta(direction);
        position = stage.WrapCoordinate(position + delta);

    }
}
