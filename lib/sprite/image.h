#pragma once
#include <circle/bcmframebuffer.h>
#include <circle/types.h>

namespace hfh3
{

    /** An image defines an area of pixels to be rendered to the screen.
      * A single color can be defined as transparent, meaning that existing
      * content in the framebuffer will be left untouched.
      *
      * Note that the Image class only keeps a reference to the underlying
      * pixel buffer and that the actual rendering is handled by the SpriteManager.
      */
    class Image
    {
    public:
        Image(u8* inData, unsigned inWidth, unsigned inHeight, int inTransparent=-1, unsigned inRowStride=0);

    private:
        u8* GetPixelAddress(int x, int y)
        {
            return &imageData[x + y*stride];
        }

        u8	*imageData;
        unsigned height;
        unsigned width;
        unsigned stride;
        int transparent;

        friend class SpriteManager;
    };
}
