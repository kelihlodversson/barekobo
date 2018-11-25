#pragma once
#include "game/starfield.h"
#include "util/vector.h"
#include "util/rect.h"

namespace hfh3
{
    /** The background is a grid of non-moving elements that are drawn before any
      * sprites are rendered on-screen. Each cell of the grid is 16 by 16 pixels large.
      */
    class Background 
    {
    public:
        Background(class World& inWorld, class ImageSheet& inImageSheet, class MiniMap* inMap);
        ~Background();

        // Each grid cell is 16x16 world pixels
        static const s16 GRID_SCALE = 16;

        using GridPosition = Vector<u8>;
        using WorldPosition = Vector<s16>;

        static GridPosition WorldToGrid (const WorldPosition& worldPosition)
        {
            return  GridPosition(worldPosition / GRID_SCALE);
        }

        static WorldPosition GridToWorld(const GridPosition& grid) 
        {
            return WorldPosition(grid) * GRID_SCALE;
        }
     
        static Rect<s16> WorldRect(const GridPosition& grid)
        {
            return Rect<s16>(GridToWorld(grid), {GRID_SCALE, GRID_SCALE});
        }

        void Draw(class View& view);

        void Clear();
        void ClearCell(GridPosition pos);
        void SetCell(GridPosition pos, u8 group, u8 index);
        bool GetCell(GridPosition pos, u8& outGroup, u8& outIndex) const;

        void ClearCell(const WorldPosition& pos) 
        { 
            ClearCell(WorldToGrid(pos));
        }
        
        void SetCell(const WorldPosition& pos, u8 group, u8 index) 
        { 
            SetCell(WorldToGrid(pos), group, index);
        }
        
        bool GetCell(WorldPosition pos, u8& outGroup, u8& outIndex) const
        {
            return GetCell(WorldToGrid(pos), outGroup, outIndex);
        }

    private:

        int GetCellIndex(GridPosition& pos) const
        {
            pos.x %= width;
            pos.y %= height;
            return int(pos.y) * width + pos.x;
        }

        struct Cell 
        {
            Cell(bool isValid = false, u8 group=0, u8 index=0) 
                : valid(isValid)
                , imageGroup(group)
                , imageIndex(index) 
            {}

            bool valid;
            u8 imageGroup;
            u8 imageIndex;
        };

        s16 width, height;
        Cell* grid;
        Starfield starfield;
        class ImageSheet& imageSheet;
        class MiniMap* map;
    };
}