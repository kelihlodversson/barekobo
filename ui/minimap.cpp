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
    CString tmp;
    Vector<s16> screenSize = screen.GetSize();
    Vector<s16> pos = screenSize-size;
    Vector<s16> linePos (pos.x+2, 12);

    screen.Clear(2);
    for (int i = 0; i<2; i++)
    {
        tmp.Format("Player %d:", i+1);
        screen.DrawString(linePos, tmp, PLAYER_COLOR[i]-2, Font::GetDefault());
        linePos.y += 10;
        tmp.Format("Score: %06d", player_score[i]);
        screen.DrawString(linePos, tmp, PLAYER_COLOR[i], Font::GetDefault());
        linePos.y += 10;
        tmp.Format("Lives: %6d", player_lives[i]);
        screen.DrawString(linePos, tmp, PLAYER_COLOR[i], Font::GetDefault());
        linePos.y += 20;
    }

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

void MiniMap::SetPlayerLives(u8 player, int lives)
{
    assert(player < 2);
    player_lives[player] = lives;
}

void MiniMap::SetPlayerScore(u8 player, int score)
{
    assert(player < 2);
    player_score[player] = score;

}
