#include "application.h"
#include "render/imagesheet.h"
#include "render/image.h"
#include "render/font.h"
#include "graphics/sprite_data.h"
#include "util/random.h"
#include "util/log.h"
#include "game/stage.h"
#include "game/actor.h"
#include "game/player.h"
#include "game/enemy.h"
#include "game/starfield.h"

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
    screenManager.DrawString(screenManager.GetSize()/2-Vector<int>(80,0), "Loading MultiKobo...", 20, Font::GetDefault());
    screenManager.Present();
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

    Stage stage(4096, 4096, screenManager);
    ImageSheet image(sprites_pixels, sprites_width, sprites_height, 16, 16, 255, 8);
    Random random;
    Random starfieldRandom;
    const int actorCount = 3000;

    IActor* actor[actorCount];
    const int background = 0;
    const int player = actorCount - 1;
    for (int i = 1; i < actorCount-1; i++)
    {
        actor[i] = new Enemy(stage, image, random);
    }
    actor[player] = new Player(stage, image, input);
    actor[background] = new Starfield(stage);

    Rect<int> clippedArea(10,10,screenManager.GetWidth()-20, screenManager.GetHeight()-20);
    while(true)
    {
        screenManager.Clear(10);
        screenManager.DrawString({1,1}, "MultiKobo. Compiled: " __DATE__ " " __TIME__, 0, Font::GetDefault());
        screenManager.SetClip(clippedArea);
        screenManager.Clear(0);

        for (int i = 0; i < actorCount; i++)
        {
            actor[i]->Update();
        }

        for (int i = 0; i < actorCount; i++)
        {
            actor[i]->Draw();
        }
        screenManager.ClearClip();
        screenManager.Present();

    }
    for (int i = 0; i < actorCount; i++)
    {
        delete actor[i];
    }
    return EXIT_HALT;
}
