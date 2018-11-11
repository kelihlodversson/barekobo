#include <circle/string.h>

#include "application.h"
#include "util/log.h"
#include "util/list.h"

#include "render/font.h"

#include "network/network.h"
#include "ui/gamemenu.h"

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
    GameMenu menu(screenManager, input, network);
    menu.MainLoop();
    return EXIT_HALT;
}
