#pragma once
#include "util/dlinklist.h"

namespace hfh3
{

    /** Manages all active game objects.
      */
    class World : public DLinkList<class Actor, class World>
    {
        typedef DLinkList<class Actor, class World> Base;
    public:
        void Update();
        void Draw();
        void CollisionCheck(class Actor* collider);
    };
}
