#include "input/proxyinput.h"
#include "util/log.h"
using namespace hfh3;

void ProxyInput::SetInputState(u8 state)
{
    playerDirection = Direction(state >> 4);
    for(int i = 0; i<4; i++)
    {
        int newState = state & (1<<i)?ButtonDown:ButtonUp;
        if(newState != (buttons[i] & 1))
        {
            newState |= ButtonStateChanged;
        }
        buttons[i] = static_cast<ButtonState>(newState);
    }
}

