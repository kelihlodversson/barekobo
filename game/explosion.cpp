#include "game/explosion.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "game/gameserver.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"

using namespace hfh3;


Explosion::Explosion(class GameServer& inWorld, class ImageSheet& imageSheet, const Vector<s16>& position, Direction inDir, int inSpeed) :
    Mover(inWorld,
          (u8)ImageSet::Explosion,
          imageSheet.GetGroupSize(),
          inDir, inSpeed,
          CollisionMask::Enemy, CollisionMask::None),
    ttl(16)
{
    SetPosition(position);
}

void Explosion::Update()
{
    UpdatePosition();
    SetImageIndex(8-(ttl/2));

    if(!--ttl)
    {
        Destroy();
    }
}

