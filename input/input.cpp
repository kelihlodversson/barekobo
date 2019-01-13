#include "util/memops.h"
#include "input/input.h"

using namespace hfh3;


Input::Input()
    : playerDirection(Direction::Stopped)
{
    memset(buttons, 0, sizeof(buttons));  
}

enum Input::ButtonState Input::GetButtonState(enum Input::Button button)
{
    enum ButtonState result = buttons[button];
    buttons[button] = static_cast<ButtonState>(buttons[button] & ButtonDown); // Clear ButtonStateChanged
    return result;
}


u8 Input::DumpInputState()
{
    return ( buttons[0]      & 1)    | 
           ( buttons[1] << 1 & 2)    |
           ( buttons[2] << 2 & 4)    |
           ( buttons[3] << 3 & 8)    |
           ((int)playerDirection)<<4 ; 
}