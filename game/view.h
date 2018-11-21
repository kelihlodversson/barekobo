#pragma once
#include "game/stage.h"
#include "render/screenmanager.h"



namespace hfh3
{
    class View
    {
    public:
        View(Stage& inStage, ScreenManager& inScreen);

        class ScreenManager& GetScreen()
        {
            return screen;
        }
       
        /** Changes the virtual offset added to any coordinates passed in
          * via DrawImage, DrawRect and DrawPixel
          */
        void SetOffset(const Vector<s16>& offset)
        {
            screenOffset = offset;
        }

        /** Same as setOffset, but moves the center of the screen
          * of the top left corner.
          */
        void SetCenterOffset(const Vector<s16>& offset)
        {
            Vector<s16> center = screen.GetClip().Center();
            screenOffset = stage.WrapCoordinate(offset - center);
        }

        Vector<s16> GetOffset()
        {
            return screenOffset;
        }

        Rect<s16> GetVisibleRect()
        {
            Rect<s16> clip = screen.GetClip();
            clip.origin += screenOffset;
            return clip;
        }

        /** Returns true if the rect passed in is within the visible screen area
          */
        bool IsVisible(const Rect<s16>& stageRect)
        {
            return GetVisibleRect().OverlapsMod(stageRect, stage.GetSize());
        }

        /** The following methods map a stage coordinate to a screen coordinate
          * before passing the argumetns to the screen manager
          */
        void DrawImage(const Vector<s16>& at, const class Image& image);
        void DrawPixel(const Vector<s16>& at, u8 color);
        void DrawRect(const Rect<s16>& rect, u8 color);
    private:
        class Stage& stage;
        class ScreenManager& screen;
        Vector<s16> screenOffset;
    };
}