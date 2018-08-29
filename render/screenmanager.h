
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

        /* Used to specify which frame to use as source for GetImageRect.
         * ACTIVE is the frame that is being rendered. VISIBLE is the previously
         * active frame currently being shown to the user
         */
        enum frame_t
        {
            ACTIVE,
            VISIBLE
        };

        bool Initialize();

        void DrawImage(const Vector<int>& at, const class Image& image);
        void DrawPixel(const Vector<int>& at, u8 color);
        void DrawRect(const Rect<int>& rect, u8 color);
        void DrawChar(const Vector<int>& at, char c, u8 color, const class Font& font);
        void DrawString(const Vector<int>& at, const char* string, u8 color, const class Font& font);
        void Clear(u8 color=0);

        /** Returns an image object representing a rectangle of screen area.
          * can be used as a source to DrawImage to copy parts of the screen
          * or the previous frame.
          *
          * Note that partially overlapping rects from the same frame buffer
          * are currently not supported.
          */
        class Image GetImageRect(const Rect<int>& rect, frame_t source = VISIBLE);

        int GetWidth() const { return size.x; }
        int GetHeight() const { return size.y; }
        Vector<int> GetSize() const { return size; }
        Rect<int> GetScreenRect() const { return Rect<int>(Vector<int>(), size); }

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

        /** Return the number of frames missed due to too much time spent between
          * calls to Present
          */
        unsigned GetMissedFrames() { return vsync.GetMissed(); }

    private:
        static const unsigned fbWidth = 640;
        static const unsigned fbHeight = 400;

        // Defined in the header to inline it.
        u8* GetPixelAddress(int x, int y, frame_t frame = ACTIVE)
        {
            // The frame buffer is twice as tall as the visible area.
            // Offset the y coordinate based on the currently active
            // frame.
            int frame_offset = size.y * (frame == ACTIVE ? active : ((active + 1) % 2));
            return &bufferAddress[x + (y+frame_offset)*stride];
        }

        u8* GetPixelAddress(const Vector<int>& at, frame_t frame = ACTIVE)
        {
            return GetPixelAddress(at.x, at.y, frame);
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
