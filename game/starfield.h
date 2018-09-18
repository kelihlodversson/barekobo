#pragma once
#include <circle/types.h>

#include "game/actor.h"
#include "game/stage.h"
#include "util/rect.h"

namespace hfh3
{

    /** Utility class for rendering a simple star field in the background.
      */
    class Starfield
    {
    public:
        Starfield(class World& inWorld, int inDensity=2500, u64 inSeed=999);

        void Draw(class View& view);
    private:
        Stage parallax[2];
        int density;
        u64 seed;
    };
}
