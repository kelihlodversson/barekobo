#include <circle/devicenameservice.h>
#include <circle/usb/usb.h>
#include <circle/usb/usbhostcontroller.h>
#include "input/input.h"
#include "util/log.h"
using namespace hfh3;
#define SET_IDLE 0x0A

Input* Input::instance;

Input::Input()
    : playerDirection(Stopped)
{
    instance = this;
}

bool Input::Initialize()
{
    gamePad = (CUSBGamePadDevice *) CDeviceNameService::Get()->GetDevice("upad1", false);
    if(!gamePad)
        return false;

    // The Circle implementation of HID drivers polls the unit in a tight loop
    // Setting the IDLE property on the device will reduce the amount of resources
    // consumed.
    if (gamePad->GetHost()->ControlMessage(gamePad->GetEndpoint0(),
                    REQUEST_OUT | REQUEST_CLASS | REQUEST_TO_INTERFACE,
                    SET_IDLE, 0,
                    gamePad->GetInterfaceNumber (), 0, 0) < 0)
    {
        ERROR("Cannot set idle parameters");
        return false;
    }

    gamePad->RegisterStatusHandler(GamePadStatusHandler);
    const TGamePadState *state = gamePad->GetReport();
    if(state)
    {
        INFO("Gamepad %u: %d Button(s) %d Hat(s) %d Axes\n", 1, state->nbuttons, state->nhats, state->naxes);
    }
    return true;
}

int Input::NormalizeAxisValue(int value, int min, int max)
{
    int mid = (min+max)/2;
    return value < mid ? -1 : value > mid ? 1 : 0;
}

void Input::GamePadStatusHandler (unsigned device, const TGamePadState *state)
{
    assert(instance != nullptr);
    if (state->naxes >= 2)
    {
        // Assuming last two axes are the X and Y axes
        const auto& x_axis = state->axes[state->naxes-2];
        const auto& y_axis = state->axes[state->naxes-1];
        int x = NormalizeAxisValue(x_axis.value, x_axis.minimum, x_axis.maximum);
        int y = NormalizeAxisValue(y_axis.value, y_axis.minimum, y_axis.maximum);
        instance->playerDirection =
            x == 0 ?
                y == 0 ? Stopped :
                y  < 0 ? North   :
                South :
            x  < 0 ?
                y == 0 ? West :
                y  < 0 ? NorthWest   :
                SouthWest :
                y == 0 ? East :
                y  < 0 ? NorthEast :
                SouthEast ;

    }
    #if 0
    CString Msg;
    Msg.Format ("Gamepad %u: Buttons 0x%X", device+1, state->buttons);

    CString Value;

    if (state->naxes > 0)
    {
        Msg.Append (" Axes");

        for (int i = 0; i < state->naxes; i++)
        {
            Value.Format (" %d", state->axes[i].value);
            Msg.Append (Value);
        }
    }

    if (state->nhats > 0)
    {
        Msg.Append (" Hats");

        for (int i = 0; i < state->nhats; i++)
        {
            Value.Format (" %d", state->hats[i]);
            Msg.Append (Value);
        }
    }

    INFO(Msg);
    #endif
}
