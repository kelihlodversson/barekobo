#include "game/stage.h"
#include "sprite/image.h"
#include "sprite/screen_manager.h"

using namespace hfh3;

Stage::Stage(int width, int height, class ScreenManager& inScreen) :
    screen(inScreen),
    maskX(width-1),
    maskY(height-1),
    screenOffset()
{
    /* The stage area must be at least the size of the physical screen */
    assert(width >= inScreen.GetWidth());
    assert(height >= inScreen.GetHeight());

    /* asserts that the passed in width and height are power of two:
       See: https://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2 */
    assert(!(maskX & width));
    assert(!(maskY & height));
}

void Stage::DrawImage(const Vector<int>& at, const class Image& image)
{
    screen.DrawImage(StageToScreen(at), image);
}

void Stage::DrawPixel(const Vector<int>& at, u8 color)
{
    screen.DrawPixel(StageToScreen(at), color);
}

void Stage::DrawRect(const Rect<int>& rect, u8 color)
{
    screen.DrawRect(StageToScreen(rect), color);
}
