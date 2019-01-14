#pragma once
#include <circle/bcmframebuffer.h>
#include <circle/types.h>
#include "util/vector.h"
#include "util/rect.h"
#include "util/vsync.h"
#include "config.h"


#if CONFIG_DMA_FRAME_COPY
#   include <circle/dmachannel.h>
#endif


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

        struct Timer {
          unsigned ticksPerFrame;
          unsigned gameTicks;
          unsigned presentTicks;
        };

        bool Initialize();

        void DrawImage(const Vector<s16>& at, const class Image& image);
        void DrawPixel(const Vector<s16>& at, u8 color);
        void DrawRect(const Rect<s16>& rect, u8 color);
        void DrawChar(const Vector<s16>& at, char c, u8 color, const class Font& font);
        void DrawString(const Vector<s16>& at, const char* string, u8 color, const class Font& font);
        void Clear(u8 color=0);

        /** Returns an image object representing a rectangle of screen area.
          * can be used as a source to DrawImage to copy parts of the screen
          * or the previous frame.
          *
          * Note that partially overlapping rects from the same frame buffer
          * are currently not supported.
          */
        class Image GetImageRect(const Rect<s16>& rect, frame_t source = VISIBLE);

        int GetWidth() const { return size.x; }
        int GetHeight() const { return size.y; }
        Vector<s16> GetSize() const { return size; }
        Rect<s16> GetScreenRect() const { return Rect<s16>(Vector<s16>(), size); }

        /** Limits output to a rectangle
          * The rectangle coordinates are in physical screen coordinates
          */
        void SetClip(const Rect<s16>& rect);
        const Rect<s16>& GetClip() const { return clip; }
        void ClearClip();

        /** Present the working image buffer
          * Use this to show the current frame after rendering.
          */
        void Present();

        /** Wait until it is okay to write to start drawing to the frame buffer.
          * This must be called at least once after calling Present().
          */
        void WaitForScreenBufferReady();

        /** Return the current frame rate in frames per second */
        unsigned GetFPS();

        /** Return the amount of time spent between each call to Present
          * in percent of frame time. */
        unsigned GetGameTimePCT();

        /** Return the amount of CPU time spent copying the current frame to
          * the GPU in percent of frame time.
          * If CONFIG_GPU_PAGE_FLIPPING is 1 this will be zero as
          * no extra copying is done. */
        unsigned GetFlipTimePCT();

        /** Clears the sum, min and max timers and the frame coutner*/
        void ClearTimers();

        /** Get a dump of the current stats. Returns the number of frames since
          * last reset and the values in the passed in references. */
        unsigned GetTimers(Timer& outSum, Timer& outMin, Timer& outMax);


        /** Return the number of frames missed due to too much time spent between
          * calls to Present
          */
        unsigned GetMissedFrames() 
        { 
          return vsync.GetMissed(); 
        }

    private:
        static const unsigned fbWidth = 640;
        static const unsigned fbHeight = 400;

        // Defined in the header to inline it.
        u8* GetPixelAddress(int x, int y, frame_t frame = ACTIVE)
        {
#if CONFIG_GPU_PAGE_FLIPPING
            // The frame buffer is twice as tall as the visible area.
            // Offset the y coordinate based on the currently active
            // frame.
            int frame_offset = size.y * (frame == ACTIVE ? active : ((active + 1) % 2));
            return &bufferAddress[x + (y+frame_offset)*stride];
#else
            u8* buffer = (frame == ACTIVE ? renderBuffer : bufferAddress);
            return &buffer[x + y*stride];
#endif
        }

        u8* GetPixelAddress(const Vector<s16>& at, frame_t frame = ACTIVE)
        {
            return GetPixelAddress(at.x, at.y, frame);
        }

#if CONFIG_GPU_PAGE_FLIPPING
        /** Implements swapping of the active and visible frame.
          * Called by Present()
          */
        void Flip();
#else
        /** Presents the render buffer by copying the contents to
          * the frame buffer
          */
        void CopyFrameData();
#endif

        /** Used by Present() to ensure the game is in sync with
          * the screen frame rate.
          */
        void WaitForVerticalSync();

        /** Bookkeeping methods used to calculate the current FPS,
          * which should be equal to the physical screen update rate
          */
        void UpdateStatsPreSync();
        void UpdateStatsPostSync();
        void UpdateStatsPostCopy();

        CBcmFrameBuffer	*framebuffer;
#if CONFIG_DMA_FRAME_COPY
        CDMAChannel dma;
#endif
#if CONFIG_GPU_PAGE_FLIPPING
        int active; // Stores the currently active screen
#else
        u8* renderBuffer;
#endif
        u8* bufferAddress;
        Vector<s16> size;
        int stride;
        Rect<s16> clip;

        unsigned frame;
        unsigned lastSync;
        unsigned lastPresent;
        Timer    current, prev;
        Timer    max, min, sum;
        VSync    vsync;
    };

}
