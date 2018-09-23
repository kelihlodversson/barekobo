#pragma once

#include <circle/types.h>
#include "util/direction.h"

namespace hfh3
{
    /** Abstract base class around input methods for controlling the game.
      */
    class Input
    {
    public:
        Input();
        
        enum Button
        {
            ButtonA,
            ButtonB,
            ButtonStart,
            ButtonSelect
        };


        enum ButtonState
        {
            ButtonUp           = 0,       // The button is not being held nor was it released this frame
            ButtonDown         = 1,       // The button is still down after being pressed down a previous frame
            ButtonStateChanged = 2,       // If the second bit is set, the state changed since last frame
            ButtonReleased     = ButtonStateChanged | ButtonUp,    // The button was released this frame
            ButtonPressed      = ButtonStateChanged | ButtonDown   // The button was pressed down this frame
        };

        Direction GetPlayerDirection() {return playerDirection;}
        enum ButtonState GetButtonState(enum Button button);

        // Returns a snapshot of the button state to be sent to a remote server
        u8 DumpInputState();

    protected:
        Direction playerDirection;
        enum ButtonState buttons[4];
        
    };

}
