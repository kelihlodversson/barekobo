#pragma once
#include <circle/net/socket.h>

#include "util/list.h"
#include "util/array.h"
#include "util/random.h"
#include "util/vector.h"
#include "util/rect.h"

#include "input/proxyinput.h"

#include "game/world.h"
#include "game/partition.h"
#include "game/levels.h"

namespace hfh3
{

    /** Manages all active game objects.
      */
    class GameServer : public World
    {
    public:
        GameServer(MainLoop& inMainLoop, class Input& inInput, class Network& inNetwork);
        virtual ~GameServer();

        // Binds to a TCP port number ans waits until a client has connected
        void Bind();

        virtual void Update() override;
        
        void BuildCommandBuffer(class Actor* player, class Actor* otherPlayer, CommandBuffer& commandBuffer);

        void LoadLevel(int level=-1);


        void SpawnFortress(const Level::FortressSpec& area);
        void SpawnEnemy(const Level::EnemySpec& enemy);
        void SpawnPlayer(const Level::SpawnPoint& point);
        void SpawnRemotePlayer(const Level::SpawnPoint& point);
        void SpawnMissile(const Vector<s16>& position, const class Direction& direction , int speed);
        void SpawnExplosion(const Vector<s16>& position, const class Direction& direction , int speed);

        void OnBaseDestroyed(class Base* base);
        void AddBase(class Base* base);

    private:

        void PerformCollisionCheck();
        void AssignPartitions();
        void PerformPendingDeletes();

        class NetworkReader : public CTask
        {
        public:
            NetworkReader(CSocket* inConnection, ProxyInput& inRemoteInput)
                : connection(inConnection)
                , remoteInput(inRemoteInput)
            {}

            virtual void Run() override;

        private:
            CSocket* connection;
            ProxyInput& remoteInput;
        };        
        
        Random random;

        static const int maxActorSize = 16;
        static const int partitionGridCount = 8;
        static const int partitionGridMask = partitionGridCount-1;
        static const int partitionCount = partitionGridCount*partitionGridCount;
        
        const Vector<s16> partitionSize;
        Partition partitions[partitionCount];
        
        Partition& GetPartition(int x, int y)
        {
            return partitions[(y & partitionGridMask)*partitionGridCount + (x & partitionGridMask)];
        }  

        Partition& GetPartition(const Vector<s16>& pos)
        {
            return GetPartition(pos.x / partitionSize.x, pos.y / partitionSize.y);
        }

        void AddActor(class Actor* newActor);

        // Returns a range of indexes to pass to GetPartition(x,y) that potentially contain
        // actors that overlap the rectangle passed in. 
        void GetPartitionRange(const Rect<s16>& rect, int& x1, int& x2, int& y1, int& y2);

        // When actors are spawned or moved out of the bounding box of a partition,
        // they will be added to this list.
        Array<class Actor*> needsNewPartition;
        Array<class Actor*> pendingDelete;
        List<class Actor*> collisionSources;
        class Actor* player;
        class Actor* remotePlayer;
        int baseCount;

        CSocket* client;
        CommandBuffer clientCommands;
        ProxyInput    clientInput;
        NetworkReader* readerTask;
        int currentLevel;
        Levels levels;

        friend class Base;
    };
}
