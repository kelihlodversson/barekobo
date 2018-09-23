#include "game/world.h"

#include "network/network.h"
#include "graphics/sprite_data.h"
#include "util/vector.h"
#include "util/log.h"

#include "render/imagesheet.h"
#include "render/image.h"
#include "render/font.h"

#include "game/actor.h"
#include "game/enemy.h"
#include "game/player.h"
#include "game/shot.h"
#include "game/view.h"
#include "game/commandbuffer.h"

using namespace hfh3;

World::World(ScreenManager& inScreen, class Input& inInput, Network& inNetwork)
    : stage(8192, 8192)
    , screen(inScreen)
    , input(inInput)
    , network(inNetwork)
    , imageSheet(sprites_pixels, sprites_width, sprites_height, 16, 16, 255, 8)
    , background(*this)
    , commands(imageSheet)
{
}

World::~World()
{
}

void World::GameLoop()
{
    Rect<s16> clippedArea(0, 10, screen.GetWidth(), screen.GetHeight()-10);
    CString message;
    CString pos;
    CString tmp;

    View view = View(stage, screen);
    u32 ip = network.GetIPAddress();
    while(true)
    {
        Update();

        message.Format("IP: %u.%u.%u.%u. FPS: %u. Missed: %d. Render:%3u%% Copy:%3u%%",
            (ip & 0xff),
            (ip & 0xff00)>>8,
            (ip & 0xff0000)>>16,
            (ip & 0xff000000)>>24,
            screen.GetFPS(),
            screen.GetMissedFrames(),
            screen.GetGameTimePCT(),
            screen.GetFlipTimePCT()
        );

        {
            screen.Clear(10);
            screen.DrawString({1,1}, message, 0, Font::GetDefault());
            screen.SetClip(clippedArea);
            screen.Clear(0);

            // Actually execute the scheduled draw commands
            commands.Run(view, background);

            screen.ClearClip();
            screen.Present();
        }
    }

}