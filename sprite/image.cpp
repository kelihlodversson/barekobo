#include "sprite/image.h"

using namespace hfh3;

Image::Image(u8* inData, int inWidth, int inHeight, int inTransparent, int inRowStride)
    : imageData(inData)
    , size(inWidth, inHeight)
    , stride(inRowStride)
    , transparent(inTransparent)
{
    // Assume that if the stride is unspecified, it's equal to the width of
    // the image.
    if(stride == 0)
    {
        stride = size.x;
    }
    if(transparent >= 0)
    {
        VerifyTransparency();
    }
}

void Image::VerifyTransparency()
{
    // Verify that the image does contain at least one transparent pixel,
    // and set transparent to -1 if not.
    for(int y=0; y<size.y; y++)
    {
        const u8* row = GetPixelAddress(0,y);
        for(int x=0; x<size.x; x++)
        {
            // If we find just one transparent pixel,
            // early out and keep the value.
            if (row[x] == transparent)
            {
                return;
            }
        }
    }
    // If we reach this, there are no transparent pixels in the image,
    // set the transparent value to -1 to indicate that.
    transparent = -1;
}
