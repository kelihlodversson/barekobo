#include "game/starfield.h"
#include "game/stage.h"
#include "util/vector.h"
#include "util/random.h"

using namespace hfh3;

Starfield::Starfield(class Stage& inStage, int inDensity, u64 inSeed) :
    IActor(inStage),
    density(inDensity),
    seed(inSeed)
{

}

void Starfield::Draw()
{
    // Initialize starfield randomizer with the same seed every frame
    Random random(seed);
    Vector<int> parallax = stage.GetOffset() / 2;

    for (int i = 0; i < density; i++)
    {
        Vector<int> star = random.GetVector<int>();
        // Use some of the bits we're going to throw away for random brightness variation
        int brightness = (star.y >> 20) & 5;

        // Draw two layers of stars using the same basic coordinates, but offset the
        // lower layer by a different offset for a parallax effect.
        stage.DrawPixel(stage.WrapCoordinate(star)+parallax, 10+brightness);
        stage.DrawPixel(stage.WrapCoordinate(star), 22+brightness*2);
    }
}

void Starfield::Update()
{
    // No update needed
}

Rect<int> Starfield::GetBounds()
{
    return Rect<int>();
}
