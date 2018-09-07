#include <circle/string.h>

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
#include "game/shot.h"
#include "game/starfield.h"
#include "game/world.h"

#include "network/network.h"

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
    INIT(screenManager)
    screenManager.DrawString(screenManager.GetSize()/2-Vector<int>(80,0), "Loading MultiKobo...", 20, Font::GetDefault());
    screenManager.Present();
    INIT(usb)
    INIT(input)
    INIT(network)
    return true;
}

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

    const int enemyCount = 3000;

    World world;
    for (int i = 0; i < enemyCount; i++)
    {
        world.Append(new Enemy(stage, image, random));
    }

    world.Append(new Player(stage, image, input));
    world.Prepend(new Starfield(stage));

    Rect<int> clippedArea(10,10,screenManager.GetWidth()-20, screenManager.GetHeight()-20);
    CString message;

    while(true)
    {
        u32 ip = network.GetIPAddress();
        message.Format("IP: %u.%u.%u.%u. FPS: %u. Missed: %d. Render:%3u%% Copy:%3u%%",
            (ip & 0xff),
            (ip & 0xff00)>>8,
            (ip & 0xff0000)>>16,
            (ip & 0xff000000)>>24,
            screenManager.GetFPS(),
            screenManager.GetMissedFrames(),
            screenManager.GetGameTimePCT(),
            screenManager.GetFlipTimePCT()
        );

        world.Update();

        screenManager.Clear(10);
        screenManager.DrawString({1,1}, message, 0, Font::GetDefault());
        screenManager.SetClip(clippedArea);
        screenManager.Clear(0);

        world.Draw();

        screenManager.ClearClip();
        screenManager.Present();
    }

    return EXIT_HALT;
}
