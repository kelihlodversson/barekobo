#pragma once
#include <circle/types.h>

#include "game/actor.h"
#include "game/stage.h"
#include "util/rect.h"
#include "config.h"

#if CONFIG_PRERENDER_STARFIELD
#   include "render/image.h"
#endif

namespace hfh3
{

    /** Utility class for rendering a simple star field in the background.
      */
    class Starfield
    {
    public:
        Starfield(class World& inWorld, int inDensity=1250, u64 inSeed=999);

        void Draw(class View& view);
    private:


        Vector<s16> nearSize;
        Vector<s16> farSize;
        Stage parallax[2];
#if CONFIG_PRERENDER_STARFIELD
        void InitImages(int density, u64 seed);
        void DrawStar(u8* dest, const Vector<s16>& size, Vector<s16> pos, u8 color);


        u8* nearPixels;
        u8* farPixels;
        Image near;
        Image far;
#else
        int density;
        u64 seed;
#endif
    };
}
