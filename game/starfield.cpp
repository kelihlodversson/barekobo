#include "game/starfield.h"
#include "game/stage.h"
#include "game/view.h"
#include "game/world.h"
#include "util/vector.h"
#include "util/random.h"
#include "util/memops.h"

using namespace hfh3;

Starfield::Starfield(class World& inWorld, int inDensity, u64 inSeed) 
    : nearSize(inWorld.GetStage().GetSize() / 2)
    , farSize(inWorld.GetStage().GetSize() / 4)
    , parallax({
        Stage(nearSize.x, nearSize.y),
        Stage(farSize.x, farSize.y),
    })
#if CONF_PRERENDER_STARFIELD
    , nearPixels(new u8[nearSize.x * nearSize.y])
    , farPixels(new u8[farSize.x * farSize.y])
    , near(nearPixels, nearSize.x, nearSize.y, 255)
    , far(farPixels, farSize.x, farSize.y, -1)
{
    memset(nearPixels, 255, nearSize.x * nearSize.y);
    memset(farPixels, 0, nearSize.x * nearSize.y);
    InitImages(inDensity, inSeed);
}
#else
    , density(inDensity)
    , seed(inSeed)
{

}
#endif

#if CONF_PRERENDER_STARFIELD
void Starfield::InitImages(int density, u64 seed)
#else
void Starfield::Draw(class View& view)
#endif
{
    // Initialize starfield randomizer with the same seed every frame
    Random random(seed);

#if !CONF_PRERENDER_STARFIELD
    // starfield uses two separate stages and views that are half and quarter
    // the width respectively of the main one. This will result in a parallax
    // effect when moving around the scene.
    View subviews[2] = {
        View(parallax[0], view.GetScreen()),
        View(parallax[1], view.GetScreen()) 
    };

    subviews[0].SetOffset(view.GetOffset() / 2);
    subviews[1].SetOffset(view.GetOffset() / 4);
#endif

    for (int i = 0; i < density; i++)
    {
        Vector<s16> star = random.GetVector<s16>();
        // Use some of the bits we're going to throw away for random brightness variation
        int brightness = (star.y >> 20) & 5;

#if CONF_PRERENDER_STARFIELD
        DrawStar(farPixels, farSize, star, 5+brightness);
        DrawStar(nearPixels, nearSize, star, 22+brightness);
#else
        subviews[1].DrawPixel(star, 5+brightness);
        subviews[0].DrawPixel(star, 22+brightness);
#endif
    }
}


#if CONF_PRERENDER_STARFIELD
void Starfield::DrawStar(u8* dest, const Vector<s16>& size, Vector<s16> pos, u8 color)
{
    pos.x &= (size.x - 1);
    pos.y &= (size.y - 1);
    dest[pos.y * size.x + pos.x] = color;
}

void Starfield::Draw(class View& view)
{
    // starfield uses two separate stages and views that are half and quarter
    // the width respectively of the main one. This will result in a parallax
    // effect when moving around the scene.
    View subviews[2] = {
        View(parallax[0], view.GetScreen()),
        View(parallax[1], view.GetScreen()) 
    };

    subviews[0].SetOffset(view.GetOffset() / 2);
    subviews[1].SetOffset(view.GetOffset() / 4);

    subviews[1].DrawImage(Vector<s16>( 0, 0 ), far );
    subviews[0].DrawImage(Vector<s16>( 0, 0 ), near);
}
#endif