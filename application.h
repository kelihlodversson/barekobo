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
#include <circle/usb/dwhcidevice.h>

#include "sprite/sprite_manager.h"
#include "input/input.h"

namespace hfh3
{

    class Application
    {
    public:
        Application();
        bool Initialize();
        int Run();
    private:

        // ** System objects:
        // Initializes the MMU and the memory subsystem of the Raspberry PI
        CMemorySystem      memory;
        CActLED            activityLED;
        CKernelOptions     options;
        CDeviceNameService nameService;
        CSerialDevice      serial;
        CExceptionHandler  exceptionHandler;
        CInterruptSystem   interrupts;
        CTimer             timer;
        CLogger            logger;
        CDWHCIDevice       usb;
        // ** Application objects:
        // Sprite manager manages rendering sprites on the screen and
        // performing page flipping to avoid flickering.
        SpriteManager spriteManager;
        Input         input;
    };
}
