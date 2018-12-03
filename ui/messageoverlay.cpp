#include "ui/messageoverlay.h"
#include "render/font.h"
#include "util/tmath.h"

#include <circle/string.h>

using namespace hfh3;

static const char* MESSAGES[] = {
    "Get Ready Player %d",
    "Level Cleared!",
    "Game Over Player %d",
};

MessageOverlay::MessageOverlay(MainLoop& mainLoop, const Rect<s16>& inBounds)
    : MainLoop::IUpdatable(mainLoop)
    , bounds(inBounds)
    , current(Message::None)
    , timer(-1)
    , player(0)
    , level(0)
{
    
}

MessageOverlay::~MessageOverlay()
{
   
}

void MessageOverlay::SetMessage(Message message, s16 inLevel, s16 timeout)
{
    level = inLevel;
    timer = timeout;

    if(message != current)
    {
        if (message == Message::None)
        {
            //Pause();
        }
        else
        {
            //Resume();
        }
    }
    current = message;
}


void MessageOverlay::Clear()
{
    if (timer <= 0 || timer > 14)
    {
        timer = 14; // Fade out the curent message
    }
}

void MessageOverlay::Update()
{
    if(current != Message::None && timer > 0 && --timer == 0)
    {
        SetMessage(Message::None,0,-1);
    }
}

void MessageOverlay::Render()
{
    if (current == Message::None)
    {
        return;
    }
    int color = (timer > 0 && timer < 14)?timer*2:28;
    CString tmp;
    Vector<s16> pos = bounds.origin;
    pos.x += bounds.size.x/2;
    pos.y += bounds.size.y/3;

    tmp.Format("Level %d", level+1);
    s16 shift = tmp.GetLength()*4;

    screen.DrawString(pos - Vector<s16>(shift, 10), tmp, Max(0,color-4), Font::GetDefault());

    tmp.Format(MESSAGES[u8(current)], player+1);
    shift = tmp.GetLength()*4;
    screen.DrawString(pos - Vector<s16>(shift, 0), tmp, color, Font::GetDefault());
}

