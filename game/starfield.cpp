#include "game/starfield.h"
#include "game/stage.h"
#include "game/world.h"
#include "util/vector.h"
#include "util/random.h"

using namespace hfh3;

Starfield::Starfield(class World& inWorld, int inDensity, u64 inSeed) :
    Actor(inWorld, CollisionMask::None, CollisionMask::None),
    parallaxStage(inWorld.GetStage().GetWidth()/2, inWorld.GetStage().GetHeight()/2, inWorld.GetStage().GetScreen()),
    density(inDensity),
    seed(inSeed)
{

}

void Starfield::Draw()
{
    // Initialize starfield randomizer with the same seed every frame
    Random random(seed);

    // parallaxStage is a separate stage, half the size and half the offset.
    // halfing the offset creates a parallax effect, and halving the size makes
    // the wrapping match the parent stage.
    parallaxStage.SetOffset(stage.GetOffset() / 2);

    for (int i = 0; i < density; i++)
    {
        Vector<int> star = random.GetVector<int>();
        // Use some of the bits we're going to throw away for random brightness variation
        int brightness = (star.y >> 20) & 5;

        parallaxStage.DrawPixel(parallaxStage.WrapCoordinate(star), 5+brightness);
        stage.DrawPixel(stage.WrapCoordinate(star), 22+brightness);
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
