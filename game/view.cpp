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

void View::DrawImage(const Vector<s16>& at, const class Image& image)
{
    screen.DrawImage(StageToScreen(at), image);
}

void View::DrawPixel(const Vector<s16>& at, u8 color)
{
    screen.DrawPixel(StageToScreen(at), color);
}

void View::DrawRect(const Rect<s16>& rect, u8 color)
{
    Vector<s16> shifted = rect.origin - screenOffset;
    Vector<s16> wrapped = stage.WrapCoordinate(shifted);
    shifted.x %= stage.GetWidth();
    shifted.y %= stage.GetHeight();

    screen.DrawRect(Rect<s16>(shifted, rect.size), color);
    screen.DrawRect(Rect<s16>(wrapped, rect.size), color);
    screen.DrawRect(Rect<s16>(Vector<s16>(wrapped.x,shifted.y), rect.size), color);
    screen.DrawRect(Rect<s16>(Vector<s16>(shifted.x,wrapped.y), rect.size), color);
}

