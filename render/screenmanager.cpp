#include <circle/util.h>
#include <circle/sched/scheduler.h>

#include "graphics/sprite_data.h"
#include "render/screenmanager.h"
#include "render/image.h"
#include "render/font.h"

#include "util/log.h"

using namespace hfh3;

ScreenManager::ScreenManager()
    : framebuffer(nullptr)
    , active(0)
    , bufferAddress(nullptr)
    , size(0,0)
    , stride(0)
    , clip()
    , lastSync(0)
    , ticksPerFrame(0)
    , frame(0)
{
}

ScreenManager::~ScreenManager()
{
    delete framebuffer;
    framebuffer = nullptr;
    bufferAddress = nullptr;
}

bool ScreenManager::Initialize()
{
    framebuffer = new CBcmFrameBuffer(fbWidth, fbHeight, 8, fbWidth, fbHeight*2);
    for(int i = 0; i<256; i++)
    {
        framebuffer->SetPalette32(i, sprites_palette[i]);
    }

	if (!framebuffer->Initialize())
    {
		return false;
	}

    framebuffer->SetVirtualOffset(0, 0);
    active = 1;
    size = Vector<int>(framebuffer->GetWidth(), framebuffer->GetHeight());
    stride = framebuffer->GetPitch();
    bufferAddress = reinterpret_cast<u8*>(framebuffer->GetBuffer());
    ClearClip();

    if (!vsync.Initialize())
    {
        return false;
    }

    return bufferAddress != nullptr;
}

// Swaps the active and visible frames and waits for vertical sync before returning.
void ScreenManager::Present()
{
    // Flip the logical and physical frame buffers
    Flip();
    // The switch won't happen until the vertical sync period, so
    // we'll have to wait until it has happened.
    WaitForVerticalSync();
    // Update frame rate statistics, etc.
    UpdateFrameStats();
}

// Swaps the visible frames immediately without synchronizing
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

void ScreenManager::WaitForVerticalSync()
{
    vsync.Wait();
}

void ScreenManager::UpdateFrameStats()
{
    CTimer *timer = CTimer::Get();
    // Get the current timer tick count
    unsigned currentTick = timer->GetClockTicks();

    // Calculate the time from one vsync to the next
    ticksPerFrame = currentTick - lastSync;

    // save the time stamp of the start of the current frame
    lastSync = currentTick;

    // Update current frame number
    frame ++;
}

unsigned ScreenManager::GetFPS()
{
    // Integer division, rounding up if the remainder is more than half the divisor
    return CLOCKHZ / ticksPerFrame + (CLOCKHZ % ticksPerFrame >= ticksPerFrame/2 ? 1:0);
}

void ScreenManager::SetClip(const Rect<int>& rect)
{
    clip = GetScreenRect() & rect;
}

void ScreenManager::ClearClip()
{
    clip = GetScreenRect();
}

void ScreenManager::DrawPixel(const Vector<int>& at, u8 color)
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

void ScreenManager::DrawRect(const Rect<int>& rect, u8 color)
{
    if(!bufferAddress)
    {
        return;
    }
    // Clip the rect to the frame buffer
    Rect<int> clipped = clip & rect;
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

Image ScreenManager::GetImageRect(const Rect<int>& rect, frame_t source)
{
    // Note that the Image returned refers directly to the framebuffer
    // data and it will not update when swapping buffers.
    u8* image_pixels = GetPixelAddress(rect.origin, source);
    return Image(image_pixels, rect.size.x, rect.size.y, -1, stride);
}

void ScreenManager::DrawImage(const Vector<int>& at, const Image& image)
{
    if(!bufferAddress)
    {
        return;
    }

    Rect<int> clipped = clip & Rect<int>(at, image.GetSize());

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
        const u8 tcolor = (u8)transparent;
        for (int image_y = image_min_y; image_y < image_max_y; image_y++)
        {
            u8* dstRow = GetPixelAddress(at.x+image_min_x, at.y+image_y);
            const u8* srcRow = image.GetPixelAddress(image_min_x, image_y);
            for(int i = 0; i < clipped.Width(); i++)
            {
                if (srcRow[i] != tcolor)
                {
                    dstRow[i] = srcRow[i];
                }
            }
        }
    }
}

void ScreenManager::DrawChar(const Vector<int>& at, char c, u8 color, const Font& font)
{
    if(!bufferAddress)
    {
        return;
    }

    Rect<int> clipped = clip & Rect<int>(at, font.GetSize(c));
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

void ScreenManager::DrawString(const Vector<int>& at, const char* string, u8 color, const Font& font)
{
    Vector<int> current = at;
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
