#include "game/view.h"
#include "render/image.h"
#include "render/screenmanager.h"

using namespace hfh3;

View::View(Stage& inStage,  ScreenManager& inScreen) 
    : stage(inStage)
    , screen(inScreen)
    , screenOffset()
{
    /* The stage area must be at least the size of the physical screen */
    assert(stage.GetWidth() >= screen.GetWidth());
    assert(stage.GetHeight() >= screen.GetHeight());
 }

void View::DrawImage(const Vector<int>& at, const class Image& image)
{
    screen.DrawImage(StageToScreen(at), image);
}

void View::DrawPixel(const Vector<int>& at, u8 color)
{
    screen.DrawPixel(StageToScreen(at), color);
}

void View::DrawRect(const Rect<int>& rect, u8 color)
{
    Vector<int> shifted = rect.origin - screenOffset;
    Vector<int> wrapped = stage.WrapCoordinate(shifted);
    shifted.x %= stage.GetWidth();
    shifted.y %= stage.GetHeight();

    screen.DrawRect(Rect<int>(shifted, rect.size), color);
    screen.DrawRect(Rect<int>(wrapped, rect.size), color);
    screen.DrawRect(Rect<int>(Vector<int>(wrapped.x,shifted.y), rect.size), color);
    screen.DrawRect(Rect<int>(Vector<int>(shifted.x,wrapped.y), rect.size), color);
}

