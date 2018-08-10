#include "game/sprite.h"
#include "game/stage.h"
#include "render/image.h"

using namespace hfh3;

Sprite::Sprite(Stage& inStage, class Image* inImages, unsigned inImageCount) :
    IActor(inStage),
    images(inImages),
    imageCount(inImageCount),
    current(0)
{}

void Sprite::Draw()
{
    stage.DrawImage(position, GetImage());
}

Rect<int> Sprite::GetBounds()
{
    return {position, GetImage().GetSize()};
}
