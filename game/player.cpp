#include "game/player.h"
#include "game/imagesets.h"
#include "sprite/image.h"
#include "sprite/imagesheet.h"
#include "sprite/screen_manager.h"
#include "util/direction.h"
#include "input/input.h"

using namespace hfh3;

// TODO: get this from the screen manager

Player::Player(class ScreenManager& inScreen, class ImageSheet& imageSheet, class Input& inInput) :
    Sprite(inScreen,
           imageSheet[ImageSet::Player0],
           imageSheet.GetGroupSize()),
    input(inInput)
{
    const int width = screenManager.GetWidth();
    const int height = screenManager.GetHeight();

    SetImageIndex(0);
    position = Vector<int>(width/2-8, height/2-8);
}

void Player::Update()
{
    const int width = screenManager.GetWidth();
    const int height = screenManager.GetHeight();
    
    Direction direction = input.GetPlayerDirection();
    if(direction != Stopped)
    {
        SetImageIndex(static_cast<int>(direction));
        Vector<int> delta = ToDelta(direction);
        position += delta;

        if (position.x < -16) position.x += width + 32;
        else if (position.x > width+16) position.x -= width + 32;
        if (position.y < -16) position.y += height + 32;
        else if (position.y > height+16) position.y -= height + 32;
    }
}
