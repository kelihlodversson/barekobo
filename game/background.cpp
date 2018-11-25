#include "game/background.h"
#include "game/world.h"
#include "game/view.h"

#include "render/image.h"
#include "render/imagesheet.h"

#include "ui/minimap.h"

using namespace hfh3;


static const u8 CLEAR_MAP_COLOR = 143;
static const u8 GROUP_MAP_COLOR[16] = 
{
   CLEAR_MAP_COLOR, // 0
   CLEAR_MAP_COLOR, // 1
   CLEAR_MAP_COLOR, // 2
   CLEAR_MAP_COLOR, // 3
   CLEAR_MAP_COLOR, // 4
   CLEAR_MAP_COLOR, // 5
   CLEAR_MAP_COLOR, // 6
   CLEAR_MAP_COLOR, // 7
   CLEAR_MAP_COLOR, // 8
   CLEAR_MAP_COLOR, // 9
   CLEAR_MAP_COLOR, // 10
   CLEAR_MAP_COLOR, // 11
   CLEAR_MAP_COLOR, // 12
   224            , // 13
   254            , // 14
   254            , // 15
};

Background::Background(World& inWorld, ImageSheet& inImageSheet, MiniMap* inMap)
    : width(inWorld.GetStage().GetWidth() / GRID_SCALE)
    , height(inWorld.GetStage().GetHeight() / GRID_SCALE)
    , grid(new Cell [width * height])
    , starfield(inWorld)
    , imageSheet(inImageSheet)
    , map(inMap)
{
}

Background::~Background()
{
    delete [] grid;
}

void Background::Draw(View& view)
{
    starfield.Draw(view);

    Rect<s16> visible = view.GetVisibleRect();
    GridPosition start = WorldToGrid(visible.origin);
    int columCount = (visible.size.x + GRID_SCALE-1) / GRID_SCALE + 1;
    int rowCount   = (visible.size.y + GRID_SCALE-1) / GRID_SCALE + 1;

    u8 group, image;
    for (int y = 0; y < rowCount; y++ )
    {
        for (int x = 0; x < columCount; x++)
        {
            GridPosition pos = start + GridPosition(x,y);
            if(GetCell(pos, group, image))
            {
                view.DrawImage(GridToWorld(pos), imageSheet[group][image]);
            }
        }
    }
}

void Background::Clear()
{
    for(Cell* item = grid; item < grid + (width*height); item++)
    {
        item->valid = false;
    }
    if (map)
    {
        map->Clear(CLEAR_MAP_COLOR);
    }
}

void Background::ClearCell(GridPosition pos)
{
    int cell = GetCellIndex(pos);
    grid[cell].valid = false;
    map->Plot(pos, CLEAR_MAP_COLOR);
}

void Background::SetCell(GridPosition pos, u8 group, u8 index)
{
    int cell = GetCellIndex(pos);
    grid[cell] = {true, group, index};
    map->Plot(pos, GROUP_MAP_COLOR[group]);
}

bool Background::GetCell(GridPosition pos, u8& outGroup, u8& outIndex) const
{
    int cell = GetCellIndex(pos);
    outGroup = grid[cell].imageGroup;
    outIndex = grid[cell].imageIndex;
    return grid[cell].valid;

}