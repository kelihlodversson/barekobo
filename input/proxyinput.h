#pragma once

#include "input/input.h"

namespace hfh3
{
    /** Wrapper around input methods for controlling the game
      */
    class ProxyInput : public Input
    {
    public:
        void SetInputState(u8 state);

    };

}
