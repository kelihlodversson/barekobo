#pragma once

#include <circle/types.h>
#include "util/direction.h"

namespace hfh3
{
    /** Wrapper around input methods for controlling the game
      */
    class Input
    {
    public:
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

        Input();
        ~Input();

        bool Initialize();
        Direction GetPlayerDirection() {return playerDirection;}
        enum ButtonState GetButtonState(enum Button button);

    private:
        static Input* instance;
        static void KeyboardStatusHandler(unsigned char modifiers, const unsigned char keys[6]);
        static void GamePadStatusHandler (unsigned device, const class TGamePadState *state);
        static int NormalizeAxisValue(int value, int min, int max);

        void UpdateButtonState(unsigned device, unsigned buttonMask, const unsigned* decode);

        Direction playerDirection;
        unsigned lastDevice;
        enum ButtonState buttons[4];
        unsigned buttonLastDevice[4];
    };

}
