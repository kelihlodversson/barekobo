#include "render/imagesheet.h"
#include "util/new.h" // for placement new

using namespace hfh3;

ImageSheet::ImageSheet(u8* inData, unsigned bufferWidth, unsigned bufferHeight, int imageWidth, int imageHeight, int transparent, unsigned inGroupSize) :
    groupSize(inGroupSize)
{
    const unsigned columns = bufferWidth / imageWidth;
    const unsigned rows = bufferHeight / imageHeight;
    imageCount = rows*columns;

    // Calculate the byte offset for a single row and column in the image data
    const unsigned rowOffset = imageHeight * bufferWidth;
    const unsigned colOffset = imageWidth;

    // Since Image does not have a default constructor, we use placement new to initialize the elements in the array:
    //      First allocate uninitialized memory big enough for the images
    images = reinterpret_cast<Image*>(new u8[imageCount * sizeof(Image)]);
    assert((reinterpret_cast<uintptr>(images) & 3) == 0); // assert that the allocated memory is aligned on 4 bytes

    //      ... then loop through them and call the Image constructor in-place
    for (unsigned row = 0, i=0; row < rows; row++)
    {
        for(unsigned col = 0; col < columns; col++, i++)
        {
            new(&images[i]) Image(&inData[rowOffset*row + colOffset*col], imageWidth, imageHeight, transparent, bufferWidth);
        }
    }
}

ImageSheet::~ImageSheet()
{
    if(images)
    {
        // Since we used placement new to allocate the array, we need to manually
        // invoke the destructors for the images.
        for(unsigned i=0; i<imageCount; i++)
        {
            images[i].~Image();
        }
        // And then free the undelying array type
        delete[] reinterpret_cast<u8*>(images);
        images = nullptr;
    }
}
