#pragma once
#include <circle/startup.h>
#include <circle/memory.h>
#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <circle/types.h>

#include "sprite/sprite_manager.h"

namespace hfh3
{

    class Application
    {
    public:
        Application() :
            memory(true),  // Passing true enables caching and the MMU, which improves performance
            spriteManager()
        {}

        bool Initialize();
        int Run();
    private:
        // ** System objects:
        // Initializes the MMU and the memory subsystem of the Raspberry PI
        CMemorySystem      memory;
    	CActLED		       activityLED;
    	CKernelOptions     options;
    	CDeviceNameService nameService;

        // ** Application objects:
        // Sprite manager manages rendering sprites on the screen and
        // performing page flipping to avoid flickering.
        SpriteManager spriteManager;
    };
}
