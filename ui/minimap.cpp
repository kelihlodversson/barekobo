#include "ui/minimap.h"
#include "render/font.h"

#include <circle/string.h>

using namespace hfh3;

static const u8 PLAYER_COLOR[] = {
    252, // ThisPlayer
     57, // OtherPlayer
};

MiniMap::MiniMap(MainLoop& mainLoop, const Vector<s16>& worldSize, s16 scaleDown)
    : MainLoop::IUpdatable(mainLoop)
    , scale(scaleDown)
    , size(worldSize / scale)
    , pixels(new u8[size.x*size.y])
    , image(pixels, size.x, size.y, 255, size.x)
{
    Clear();
}

MiniMap::~MiniMap()
{
    delete[] pixels;
}

void MiniMap::Clear(u8 color)
{
    memset(pixels, color, size.x*size.y);
}

void MiniMap::Update()
{
}

void MiniMap::Render()
{
    CString p0,p1;
    p0.Format("%4d, %4d", player_position[0].x, player_position[0].y);
    p1.Format("%4d, %4d", player_position[1].x, player_position[1].y);

    Vector<s16> screenSize = screen.GetSize();
    Vector<s16> pos = screenSize-size;


    screen.Clear(2);
    screen.DrawString({s16(pos.x+10), 12}, p0, PLAYER_COLOR[0], Font::GetDefault());
    screen.DrawString({s16(pos.x+10), 22}, p1, PLAYER_COLOR[1], Font::GetDefault());
    screen.DrawImage(pos, image);
    screen.DrawPixel(pos + player_position[0]/scale, PLAYER_COLOR[0]);
    screen.DrawPixel(pos + player_position[1]/scale, PLAYER_COLOR[1]);
}

Rect<s16> MiniMap::GetBounds() const
{
    Vector<s16> screenSize = screen.GetSize();
    return Rect<s16> (screenSize.x-size.x, 10, size.x, screenSize.y-10);
}

void MiniMap::Plot(const Vector<u8>& grid_point, u8 color)
{
    pixels[grid_point.y*size.x + grid_point.x] = color;
}

void MiniMap::SetPlayerPosition(u8 player, const Vector<s16>& position)
{
    assert(player < 2);
    player_position[player] = position;
}