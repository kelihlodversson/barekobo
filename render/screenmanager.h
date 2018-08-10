
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
        ScreenManager();
        ~ScreenManager();

        bool Initialize();

        void DrawImage(const Vector<int>& at, const class Image& image);
        void DrawPixel(const Vector<int>& at, u8 color);
        void DrawRect(const Rect<int>& rect, u8 color);
        void Clear(u8 color=0);

        int GetWidth() const { return size.x; }
        int GetHeight() const { return size.y; }
        Vector<int> GetSize() const { return size; }

        /** Limits output to a rectangle
        * The rectangle coordinates are in physical screen coordinates
        */
        void SetClip(const Rect<int>& rect);
        const Rect<int>& GetClip() const { return clip; }
        void ClearClip();

        /** Present the working image buffer
          * Use this to show the current frame after rendering.
          */
        void Present();
    private:
        static const unsigned fbWidth = 400;
        static const unsigned fbHeight = 300;
        // Defined in the header to inline it.
        u8* GetPixelAddress(int x, int y)
        {
            return &bufferAddress[x + (y+active*size.y)*stride];
        }

        u8* GetPixelAddress(const Vector<int>& at)
        {
            return GetPixelAddress(at.x, at.y);
        }

        CBcmFrameBuffer	*framebuffer;
        int active; // Stores the currently active screen

        u8* bufferAddress;
        Vector<int> size;
        int stride;
        Rect<int> clip;
    };

}
