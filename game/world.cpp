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
    , stage(8192, 8192)
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

// Actually execute the scheduled draw commands
void World::Render()
{
    View view = View(stage, screen);
    commands.Run(view, background);
}