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
#include "game/commandlist.h"
#include "config.h"

using namespace hfh3;

World::World(MainLoop& inLoop, class Input& inInput, Network& inNetwork)
    : IUpdatable(inLoop)
    , stage(2048, 2048)
    , input(inInput)
    , network(inNetwork)
    , imageSheet(sprites_pixels, sprites_width, sprites_height, 16, 16, 255, 8)
    , minimap(mainLoop.CreateClient<MiniMap>(stage.GetSize()))
    , overlay(mainLoop.CreateClient<MessageOverlay>(GetBounds()))
    , background(*this, imageSheet, minimap)
    , commands(imageSheet)
{
    minimap->SetPlayerLives(0,0);
    minimap->SetPlayerLives(1,0);
    minimap->SetPlayerScore(0,0);
    minimap->SetPlayerScore(1,0);
}

World::~World()
{
    mainLoop.DestroyClient(overlay);
    mainLoop.DestroyClient(minimap);
    minimap = nullptr;
    overlay = nullptr;
    
}

// Actually execute the scheduled draw commands
void World::Render()
{
    View view = View(stage, screen);
    #if !CONFIG_PRERENDER_STARFIELD
        screen.DrawRect(World::GetBounds(),0);
    #endif
    commands.Run(view, background, overlay, minimap);
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

