#include "game/stage.h"
#include "render/image.h"
#include "render/screenmanager.h"

using namespace hfh3;

Stage::Stage(int width, int height) 
    : size(width, height)
    , maskX(width-1)
    , maskY(height-1)
{
    /* asserts that the passed in width and height are power of two:
       See: https://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2 */
    assert(!(maskX & width));
    assert(!(maskY & height));
}
