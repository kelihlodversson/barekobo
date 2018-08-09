#pragma once
#include <circle/types.h>
#include "sprite/image.h"

namespace hfh3
{

    /** An image defines an area of pixels to be rendered to the screen.
      * A single color can be defined as transparent, meaning that existing
      * content in the framebuffer will be left untouched.
      *
      * Note that the Image class only keeps a reference to the underlying
      * pixel buffer and that the actual rendering is handled by the ScreenManager.
      */
    class ImageSheet
    {
    public:
        /** Initializes an image sheet from a rawy byte buffer. It assumes that
          * all images in the sheet are of equal size and are tiled next to each other.
          *
          * @param inData The raw byte buffer containing a sheet of images.
          * @param bufferWidth The number of bytes per line of the buffer image.
          * @param bufferHeight The total number of scan lines in the buffer
          * @param imageWidth The width of each image in the sheet
          * @param imageHeight The height of each image in the sheet
          * @param transparent The index of the transparent pixel. Pass -1 if all pixels should be rendered.
          * @param groupSize The number of images in each group.
          */
        ImageSheet(u8* inData, unsigned bufferWidth, unsigned bufferHeight, int imageWidth, int imageHeight, int transparent=-1, unsigned groupSize=1);
        ~ImageSheet();

        /** Returns an array of images representing a group of images.
          * Example: imageSheet[4][0] // Returns the first image in the 5th image group
          */
        class Image* operator[](int i)
        {
            return &images[groupSize*i];
        }

        int GetImageCount() const
        {
            return imageCount;
        }

        int GetGroupCount() const
        {
            return imageCount / groupSize;
        }

        int GetGroupSize() const
        {
            return groupSize;
        }

    private:
        u8* GetImageStart(u8* data, int col, int row);
        Image* images;
        unsigned groupSize;
        unsigned imageCount;
    };
}
