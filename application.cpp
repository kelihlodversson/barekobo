#include "application.h"
#include "sprite/image.h"
#include "sprite/sprite_data.h"
#include "util/random.h"

using namespace hfh3;

bool Application::Initialize()
{
    return spriteManager.Initialize();
}

/**
 * This is the main run loop for the application.
 * Should update game state and present each frame.
 */

;

struct Character {
    int x,y;
    int dir;
    int model;
    int relaxed;

    void Update(int width, int height, Random& random)
    {
        int dx = (dir % 4) == 0 ? 0 : (dir > 4) ? -1 : 1;
        int dy = (dir % 4) == 2 ? 0 : (dir % 6) > 2  ? 1 : -1;
        x = x + dx;
        y = y + dy;
        if (x < -16) x += width + 32;
        if (y < -16) y += height + 32;
        if (x > width+16) x -= width + 32;
        if (y > height+16) y -= height + 32;

        if (random.Get() % relaxed == 0)
        {
            unsigned r = random.Get();
            dir = (dir + (r%3)-1) % 8;
        }
    }
};


int Application::Run()
{
    const int width = spriteManager.GetWidth()-8;
    const int height = spriteManager.GetHeight()-8;

    Image image[6][8] = {
        {
            Image (&sprites_pixels[0*16*sprites_width+8*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[0*16*sprites_width+9*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[0*16*sprites_width+10*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[0*16*sprites_width+11*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[0*16*sprites_width+12*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[0*16*sprites_width+13*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[0*16*sprites_width+14*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[0*16*sprites_width+15*16], 16, 16, 255, sprites_width),
        },
        {
            Image (&sprites_pixels[1*16*sprites_width+8*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[1*16*sprites_width+9*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[1*16*sprites_width+10*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[1*16*sprites_width+11*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[1*16*sprites_width+12*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[1*16*sprites_width+13*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[1*16*sprites_width+14*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[1*16*sprites_width+15*16], 16, 16, 255, sprites_width),
        },
        {
            Image (&sprites_pixels[2*16*sprites_width+8*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+9*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+10*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+11*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+12*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+13*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+14*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+15*16], 16, 16, 255, sprites_width),
        },
        {
            Image (&sprites_pixels[3*16*sprites_width+8*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+9*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+10*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+11*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+12*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+13*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+14*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+15*16], 16, 16, 255, sprites_width),
        },
        {
            Image (&sprites_pixels[2*16*sprites_width+0*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+1*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+2*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+3*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+4*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+5*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+6*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[2*16*sprites_width+7*16], 16, 16, 255, sprites_width),
        },
        {
            Image (&sprites_pixels[3*16*sprites_width+0*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+1*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+2*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+3*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+4*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+5*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+6*16], 16, 16, 255, sprites_width),
            Image (&sprites_pixels[3*16*sprites_width+7*16], 16, 16, 255, sprites_width),
        },

    };
    Random random;
    const int spriteCount = 200;
    Character sprite[spriteCount];
    for (int i = 0; i < spriteCount; i++)
    {
        sprite[i].x = (random.Get()+i*4) % width;
        sprite[i].y = random.Get() % height;
        sprite[i].dir = random.Get() % 8;
        sprite[i].model = random.Get() % 6;
        sprite[i].relaxed = random.Get() % 100;
    }

    while(true)
    {
        spriteManager.Clear();
        for (int i = 0; i < spriteCount; i++)
        {
            spriteManager.DrawImage(sprite[i].x, sprite[i].y, image[sprite[i].model][sprite[i].dir]);
            sprite[i].Update(width, height, random);
        }
        spriteManager.Present();

    }
    return EXIT_HALT;
}
