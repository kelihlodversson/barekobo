#include "application.h"

using namespace hfh3;

bool Application::Initialize()
{
    return spriteManager.Initialize();
}


/**
 * This is the main run loop for the application.
 * Should update game state and present each frame.
 */
int Application::Run()
{
    while(true)
    {
        spriteManager.Present();
    }
    return EXIT_HALT;
}
