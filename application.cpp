#include "application.h"
#include "sprite/image.h"
#include "graphics/sprite_data.h"
#include "util/random.h"
#include "util/log.h"

using namespace hfh3;

Application::Application() :
    memory(true),  // Passing true enables caching and the MMU, which improves performance
    timer(&interrupts),      // The timer needs a pointer to the intterrupt system
    logger(LogDebug, &timer),
    usb(&interrupts, &timer), // The usb subsystem needs both the timer and access to bind to interrupts
    spriteManager()
{}

// Wrapper around calling the Initialize method.
// If it fails it will immediately abort the current function.
#define INIT(obj, ...) if(!obj.Initialize(__VA_ARGS__)) { return false; }

bool Application::Initialize()
{
    INIT(spriteManager)
    INIT(serial, 115200)
    {
        CDevice *log_device = nameService.GetDevice(options.GetLogDevice(), false);
        if (!log_device)
        {
            log_device = &serial;
        }

        INIT(logger, log_device)
    }
    INIT(interrupts)
    INIT(timer)
    INIT(usb)
    INIT(input)
    return true;
}


/** Simple struct holding state for each character shown on-screen
 */
struct Character {
    int x,y;
    Direction dir;
    int model;
    int shape;
    int relaxed;
    bool is_player;

    void Update(int width, int height, Random& random)
    {
        int dx=0, dy=0;
        switch(dir)
        {
        case North:
            dy =- 1;
            break;
        case NorthEast:
            dx = 1;
            dy = -1;
            break;
        case East:
            dx = 1;
            break;
        case SouthEast:
            dx = 1;
            dy = 1;
            break;
        case South:
            dy = 1;
            break;
        case SouthWest:
            dx = -1;
            dy = 1;
            break;
        case West:
            dx = -1;
            break;
        case NorthWest:
            dx = -1;
            dy = -1;
            break;
        default:
        case Stopped:
            return;
        }
        shape = dir;
        x = x + dx;
        y = y + dy;
        if (x < -16) x += width + 32;
        else if (x > width+16) x -= width + 32;
        if (y < -16) y += height + 32;
        else if (y > height+16) y -= height + 32;

        // Change direction at random intervals.
        if (!is_player && random.Get() % relaxed == 0)
        {
            unsigned r = random.Get();
            dir = static_cast<Direction>((dir + (r%3)-1) % 8);
        }
    }
};
#if 0
static void TimerTest (unsigned hTimer, void *pParam, void *pContext)
{
    INFO("Timer %u, %p, %p", hTimer, pParam, pContext);
    if (!CTimer::Get()->StartKernelTimer (MSEC2HZ(1000), TimerTest, pParam, pContext))
    {
        ERROR("Could not schedule kernel timer");
    }
}
#endif

/**
* This is the main run loop for the application.
* Should update game state and present each frame.
*/
int Application::Run()
{
    INFO("Started MultiKobo. Compile time: " __DATE__ " " __TIME__);
    //TimerTest(-1, this, nullptr);

    const int width = spriteManager.GetWidth()-8;
    const int height = spriteManager.GetHeight()-8;
    serial.Write("Application::Run()\r\n",20);
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
    const int spriteCount = 20;
    Character sprite[spriteCount];
    const int player = spriteCount - 1;
    for (int i = 0; i < spriteCount; i++)
    {
        sprite[i].x = (random.Get()+i*4) % width;
        sprite[i].y = random.Get() % height;
        sprite[i].shape = random.Get() % 8;
        sprite[i].dir = static_cast<Direction>(sprite[i].shape);
        sprite[i].model = random.Get() % 6;
        sprite[i].relaxed = random.Get() % 100;
        sprite[i].is_player = false;
    }

    sprite[player].is_player = true;

    while(true)
    {
        spriteManager.Clear();
        sprite[player].dir = input.GetPlayerDirection();
        for (int i = 0; i < spriteCount; i++)
        {
            spriteManager.DrawImage(sprite[i].x, sprite[i].y, image[sprite[i].model][sprite[i].shape]);
            sprite[i].Update(width, height, random);
        }
        spriteManager.Present();

    }
    return EXIT_HALT;
}
