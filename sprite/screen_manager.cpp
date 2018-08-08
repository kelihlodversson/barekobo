#include <circle/util.h>
#include "graphics/sprite_data.h"
#include "sprite/screen_manager.h"
#include "sprite/image.h"

using namespace hfh3;



ScreenManager::ScreenManager()
    : framebuffer(nullptr)
    , active(0)
    , bufferAddress(nullptr)
    , width(0)
    , height(0)
    , stride(0)
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
    width = framebuffer->GetWidth();
    height = framebuffer->GetHeight();
    stride = framebuffer->GetPitch();
    bufferAddress = reinterpret_cast<u8*>(framebuffer->GetBuffer());
    return bufferAddress != nullptr;
}

// Draws the current frame and presents it.
void ScreenManager::Present()
{
    if(!framebuffer)
    {
        return;
    }

    framebuffer->SetVirtualOffset(0, active*height);
    framebuffer->WaitForVerticalSync();
    active = (active + 1) % 2; // Swap the active screen
}

void ScreenManager::DrawPixel(int x, int y, u8 color)
{
    if(!bufferAddress)
    {
        return;
    }
    if ( x < 0 || y < 0 || x > width || y > width)
    {
        return;
    }
    *GetPixelAddress(x,y) = color;
}

void ScreenManager::DrawRect(int x, int y, int w, int h, u8 color)
{
    if(!bufferAddress)
    {
        return;
    }
    // Clip the rect to the frame buffer
    if (w + x > width)
    {
        w = width - x;
    }
    if (h + y > height)
    {
        h = height - y;
    }
    if (x < 0)
    {
        w += x;
        x = 0;
    }
    if (y < 0)
    {
        h += y;
        y = 0;
    }

    int max_y = y+h;

    // If the rectangle fills the entire width of the screen we can draw it with a single memset
    if( w == width )
    {
        assert(x == 0);
        memset(GetPixelAddress(0,y), color, stride * h);
    }
    // Else, we need to draw each scan line separately
    else
    {
        for (int row = y; y < max_y; row++)
        {
            memset(GetPixelAddress(x,row), color, w);
        }
    }
}

void ScreenManager::Clear(u8 color)
{
    if(!bufferAddress)
    {
        return;
    }
    memset(GetPixelAddress(0,0), color, stride * height);
}

void ScreenManager::DrawImage(int x, int y, Image& image)
{
    if(!bufferAddress)
    {
        return;
    }
    int image_min_x = 0;
    int image_min_y = 0;
    int image_max_x = image.width;
    int image_max_y = image.height;

    // Clip image to the frame buffer
    if (image_max_x + x > width)
    {
        image_max_x = width - x;
    }
    if (image_max_y + y > height)
    {
        image_max_y = height - y;
    }
    if (x < 0)
    {
        image_min_x =  -x;
    }
    if (y < 0)
    {
        image_min_y =  -y;
    }

    int row_width = image_max_x - image_min_x;

    // if the image has no transparent pixels, we can simply memcpy each row
    if(image.transparent < 0)
    {
        for (int image_y = image_min_y; image_y < image_max_y; image_y++)
        {
            memcpy(GetPixelAddress(x+image_min_x, y+image_y), image.GetPixelAddress(image_min_x, image_y), row_width);
        }
    }
    // else we have to compare each pixel to the transparent value before plotting it
    else
    {
        for (int image_y = image_min_y; image_y < image_max_y; image_y++)
        {
            u8* dstRow = GetPixelAddress(x+image_min_x, y+image_y);
            u8* srcRow = image.GetPixelAddress(image_min_x, image_y);
            const u8 transparent = (u8)image.transparent;
            for(int i = 0; i < row_width; i++)
            {
                if (srcRow[i] != transparent)
                {
                    dstRow[i] = srcRow[i];
                }
            }
        }
    }
}
