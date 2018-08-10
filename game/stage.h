#pragma once
#include "sprite/screen_manager.h"

namespace hfh3
{

    /** Represents a stage of a given size, which must be a power of two
      * for performance reasons.
      * The stage will wrap around at the edges.
      */
    class Stage
    {
    public:
        /** Creates a new stage of size width x height
          * The width and heights must be power of two.
          */
        Stage(int width, int height, class ScreenManager& inScreen);

        /** Changes the virtual offset added to any coordinates passed in
          * via DrawImage, DrawRect and DrawPixel
          */
        void SetOffset(const Vector<int>& offset)
        {
            screenOffset = offset;
        }

        /** Same as setOffset, but moves the center of the screen
          * of the top left corner.
          */
        void SetCenterOffset(const Vector<int>& offset)
        {
            Vector<int> center = screen.GetSize()/2;
            screenOffset = offset - center;
        }

        Vector<int> GetOffset()
        {
            return screenOffset;
        }

        /** The following methods map a stage coordinate to a screen coordinate
          * before passing the argumetns to the screen manager
          */
        void DrawImage(const Vector<int>& at, const class Image& image);
        void DrawPixel(const Vector<int>& at, u8 color);
        void DrawRect(const Rect<int>& rect, u8 color);

        /** Modifies the vector reference passed in so it is within
          * the play area.
          */
        Vector<int> WrapCoordinate(const Vector<int>& vector) const
        {
            return {vector.x & maskX, vector.y & maskY};
        }

        /** Translates a stage coordinate to a screen coordinate.
          * (no pun intended.)
          */
        Vector<int> StageToScreen(const Vector<int>& stage) const
        {
            return WrapCoordinate(stage - screenOffset);
        }

        Rect<int> StageToScreen(const Rect<int>& stageRect) const
        {
            return {StageToScreen(stageRect.origin), stageRect.size};
        }

        class ScreenManager& GetScreen()
        {
            return screen;
        }

        int GetWidth() { return maskX+1; }
        int GetHeight() { return maskY+1; }
        Vector<int> GetSize() { return {GetWidth(), GetHeight()}; }

    private:
        class ScreenManager& screen;
        int maskX;
        int maskY;
        Vector<int> screenOffset;


    };
}
