#include "game/view.h"
#include "render/image.h"
#include "render/screenmanager.h"
#include "util/log.h"

using namespace hfh3;

View::View(Stage& inStage,  ScreenManager& inScreen) 
    : stage(inStage)
    , screen(inScreen)
    , screenOffset()
{
    auto clipSize = screen.GetClip().size;
    /* The stage area must be at least the size of the physical screen */
    //DEBUG("Stage size (%d,%d) Screen size (%d, %d)", stage.GetWidth(), stage.GetHeight(), clipSize.x, clipSize.y);
    assert(stage.GetWidth() >= clipSize.x);
    assert(stage.GetHeight() >= clipSize.y);
 }

void View::DrawImage(const Vector<s16>& at, const class Image& image)
{
    Vector<s16> shifted = at - screenOffset;
    Vector<s16> wrapped = stage.WrapCoordinate(shifted);
    shifted %= stage.GetSize();

    // Depending on the size and position of the image relative to the view area,
    // it can show up in 4 places on the screen
    screen.DrawImage(shifted, image);
    screen.DrawImage(wrapped, image);
    screen.DrawImage(Vector<s16>(wrapped.x,shifted.y), image);
    screen.DrawImage(Vector<s16>(shifted.x,wrapped.y), image);
}

void View::DrawPixel(const Vector<s16>& at, u8 color)
{
    screen.DrawPixel(stage.WrapCoordinate(at - screenOffset), color);
}

void View::DrawRect(const Rect<s16>& rect, u8 color)
{
    Vector<s16> shifted = rect.origin - screenOffset;
    Vector<s16> wrapped = stage.WrapCoordinate(shifted);
    shifted %= stage.GetSize();

    // Depending on the size and position of the rectangle relative to the view area,
    // it can show up in 4 places on the screen
    screen.DrawRect(Rect<s16>(shifted, rect.size), color);
    screen.DrawRect(Rect<s16>(wrapped, rect.size), color);
    screen.DrawRect(Rect<s16>(Vector<s16>(wrapped.x,shifted.y), rect.size), color);
    screen.DrawRect(Rect<s16>(Vector<s16>(shifted.x,wrapped.y), rect.size), color);
}

