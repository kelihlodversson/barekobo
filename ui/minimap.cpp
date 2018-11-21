#include "ui/minimap.h"

using namespace hfh3;

static const u8 COLOR[] = {
    143, // Empty
    254, // BaseEdge
    224, // BaseCore
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

void MiniMap::Clear()
{
    memset(pixels, COLOR[Empty], size.x*size.y);
}

void MiniMap::Update()
{
}

void MiniMap::Render()
{
    Vector<s16> pos = screen.GetSize()-size;
    screen.DrawImage(pos, image);
    screen.DrawPixel(pos + player_position[0], COLOR[3]);
    screen.DrawPixel(pos + player_position[1], COLOR[4]);
}

Rect<s16> MiniMap::GetBounds() const
{
    return Rect<s16> (screen.GetSize()-size, size);
}

void MiniMap::Plot(const Vector<s16>& at, EntryType type)
{
    Vector<s16> grid_point = at / scale;
    pixels[grid_point.y*size.x + grid_point.x] = COLOR[type];
}

void MiniMap::SetPlayerPosition(u8 player, const Vector<s16>& position)
{
    assert(player < 2);
    player_position[player] = position / scale;
}