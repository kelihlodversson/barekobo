#pragma once
#include "util/vector.h"
#include "util/list.h"
#include "util/array.h"
#include "util/rect.h"
#include "util/direction.h"

namespace hfh3
{
    /** Description of a level in the game.
      * Contains the location and sizes of bases, enemies present
      * and the spawn locations for players.
      */
    struct Level
    {
        struct SpawnPoint
        {
            Vector<s16> location;
            Direction   heading;
        };

        using FortressSpec = Rect<s16>;
        
        /**
         * Locations where the players will start from at the beginning of the level
         * and when respawning after dying.
         */
        Array<SpawnPoint> playerStarts;

        /**
         * A list of one or more bases that should be created at the start of the level
         */
        Array<FortressSpec> fortresses;
    };
}
