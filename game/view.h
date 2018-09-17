#pragma once
#include "game/stage.h"
#include "render/screenmanager.h"



namespace hfh3
{
    class View
    {
    public:
        View(Stage& inStage, ScreenManager& inScreen);

        /** Translates a stage coordinate to a screen coordinate.
          * (no pun intended.)
          */
        Vector<int> StageToScreen(const Vector<int>& stageVector) const
        {
            return stage.WrapCoordinate(stageVector - screenOffset);
        }

        Rect<int> StageToScreen(const Rect<int>& stageRect) const
        {
            return {StageToScreen(stageRect.origin), stageRect.size};
        }

        class ScreenManager& GetScreen()
        {
            return screen;
        }
       
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
            screenOffset = stage.WrapCoordinate(offset - center);
        }

        Vector<int> GetOffset()
        {
            return screenOffset;
        }

        Rect<int> GetVisibleRect()
        {
            return {screenOffset, screen.GetSize()};
        }

        /** Returns true if the rect passed in is within the visible screen area
          */
        bool IsVisible(const Rect<int>& stageRect)
        {
            return GetVisibleRect().OverlapsMod(stageRect, stage.GetSize());
        }

        /** The following methods map a stage coordinate to a screen coordinate
          * before passing the argumetns to the screen manager
          */
        void DrawImage(const Vector<int>& at, const class Image& image);
        void DrawPixel(const Vector<int>& at, u8 color);
        void DrawRect(const Rect<int>& rect, u8 color);
    private:
        class Stage& stage;
        class ScreenManager& screen;
        Vector<int> screenOffset;
    };
}