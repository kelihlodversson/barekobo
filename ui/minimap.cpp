#include "ui/minimap.h"

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
    Vector<s16> pos = screen.GetSize()-size;
    screen.DrawImage(pos, image);
    screen.DrawPixel(pos + player_position[0], PLAYER_COLOR[0]);
    screen.DrawPixel(pos + player_position[1], PLAYER_COLOR[1]);
}

Rect<s16> MiniMap::GetBounds() const
{
    return Rect<s16> (screen.GetSize()-size, size);
}

void MiniMap::Plot(const Vector<u8>& grid_point, u8 color)
{
    pixels[grid_point.y*size.x + grid_point.x] = color;
}

void MiniMap::SetPlayerPosition(u8 player, const Vector<s16>& position)
{
    assert(player < 2);
    player_position[player] = position / scale;
}