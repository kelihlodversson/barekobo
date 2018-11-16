#include "game/levels.h"

using namespace hfh3;

/* Note this definition uses C99 designated initializers for struct members.
 * This is not a part of the C++ standard, but allowed by gcc
 */
Levels::Levels()
: Array({
    {
        .playerStarts = { 
            { {512, 1536}, Direction::South},
            { {512,	1504}, Direction::North}
        },
        .enemies = {},
        .fortresses = {
            {768, 1024, 64, 64},
            {384, 1280, 64, 64},
        }
    },
    {
        .playerStarts = { 
            { {512, 1024}, Direction::East},
            { {512, 1056}, Direction::West}
        },
        .enemies = {},
        .fortresses = {
            {256, 768, 96, 96},
            {768, 768, 96, 96},
            {256, 1024, 96, 96},
            {768, 1024, 96, 96},
        }
    },
})
{}