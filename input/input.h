#pragma once

#include <circle/types.h>

namespace hfh3
{
    enum Direction
    {
        North,
        NorthEast,
        East,
        SouthEast,
        South,
        SouthWest,
        West,
        NorthWest,
        Stopped
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
        static void KeyboardStatusHandler(unsigned char modifiers, const unsigned char keys[6]);
        static void GamePadStatusHandler (unsigned device, const class TGamePadState *state);
        static int NormalizeAxisValue(int value, int min, int max);


        Direction playerDirection;
        unsigned lastDevice;
    };

}
