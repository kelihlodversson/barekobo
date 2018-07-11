#include <circle/types.h>
#include <circle/startup.h>

#include "application.h"

using namespace hfh3;


int main()
{
    Application myApp;
    if (!myApp.Initialize())
    {
        halt ();
        return EXIT_HALT;
    }

    int shutdownMode = myApp.Run();

    switch (shutdownMode)
    {
    case EXIT_REBOOT:
        reboot ();
        return EXIT_REBOOT;

    case EXIT_HALT:
    default:
        halt ();
        return EXIT_HALT;
    }
}
