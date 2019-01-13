#include <circle/devicenameservice.h>
#include <circle/usb/usb.h>
#include <circle/usb/usbhostcontroller.h>
#include <circle/usb/usbgamepad.h>
#include <circle/usb/usbkeyboard.h>
#include <circle/startup.h>

#include "input/localinput.h"
#include "util/log.h"
#include "util/memops.h"
using namespace hfh3;
#define SET_IDLE 0x0A

LocalInput* LocalInput::instance;

LocalInput::LocalInput()
    : lastDevice(0)
{
    memset(buttonLastDevice, 0, sizeof(buttons));
    instance = this;
}

bool LocalInput::Initialize()
{
    bool found = false;
    for(int i = 1; i < 10; i++)
    {
        CString device_id;
        device_id.Format ("upad%u", i);

        CUSBGamePadDevice *gamePad = (CUSBGamePadDevice *) CDeviceNameService::Get()->GetDevice(device_id, false);
        if(!gamePad)
            break;

        // The Circle implementation of HID drivers polls the unit in a tight loop
        // Setting the IDLE property on the device will reduce the amount of resources
        // consumed.
        if (gamePad->GetHost()->ControlMessage(gamePad->GetEndpoint0(),
                        REQUEST_OUT | REQUEST_CLASS | REQUEST_TO_INTERFACE,
                        SET_IDLE, 0,
                        gamePad->GetInterfaceNumber (), 0, 0) < 0)
        {
            ERROR("Cannot set idle parameters");
            continue;
        }

        gamePad->RegisterStatusHandler(GamePadStatusHandler);
        const TUSBDeviceDescriptor *deviceDescriptor = gamePad->GetDevice()->GetDeviceDescriptor();
        assert (deviceDescriptor);
        INFO("Found game controller: %04x:%04x", deviceDescriptor->idVendor, deviceDescriptor->idProduct);
        found = true;
    }

    // allow using a keyboard as well
    CUSBKeyboardDevice *keyboard = (CUSBKeyboardDevice *) CDeviceNameService::Get()->GetDevice("ukbd1", false);
    if(keyboard)
    {
        keyboard->RegisterKeyStatusHandlerRaw(KeyboardStatusHandler);
        const TUSBDeviceDescriptor *deviceDescriptor = keyboard->GetDevice()->GetDeviceDescriptor();
        assert (deviceDescriptor);
        INFO("Found keyboard: %04x:%04x", deviceDescriptor->idVendor, deviceDescriptor->idProduct);
        found = true;
    }
    return found;
}

/** convert a normalized axis value to a compass direction */
static Direction AxisToDirection(int x, int y)
{
    if (x < 0)
    {
        if (y < 0)
            return Direction::NorthWest;
        else if (y > 0)
            return Direction::SouthWest;
        else
            return Direction::West;
    }
    else if (x > 0)
    {
        if (y < 0)
            return Direction::NorthEast;
        else if (y > 0)
            return Direction::SouthEast;
        else
            return Direction::East;
    }
    else
    {
        if (y < 0)
            return Direction::North;
        else if (y > 0)
            return Direction::South;
        else
            return Direction::Stopped;
    }
}

// Key scan codes used for player control
// The letter names correspond to the keycaps on a US or UK querty keyboard.
// On other keyboards the location will still be the same, but the printed keycap may be different.
enum KeyCodes {
    LetterA     = 0x04,
    LetterD     = 0x07,
    LetterS     = 0x16,
    LetterW     = 0x1a,
    DeleteKey   = 0x4c,
    RightArrow  = 0x4f,
    LeftArrow   = 0x50,
    DownArrow   = 0x51,
    UpArrow     = 0x52,
};

enum ModfierMask {
    LeftCtrl    = 0x01,
    LeftShift   = 0x02,
    LeftAlt     = 0x04,
    LeftMeta    = 0x08,
    RightCtrl   = 0x10,
    RightShift  = 0x20,
    RightAlt    = 0x40,
    RightMeta   = 0x80,
    AnyCtrl     = LeftCtrl  | RightCtrl,
    AnyShift    = LeftShift | RightShift,
    AnyAlt      = LeftAlt   | RightAlt,
    AnyMeta     = LeftMeta  | RightMeta
};

static const unsigned kbdModifierButtonDecode[] =
{
    AnyShift, // ButtonA
    AnyCtrl,  // ButtonB
    AnyAlt,   // ButtonStart
    AnyMeta   // ButtonSelect
};

void LocalInput::KeyboardStatusHandler(unsigned char modifiers, const unsigned char keys[6])
{
    static const unsigned kbd_device = (unsigned)-1;
    assert(instance != nullptr);

    instance->UpdateButtonState(kbd_device, modifiers, kbdModifierButtonDecode);

    // Ignore input from the keyboard if another controller is already controlling the player
    if (instance->playerDirection != Direction::Stopped && instance->lastDevice != kbd_device)
    {
        return;
    }

    int x = 0, y = 0;
    bool deletePressed = false;
    for(int i=0; i<6; i++)
    {
        switch(keys[i])
        {
            case UpArrow:
            case LetterW:
                y-=1;
                break;
            case DownArrow:
            case LetterS:
                y+=1;
                break;
            case LeftArrow:
            case LetterA:
                x-=1;
                break;
            case RightArrow:
            case LetterD:
                x+=1;
                break;
            case DeleteKey:
                deletePressed = true;
                break;
            default:
                // ignore all other keys
                break;
        }
    }

    // Reboot if CTRL-ALT-DEL is pressed at the same time
    if (deletePressed && (modifiers & AnyCtrl) && (modifiers & AnyAlt)
        && !(modifiers & ~(AnyCtrl|AnyAlt)))
    {
        INFO("REBOOT!");
        reboot();
    }


    instance->playerDirection = AxisToDirection(x,y);
    instance->lastDevice = kbd_device;
}


int LocalInput::NormalizeAxisValue(int value, int min, int max)
{
    int low_threshold = (min+max)/4;
    int high_threshold = 3*(min+max)/4;
    return value < low_threshold ? -1 : value > high_threshold ? 1 : 0;
}

static const unsigned defaultButtonDecode[] =
{
    0x0080, // ButtonA
    0x0100, // ButtonB
    0x0020, // ButtonStart
    0x0010  // ButtonSelect
};

void LocalInput::GamePadStatusHandler (unsigned device, const TGamePadState *state)
{
    assert(instance != nullptr);
    instance->UpdateButtonState(device, state->buttons, defaultButtonDecode);

    // Ignore input from this controller if another controller is already controlling the player
    if (instance->playerDirection != Direction::Stopped && instance->lastDevice != device)
    {
        return;
    }

    Direction newDirection = Direction::Stopped;
    if (state->nhats > 0) // Prefer hats over axes
    {
        for (int i = 0 ; i<state->nhats; i++)
        {
            if (state->hats[i] != Direction::Stopped)
            {
                newDirection = (Direction)state->hats[i];
            }
        }
    }
    if (newDirection == Direction::Stopped && state->naxes >= 2)
    {
        int found=-1;
        // Find the first pair of X/Y axes
        for(int i=0; i<state->naxes-1;i++)
        {
            if(state->axes[i].type == AxisType::X && state->axes[i+1].type == AxisType::Y)
            {
                found = i;
                break;
            }
        }
        if (found >= 0)
        {
            const auto& x_axis = state->axes[found];
            const auto& y_axis = state->axes[found+1];
            int x = NormalizeAxisValue(x_axis.value, x_axis.minimum, x_axis.maximum);
            int y = NormalizeAxisValue(y_axis.value, y_axis.minimum, y_axis.maximum);
            newDirection = AxisToDirection(x, y);
        }
    }
    instance->playerDirection = newDirection;
    instance->lastDevice = device;
}

void LocalInput::UpdateButtonState(unsigned device, unsigned buttonMask, const unsigned* decode)
{
    for(int i = 0; i<4; i++)
    {
        int newState = buttonMask & decode[i]?ButtonDown:ButtonUp;

        // Ignore additional button presses coming from this device
        // while other devices are holding it down
        if(buttons[i] != ButtonUp && buttonLastDevice[i] != device)
        {
            continue;
        }

        if(newState != (buttons[i] & 1))
        {
            newState |= ButtonStateChanged;
        }
        buttons[i] = static_cast<ButtonState>(newState);
        buttonLastDevice[i] = device;
    }
}
