#include "game/starfield.h"
#include "game/stage.h"
#include "game/view.h"
#include "game/world.h"
#include "util/vector.h"
#include "util/random.h"

using namespace hfh3;

Starfield::Starfield(class World& inWorld, int inDensity, u64 inSeed) :
    parallax({
        Stage(inWorld.GetStage().GetWidth()/2, inWorld.GetStage().GetHeight()/2),
        Stage(inWorld.GetStage().GetWidth()/4, inWorld.GetStage().GetHeight()/4),
    }),
    density(inDensity),
    seed(inSeed)
{

}

void Starfield::Draw(class View& view)
{
    // Initialize starfield randomizer with the same seed every frame
    Random random(seed);

    // starfield uses two separate stages and views that are half and quarter
    // the width respectively of the main one. This will result in a parallax
    // effect when moving around the screne.
    View subviews[2] = {
        View(parallax[0], view.GetScreen()),
        View(parallax[1], view.GetScreen()) 
    };

    subviews[0].SetOffset(view.GetOffset() / 2);
    subviews[1].SetOffset(view.GetOffset() / 4);

    for (int i = 0; i < density; i++)
    {
        Vector<int> star = random.GetVector<int>();
        // Use some of the bits we're going to throw away for random brightness variation
        int brightness = (star.y >> 20) & 5;

        subviews[0].DrawPixel(star, 22+brightness);
        subviews[1].DrawPixel(star, 5+brightness);
    }
}