#include <circle/string.h>

#include "application.h"
#include "util/log.h"
#include "util/list.h"

#include "render/font.h"

#include "game/gameserver.h"
#include "game/gameclient.h"

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
    screenManager.DrawString(screenManager.GetSize()/2-Vector<s16>(80,0), "Loading MultiKobo...", 20, Font::GetDefault());
    screenManager.Present();
    INIT(usb)
    INIT(input)
    INIT(network)
    return true;
}

/**
 * This is the main entry point for the application.
 */
int Application::Run()
{
    INFO("Started MultiKobo. Compile time: " __DATE__ " " __TIME__);
    const int enemyCount = 3000;

    u32 server_address = 0x4789A8C0; //192.168.137.71

    if ( network.GetIPAddress() == server_address )
    {

        screenManager.DrawString(screenManager.GetSize()/2-Vector<s16>(32*4,-16), "Waiting for client connection...", 20, Font::GetDefault());
        screenManager.Present();


        List<class Actor*>::Reserve(3100);
        GameServer server(screenManager, input, network);
        server.SpawnFortress();
        for (int i = 0; i < enemyCount; i++)
        {
            server.SpawnEnemy();
        }

        server.Bind();
        screenManager.Clear();
        screenManager.Present();
        


        server.SpawnPlayer();
        server.GameLoop();
    }
    else
    {
        screenManager.DrawString(screenManager.GetSize()/2-Vector<s16>(23*4,-16), "Connecting to server...", 20, Font::GetDefault());
        screenManager.Present();

        GameClient client(screenManager, input, network);

        client.Connect();
        screenManager.Clear();
        screenManager.Present();
        client.GameLoop();
    }
    return EXIT_HALT;
}
