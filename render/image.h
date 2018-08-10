#pragma once
#include <circle/types.h>
#include "util/vector.h"

namespace hfh3
{

    /** An image defines an area of pixels to be rendered to the screen.
      * A single color can be defined as transparent, meaning that existing
      * content in the framebuffer will be left untouched.
      *
      * Note that the Image class only keeps a reference to the underlying
      * pixel buffer and that the actual rendering is handled by the ScreenManager.
      */
    class Image
    {
    public:
        Image(u8* inData, int inWidth, int inHeight, int inTransparent=-1, int inRowStride=0);

        const Vector<int>& GetSize() const
        {
            return size;
        }

        const u8* GetPixelAddress(int x, int y) const
        {
            return &imageData[x + y*stride];
        }

        int GetTransparent() const
        {
            return transparent;
        }

    private:
        void VerifyTransparency();
        u8	*imageData;
        Vector<int> size;
        int stride;
        int transparent;

    };
}
