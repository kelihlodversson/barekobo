#include "ui/stats.h"

#include "util/vector.h"
#include "util/log.h"
#include "input/input.h"

#include "render/font.h"


using namespace hfh3;

Stats::Stats(MainLoop& mainLoop)
    : MainLoop::IUpdatable(mainLoop)
{
}

Stats::~Stats()
{
}


void Stats::Update()
{
}

void Stats::Render()
{
    CString message;
    message.Format("FPS: %u. Missed: %d. Render:%3u%% Copy:%3u%%",
        screen.GetFPS(),
        screen.GetMissedFrames(),
        screen.GetGameTimePCT(),
        screen.GetFlipTimePCT()
    );

    screen.Clear(10);
    screen.DrawString({1,1}, message, 0, Font::GetDefault());
}

Rect<s16> Stats::GetBounds() const
{
    return Rect<s16> (0, 0, screen.GetWidth(), 10);
}