#pragma once
#include "render/screenmanager.h"
#include "render/imagesheet.h"
#include "util/dlinklist.h"
#include "util/random.h"

#include "game/partition.h"
#include "game/starfield.h"

namespace hfh3
{

    /** Manages all active game objects.
      */
    class World
    {
    public:
        World(ScreenManager& inScreen, class Input& inInput, class Network& inNetwork);
        ~World();

        void GameLoop();

        void Update();
        void Draw();
        Actor* CollisionCheck(class Actor* collider);

        void SpawnEnemy();
        void SpawnPlayer();
        void SpawnMissile(const Vector<int>& position, const class Direction& direction , int speed);
        Stage& GetStage() { return stage; }

    private:

        void AssignPartitions();
        void PerformPendingDeletes();

        Random random;
        Stage stage;
        class Input& input;
        class Network& network;
        ImageSheet imageSheet;
        Starfield background;
        DLinkList<class Partition> partitions;

        // When actors are spawned or moved out of the bounding box of a partition,
        // they will be added to this list.
        DLinkList<class Actor*> needsNewPartition;
        DLinkList<class Actor*> pendingDelete;
    };
}
