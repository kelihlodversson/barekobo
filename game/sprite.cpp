#include "game/sprite.h"
#include "sprite/image.h"
#include "sprite/screen_manager.h"

using namespace hfh3;

Sprite::Sprite(ScreenManager& inScreen, class Image* inImages, unsigned inImageCount) :
    IActor(inScreen),
    images(inImages),
    imageCount(inImageCount),
    current(0)
{}

void Sprite::Draw()
{
    screenManager.DrawImage(position, GetImage());
}

Rect<int> Sprite::GetBounds()
{
    return Rect<int>(position, GetImage().GetSize());
}
