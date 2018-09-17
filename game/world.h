#pragma once
#include "render/screenmanager.h"
#include "render/imagesheet.h"
#include "util/list.h"
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
        void SpawnEnemy();
        void SpawnPlayer();
        void SpawnMissile(const Vector<int>& position, const class Direction& direction , int speed);
        Stage& GetStage() { return stage; }

    private:

        void PerformCollisionCheck();
        void AssignPartitions();
        void PerformPendingDeletes();

        Random random;
        Stage stage;
        class Input& input;
        class Network& network;
        ImageSheet imageSheet;
        Starfield background;
        static const int maxActorSize = 16;
        static const int partitionGridCount = 4;
        static const int partitionGridMask = partitionGridCount-1;
        static const int partitionCount = partitionGridCount*partitionGridCount;
        
        const Vector<int> partitionSize;
        Partition partitions[partitionCount];
        
        Partition& GetPartition(int x, int y)
        {
            return partitions[(y & partitionGridMask)*partitionGridCount + (x & partitionGridMask)];
        }  

        Partition& GetPartition(const Vector<int>& pos)
        {
            return GetPartition(pos.x / partitionSize.x, pos.y / partitionSize.y);
        }

        void AddActor(class Actor* newActor);

        // Returns a range of indexes to pass to GetPartition(x,y) that potentially contain
        // actors that overlap the rectangle passed in. 
        void GetPartitionRange(const Rect<int>& rect, int& x1, int& x2, int& y1, int& y2);

        // When actors are spawned or moved out of the bounding box of a partition,
        // they will be added to this list.
        List<class Actor*> needsNewPartition;
        List<class Actor*> pendingDelete;
        List<class Actor*> collisionSources;
    };
}
