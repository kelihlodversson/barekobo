
#pragma once
#include <circle/bcmframebuffer.h>
#include <circle/types.h>

namespace hfh3
{

    /** Class for managing a list of images to be drawn on-screen.
      */
    class SpriteManager
    {
    public:

        SpriteManager();
        ~SpriteManager();

        bool Initialize();
        void DrawImage(int x, int y, class Image& image);
        void DrawPixel(int x, int y, u8 color);
        void DrawRect(int x, int y, int w, int h, u8 color);
        void Clear(u8 color=0);

        int GetWidth() const { return width; }
        int GetHeight() const { return height; }

        void Present();
    private:
        static const unsigned fbWidth = 400;
        static const unsigned fbHeight = 300;
        // Defined in the header to inline it.
        u8* GetPixelAddress(int x, int y)
        {
            return &bufferAddress[x + (y+active*height)*stride];
        }


        CBcmFrameBuffer	*framebuffer;
        int active; // Stores the currently active screen

        u8* bufferAddress;
        int width;
        int height;
        int stride;
    };

}
