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

World::World(MainLoop& inLoop, class Input& inInput, Network& inNetwork)
    : IUpdatable(inLoop)
    , stage(2048, 4096)
    , input(inInput)
    , network(inNetwork)
    , imageSheet(sprites_pixels, sprites_width, sprites_height, 16, 16, 255, 8)
    , background(*this)
    , minimap(mainLoop.CreateClient<MiniMap>(stage.GetSize()))
    , commands(imageSheet, minimap)
{
}

World::~World()
{
    mainLoop.DestroyClient(minimap);
    minimap = nullptr;
}

// Actually execute the scheduled draw commands
void World::Render()
{
    View view = View(stage, screen);
    screen.DrawRect(World::GetBounds(),0);
    commands.Run(view, background);
}

Rect<s16> World::GetBounds() const
{
    Rect<s16> screenRect = Rect<s16>( 0, 10 , screen.GetWidth() , screen.GetHeight() - 10);
    if (minimap)
    {
        screenRect.size.x -= minimap->GetBounds().size.x;
    }
    return screenRect;
}

void World::Pause()
{
    IUpdatable::Pause();
    if(minimap)
    {
        minimap->Pause();
    }
}

void World::Resume()
{
    IUpdatable::Resume();
    if(minimap)
    {
        minimap->Resume();
    }
}

