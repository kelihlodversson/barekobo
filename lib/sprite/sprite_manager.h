
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
        static const unsigned fbWidth = 640;
        static const unsigned fbHeight = 480;

        SpriteManager();
        ~SpriteManager();

        bool Initialize();

        void Present();
    private:
        CBcmFrameBuffer	*framebuffer;
        bool flip; // Stores the currently active screen
    };

}
