#pragma once

#include <circle/types.h>
#include <circle/usb/usbgamepad.h>

namespace hfh3
{
    enum Direction
    {
        Stopped,
        North,
        NorthEast,
        East,
        SouthEast,
        South,
        SouthWest,
        West,
        NorthWest
    };

    /** Wrapper around input methods for controlling the game
      */
    class Input
    {
    public:
        Input();
        ~Input();

        bool Initialize();
        Direction GetPlayerDirection() {return playerDirection;}

    private:
        static Input* instance;
        static void GamePadStatusHandler (unsigned device, const TGamePadState *state);
        static int NormalizeAxisValue(int value, int min, int max);

        Direction playerDirection;
        CUSBGamePadDevice *gamePad;
    };

}
