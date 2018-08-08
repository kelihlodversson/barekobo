
#pragma once
#include <circle/bcmframebuffer.h>
#include <circle/types.h>
#include "util/vector.h"
#include "util/rect.h"

namespace hfh3
{

    /** Class for managing a list of images to be drawn on-screen.
      */
    class ScreenManager
    {
    public:
        typedef Vector<int> Coordinate;
        typedef Rect<int> ScreenRect;

        ScreenManager();
        ~ScreenManager();

        bool Initialize();
        void SetClip(const ScreenRect& rect);
        void ClearClip();
        void DrawImage(const Coordinate& at, const class Image& image);
        void DrawPixel(const Coordinate& at, u8 color);
        void DrawRect(const ScreenRect& rect, u8 color);
        void Clear(u8 color=0);

        int GetWidth() const { return size.x; }
        int GetHeight() const { return size.y; }
        const ScreenRect& GetBounds() const { return clip; }

        void Present();
    private:
        static const unsigned fbWidth = 400;
        static const unsigned fbHeight = 300;
        // Defined in the header to inline it.
        u8* GetPixelAddress(int x, int y)
        {
            return &bufferAddress[x + (y+active*size.y)*stride];
        }

        u8* GetPixelAddress(const Coordinate& at)
        {
            return GetPixelAddress(at.x, at.y);
        }

        CBcmFrameBuffer	*framebuffer;
        int active; // Stores the currently active screen

        u8* bufferAddress;
        Coordinate size;
        int stride;
        ScreenRect clip;
    };

}
