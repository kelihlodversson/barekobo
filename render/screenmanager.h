
#pragma once
#include <circle/bcmframebuffer.h>
#include <circle/types.h>
#include "util/vector.h"
#include "util/rect.h"
#include "util/vsync.h"

namespace hfh3
{

    /** Class for managing rendering to the screen using a double buffer
      * technique to avoid tearing and flickering artefacts.
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
        void DrawChar(const Vector<int>& at, char c, u8 color, const class Font& font);
        void DrawString(const Vector<int>& at, const char* string, u8 color, const class Font& font);
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

        /** Return the current frame rate in frames per second */
        unsigned GetFPS();

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

        /** Implements swapping of the active and visible frame.
          * Called by Present()
          */
        void Flip();

        /** Used by Present() to ensure the game is in sync with
          * the screen frame rate.
          */
        void WaitForVerticalSync();

        /** Bookkeeping method used to calculate the current FPS,
          * which should be equal to the physical screen update rate
          */
        void UpdateFrameStats();

        CBcmFrameBuffer	*framebuffer;
        int active; // Stores the currently active screen

        u8* bufferAddress;
        Vector<int> size;
        int stride;
        Rect<int> clip;

        unsigned lastSync;
        unsigned ticksPerFrame;
        unsigned frame;

        VSync    vsync;
    };

}
