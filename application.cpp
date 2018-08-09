#include "application.h"
#include "sprite/imagesheet.h"
#include "sprite/image.h"
#include "graphics/sprite_data.h"
#include "util/random.h"
#include "util/log.h"
#include "game/actor.h"
#include "game/player.h"
#include "game/enemy.h"

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

    ImageSheet image(sprites_pixels, sprites_width, sprites_height, 16, 16, 255, 8);
    Random random;
    const int spriteCount = 50;
    IActor* sprite[spriteCount];
    const int player = spriteCount - 1;
    for (int i = 0; i < spriteCount-1; i++)
    {
        sprite[i] = new Enemy(screenManager, image, random);
    }
    sprite[player] = new Player(screenManager, image, input);

    ScreenManager::ScreenRect clippedArea(10,10,screenManager.GetWidth()-20, screenManager.GetHeight()-20);
    while(true)
    {
        screenManager.Clear(10);
        screenManager.SetClip(clippedArea);
        screenManager.Clear(0);
        for (int i = 0; i < spriteCount; i++)
        {
            sprite[i]->Update();
            sprite[i]->Draw();
        }
        screenManager.ClearClip();
        screenManager.Present();

    }
    for (int i = 0; i < spriteCount; i++)
    {
        delete sprite[i];
    }
    return EXIT_HALT;
}
