#include "application.h"
#include "sprite/imagesheet.h"
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
    screenManager()
{}

// Wrapper around calling the Initialize method.
// If it fails it will immediately abort the current function.
#define INIT(obj, ...) if(!obj.Initialize(__VA_ARGS__)) { return false; }

bool Application::Initialize()
{
    INIT(screenManager)
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
    Vector<int> position;
    Direction dir;
    Image* model;
    int shape;
    int relaxed;
    bool is_player;

    void Draw(ScreenManager& screenManager)
    {
        screenManager.DrawImage(position, model[shape]);
    }

    void Update(int width, int height, Random& random)
    {
        Vector<int> delta(0,0);
        switch(dir)
        {
        case North:
            delta.y =- 1;
            break;
        case NorthEast:
            delta.x = 1;
            delta.y = -1;
            break;
        case East:
            delta.x = 1;
            break;
        case SouthEast:
            delta.x = 1;
            delta.y = 1;
            break;
        case South:
            delta.y = 1;
            break;
        case SouthWest:
            delta.x = -1;
            delta.y = 1;
            break;
        case West:
            delta.x = -1;
            break;
        case NorthWest:
            delta.x = -1;
            delta.y = -1;
            break;
        default:
        case Stopped:
            return;
        }
        shape = dir;
        position += delta;
        if (position.x < -16) position.x += width + 32;
        else if (position.x > width+16) position.x -= width + 32;
        if (position.y < -16) position.y += height + 32;
        else if (position.y > height+16) position.y -= height + 32;

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

    const int width = screenManager.GetWidth()-8;
    const int height = screenManager.GetHeight()-8;
    serial.Write("Application::Run()\r\n",20);
    ImageSheet image(sprites_pixels, sprites_width, sprites_height, 16, 16, 255, 8);
    Random random;
    const int spriteCount = 50;
    Character sprite[spriteCount];
    const int player = spriteCount - 1;
    const int player_model = 6;
    for (int i = 0; i < spriteCount; i++)
    {
        sprite[i].position = Vector<int>((random.Get()+i*4) % width, random.Get() % height);
        sprite[i].shape = random.Get() % image.GetGroupSize();
        sprite[i].dir = static_cast<Direction>(sprite[i].shape);

        int model = random.Get() % 8 + 1;
        if(model>=2) // Skip the explosion sprites
            model++;
        if(model>=6) // Skip the player model sprites
            model++;
        if(model>=9) // Skip some random sprites
            model++;
        if(model>=11) // Skip some random sprites
            model++;
        sprite[i].model = image[model];
        sprite[i].relaxed = random.Get() % 100;
        sprite[i].is_player = false;
    }

    sprite[player].is_player = true;
    sprite[player].model = image[player_model];

    ScreenManager::ScreenRect clippedArea(10,10,screenManager.GetWidth()-20, screenManager.GetHeight()-20);
    while(true)
    {
        screenManager.Clear(10);
        screenManager.SetClip(clippedArea);
        screenManager.Clear(0);
        sprite[player].dir = input.GetPlayerDirection();
        for (int i = 0; i < spriteCount; i++)
        {
            sprite[i].Draw(screenManager);
            sprite[i].Update(width, height, random);
        }
        screenManager.ClearClip();
        screenManager.Present();

    }
    return EXIT_HALT;
}
