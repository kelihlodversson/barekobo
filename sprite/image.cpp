#include "sprite/image.h"

using namespace hfh3;

Image::Image(u8* inData, unsigned inWidth, unsigned inHeight, int inTransparent, unsigned inRowStride)
    : imageData(inData)
    , width(inWidth)
    , height(inHeight)
    , stride(inRowStride)
    , transparent(inTransparent)
{
    // Assume that if the stride is unspecified, it's equal to the width of
    // the image.
    if(stride == 0)
    {
        stride = width;
    }
}
