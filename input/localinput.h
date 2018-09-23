#pragma once

#include "input/input.h"

namespace hfh3
{
    /** Wrapper around input methods for controlling the game
      */
    class LocalInput : public Input
    {
    public:
        LocalInput();
        ~LocalInput();

        bool Initialize();

    private:
        static LocalInput* instance;
        static void KeyboardStatusHandler(unsigned char modifiers, const unsigned char keys[6]);
        static void GamePadStatusHandler (unsigned device, const class TGamePadState *state);
        static int NormalizeAxisValue(int value, int min, int max);

        void UpdateButtonState(unsigned device, unsigned buttonMask, const unsigned* decode);

        unsigned lastDevice;
        unsigned buttonLastDevice[4];
    };

}
