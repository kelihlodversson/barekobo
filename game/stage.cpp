#include "game/stage.h"
#include "render/image.h"
#include "render/screenmanager.h"

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
    Vector<int> shifted = rect.origin - screenOffset;
    Vector<int> wrapped = WrapCoordinate(shifted);
    shifted.x %= GetWidth();
    shifted.y %= GetHeight();

    screen.DrawRect(Rect<int>(shifted, rect.size), color);
    screen.DrawRect(Rect<int>(wrapped, rect.size), color);
    screen.DrawRect(Rect<int>(Vector<int>(wrapped.x,shifted.y), rect.size), color);
    screen.DrawRect(Rect<int>(Vector<int>(shifted.x,wrapped.y), rect.size), color);
}
