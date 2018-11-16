#pragma once
#include "game/level.h"

namespace hfh3
{

    /** Singleton class that is an array of levels with preinitialized content
     */
    class Levels : public Array<Level>
    {
    public:
        Levels();
    };

}
