#include <circle/util.h>
#include <circle/sched/scheduler.h>
#include <climits>

#include "graphics/sprite_data.h"
#include "render/screenmanager.h"
#include "render/image.h"
#include "render/font.h"

#include "util/log.h"

#if CONFIG_NEON_RENDER
#include <arm_neon.h>
#endif

using namespace hfh3;

/** Integer division with rounding to the nearest integer.
  */
static inline unsigned DivRound(unsigned a, unsigned b)
{
    return (a / b) + ((a % b) >= (b / 2)?1:0);
}

ScreenManager::ScreenManager()
    : framebuffer(nullptr)
#if CONFIG_DMA_FRAME_COPY
    , dma(DMA_CHANNEL_SCREEN)
#endif
#if CONFIG_GPU_PAGE_FLIPPING
    , active(0)
#else
    , renderBuffer(nullptr)
#endif
    , bufferAddress(nullptr)
    , size(0,0)
    , stride(0)
    , clip()
    , lastSync(0)
    , frame(0)
{
    current = {0,0,0};
    ClearTimers();
}

ScreenManager::~ScreenManager()
{
#if CONFIG_GPU_PAGE_FLIPPING == 0
    delete[] renderBuffer;
    renderBuffer = nullptr;
#endif

    delete framebuffer;
    framebuffer = nullptr;
    bufferAddress = nullptr;
}

bool ScreenManager::Initialize()
{
#if CONFIG_GPU_PAGE_FLIPPING
    const unsigned virtualHeight = fbHeight * 2;
    active = 1;
#else
    const unsigned virtualHeight = fbHeight;
#endif

    framebuffer = new CBcmFrameBuffer(fbWidth, fbHeight, 8, fbWidth, virtualHeight);
    for(int i = 0; i<256; i++)
    {
        framebuffer->SetPalette32(i, sprites_palette[i]);
    }

	if (!framebuffer->Initialize())
    {
		return false;
	}

    framebuffer->SetVirtualOffset(0, 0);
    size = Vector<s16>(framebuffer->GetWidth(), framebuffer->GetHeight());
    stride = framebuffer->GetPitch();
    bufferAddress = reinterpret_cast<u8*>(framebuffer->GetBuffer());
    ClearClip();

    if (!vsync.Initialize())
    {
        return false;
    }

#if CONFIG_GPU_PAGE_FLIPPING == 0
    renderBuffer = new u8[size.y*stride];
    Clear();
#endif

#if CONFIG_DMA_FRAME_COPY
    dma.SetupMemCopy2D(bufferAddress, renderBuffer, size.x, size.y, stride-size.x);
#endif

    return bufferAddress != nullptr;
}

// Swaps the active and visible frames and waits for vertical sync before returning.
void ScreenManager::Present()
{
    UpdateStatsPreSync();
#if CONFIG_GPU_PAGE_FLIPPING
    // Flip the logical and physical frame buffers
    // The GPU won't do the switch until the vertical sync period, so
    // we'll have to request it BEFORE we wait for VSync
    Flip();
#endif

    WaitForVerticalSync();

    // Update frame rate statistics, etc.
    UpdateStatsPostSync();
#if CONFIG_GPU_PAGE_FLIPPING == 0
    // Copy the logical frame buffer to the physical
    // This has to happen immediately AFTER waiting for VSYNC so it will
    // happen during the vertical blank period.
    CopyFrameData();
#endif
    UpdateStatsPostCopy();
}

#if CONFIG_GPU_PAGE_FLIPPING
// Swaps the visible frames
void ScreenManager::Flip()
{
    if(!framebuffer)
    {
        return;
    }

    // Request the GPU to switch the visible view port to the active screen.
    // Note that the GPU will not actually perform the switch until during the next
    // vertical sync.
    framebuffer->SetVirtualOffset(0, active*GetHeight());

    // Swap the active screen so future draw commands will keep going to the off-screen buffer
    active = (active + 1) % 2;
}
#else
// Copy the frame data to the screen
void ScreenManager::CopyFrameData()
{
    if(!framebuffer)
    {
        return;
    }

    assert(bufferAddress);
    assert(renderBuffer);
#if CONFIG_DMA_FRAME_COPY
    dma.Start();
    dma.Wait();
#else
    memcpy(bufferAddress, renderBuffer, size.y * stride);
#endif
}
#endif

void ScreenManager::WaitForVerticalSync()
{
    vsync.Wait();
}

void ScreenManager::UpdateStatsPreSync()
{
    CTimer *timer = CTimer::Get();
    // Get the current timer tick count
    const unsigned currentTick = timer->GetClockTicks();
    current.gameTicks = currentTick - lastSync - current.presentTicks;
}

#define UPDATE_SUM(field) sum.field += current.field
#define UPDATE_MAX(field) if (max.field < current.field) max.field = current.field
#define UPDATE_MIN(field) if (min.field > current.field) min.field = current.field

void ScreenManager::UpdateStatsPostSync()
{
    CTimer *timer = CTimer::Get();
    // Get the current timer tick count
    const unsigned currentTick = timer->GetClockTicks();

    // Calculate the time from one vsync to the next
    current.ticksPerFrame = currentTick - lastSync;

    // save the time stamp of the start of the current frame
    lastSync = currentTick;

    UPDATE_SUM(gameTicks);
    UPDATE_SUM(presentTicks);
    UPDATE_SUM(ticksPerFrame);

    UPDATE_MAX(gameTicks);
    UPDATE_MAX(presentTicks);
    UPDATE_MAX(ticksPerFrame);
    
    UPDATE_MIN(gameTicks);
    UPDATE_MIN(presentTicks);
    UPDATE_MIN(ticksPerFrame);

    // Update current frame number
    frame ++;
}

unsigned ScreenManager::GetTimers(Timer& outSum, Timer& outMin, Timer& outMax)
{
    outSum = sum;
    outMin = min;
    outMax = max;
    return frame;
}

void ScreenManager::ClearTimers()
{
    sum = max = {0,0,0};
    frame = 0;
    min = {UINT_MAX,UINT_MAX,UINT_MAX};
}

void ScreenManager::UpdateStatsPostCopy()
{
    CTimer *timer = CTimer::Get();
    // Get the current timer tick count
    const unsigned currentTick = timer->GetClockTicks();
    current.presentTicks = currentTick - lastSync;
}

unsigned ScreenManager::GetFPS()
{
    return DivRound(CLOCKHZ, current.ticksPerFrame);
}

unsigned ScreenManager::GetGameTimePCT()
{
    return DivRound(100 * current.gameTicks, current.ticksPerFrame);
}

unsigned ScreenManager::GetFlipTimePCT()
{
    return DivRound(100 * current.presentTicks, current.ticksPerFrame);
}

void ScreenManager::SetClip(const Rect<s16>& rect)
{
    clip = GetScreenRect() & rect;
}

void ScreenManager::ClearClip()
{
    clip = GetScreenRect();
}

void ScreenManager::DrawPixel(const Vector<s16>& at, u8 color)
{
    if(!bufferAddress)
    {
        return;
    }
    if ( !clip.Contains(at) )
    {
        return;
    }
    *GetPixelAddress(at) = color;
}

void ScreenManager::DrawRect(const Rect<s16>& rect, u8 color)
{
    if(!bufferAddress)
    {
        return;
    }
    // Clip the rect to the frame buffer
    Rect<s16> clipped = clip & rect;
    if(!clipped.IsValid())
    {
        return;
    }

    // If the rectangle fills the entire width of the screen we can draw it with a single memset
    if( clipped.Width() == GetWidth())
    {
        assert(clipped.Left() == 0);
        memset(GetPixelAddress(clipped.origin), color, stride * clipped.Height());
    }
    // Else, we need to draw each scan line separately
    else
    {
        for (int row = clipped.Top(); row < clipped.Bottom(); row++)
        {
            memset(GetPixelAddress(clipped.Left(),row), color, clipped.Width());
        }
    }
}

void ScreenManager::Clear(u8 color)
{
    DrawRect(clip, color);
}

Image ScreenManager::GetImageRect(const Rect<s16>& rect, frame_t source)
{
    // Note that the Image returned refers directly to the framebuffer
    // data and it will not update when swapping buffers.
    u8* image_pixels = GetPixelAddress(rect.origin, source);
    return Image(image_pixels, rect.size.x, rect.size.y, -1, stride);
}

void ScreenManager::DrawImage(const Vector<s16>& at, const Image& image)
{
    if(!bufferAddress)
    {
        return;
    }

    Rect<s16> clipped = clip & Rect<s16>(at, image.GetSize());

    if(!clipped.IsValid())
    {
        return;
    }

    int image_min_x = Max(clipped.Left() - at.x , 0);
    int image_min_y = Max(clipped.Top() - at.y , 0);
    int image_max_y = image_min_y + clipped.Height();

    // if the image has no transparent pixels, we can simply memcpy each row
    int transparent = image.GetTransparent();
    if(transparent < 0)
    {
        for (int image_y = image_min_y; image_y < image_max_y; image_y++)
        {
            memcpy(GetPixelAddress(at.x+image_min_x, at.y+image_y), image.GetPixelAddress(image_min_x, image_y), clipped.Width());
        }
    }
    // else we have to compare each pixel to the transparent value before plotting it
    else
    {
        const int width = clipped.Width();
        const u8 tcolor = (u8)transparent;
#if CONFIG_NEON_RENDER
        // The neon intrinsics below copy 16 pixels in one chunk
        const int neonWidth = (width/sizeof(uint8x16_t))*sizeof(uint8x16_t);
        const uint8x16_t tvector = vmovq_n_u8(tcolor); // 16x transparent color
#endif
        for (int image_y = image_min_y; image_y < image_max_y; image_y++)
        {
            u8* dstRow = GetPixelAddress(at.x+image_min_x, at.y+image_y);
            const u8* srcRow = image.GetPixelAddress(image_min_x, image_y);
            int i=0;
#if CONFIG_NEON_RENDER
            // There is no attempt of aligning the data, which may explain why
            // there is little if no speed gain compared to the naive method.
            for(; i < neonWidth; i+= sizeof(uint8x16_t))
            {
                // Grab 16 pixels of source and destination data
                uint8x16_t dstChunk = vld1q_u8(&dstRow[i]);
                uint8x16_t srcChunk = vld1q_u8(&srcRow[i]);

                // Compare 16 pixesl of source data against the transparent color
                uint8x16_t transparent = vceqq_u8(srcChunk, tvector);

                // Select source where the color was not transparent and dst where it was
                // and the result to memory
                vst1q_u8(&dstRow[i],vbslq_u8(transparent, dstChunk, srcChunk));
            }

            // Since the clip areay may not be divisible by 16, we iterate through
            // the remainder one pixel at a time.
#endif
            for(; i < width; i++)
            {
                if (srcRow[i] != tcolor)
                {
                    dstRow[i] = srcRow[i];
                }
            }
        }
    }
}

void ScreenManager::DrawChar(const Vector<s16>& at, char c, u8 color, const Font& font)
{
    if(!bufferAddress)
    {
        return;
    }

    Rect<s16> clipped = clip & Rect<s16>(at, font.GetSize(c));
    if(!clipped.IsValid())
    {
        return;
    }

    int cell_min_x = Max(clipped.Left() - at.x , 0);
    int cell_min_y = Max(clipped.Top() - at.y , 0);
    int cell_max_y = cell_min_y + clipped.Height();
    int cell_max_x = cell_min_x + clipped.Width();
    for (int cell_y = cell_min_y; cell_y < cell_max_y; cell_y++)
    {
        u8* dst = GetPixelAddress(clipped.Left(), at.y+cell_y);
        for(int cell_x = cell_min_x; cell_x < cell_max_x; cell_x++)
        {
            if (font.GetPixel(c, cell_x, cell_y))
            {
                *dst = color;
            }
            dst++;
        }
    }
}

void ScreenManager::DrawString(const Vector<s16>& at, const char* string, u8 color, const Font& font)
{
    Vector<s16> current = at;
    for(int i=0; string[i]; i++)
    {
        if(string[i] == '\n')
        {
            current.x = at.x;
            current.y += font.GetHeight();
        }
        else
        {
            DrawChar(current, string[i], color, font);
            current.x += font.GetSize(string[i]).x;
        }
    }
}
