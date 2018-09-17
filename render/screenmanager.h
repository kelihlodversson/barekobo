#pragma once
#include <circle/bcmframebuffer.h>
#include <circle/types.h>
#include "util/vector.h"
#include "util/rect.h"
#include "util/vsync.h"

// Set CONFIG_GPU_PAGE_FLIPPING to 1 to allocate both the active and visible
// frame buffers in GPU memory. The GPU will be used to page between them.
// If set to 0 rendering will be to a temporary buffer in CPU ram and
// page flipping will be implemented by memcpy-ing the contents to GPU ram.
// Due to CPU memory being cached and GPU not, the latter is actually
// around 3 times faster than the former despite the final memcpy.
#ifndef CONFIG_GPU_PAGE_FLIPPING
#   define CONFIG_GPU_PAGE_FLIPPING 0
#endif

// If CONFIG_DMA_FRAME_COPY is set (and CONFIG_GPU_PAGE_FLIPPING is not set)
// the final memory copy from CPU to GPU ram is done using a DMA transfer instead
// of using the CPU. This is approximately 5 times slower thant the CPU methhod,
// but further refinements could be done to allow non-rendering code to run
// in parallel with the copying.
#ifndef CONFIG_DMA_FRAME_COPY
#   define CONFIG_DMA_FRAME_COPY 0
#endif

// If set to 1, the ScreenManager class will use neon intrinsics to copy 16 pixels
// at a time to the destination using NEON intrinsics. This is currently disabled,
// as it seems to have a neglible effect.
#ifndef CONFIG_NEON_RENDER
#   define CONFIG_NEON_RENDER 0
#endif

#if CONFIG_GPU_PAGE_FLIPPING && CONFIG_DMA_FRAME_COPY
#   error "CONFIG_GPU_PAGE_FLIPPING and CONFIG_DMA_FRAME_COPY are mutually exclusive"
#endif


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

        /** Return the amount of time spent between each call to Present
          * in percent of frame time. */
        unsigned GetGameTimePCT();

        /** Return the amount of CPU time spent copying the current frame to
          * the GPU in percent of frame time.
          * If CONFIG_GPU_PAGE_FLIPPING is 1 this will be zero as
          * no extra copying is done. */
        unsigned GetFlipTimePCT();

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

        u8* GetPixelAddress(const Vector<int>& at, frame_t frame = ACTIVE)
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

        /** Bookkeeping method used to calculate the current FPS,
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
        Vector<int> size;
        int stride;
        Rect<int> clip;

        unsigned lastSync;
        unsigned ticksPerFrame;
        unsigned gameTicks;
        unsigned presentTicks;
        unsigned frame;
        VSync    vsync;
    };

}
