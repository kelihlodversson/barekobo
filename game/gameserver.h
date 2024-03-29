#pragma once
#include <circle/net/socket.h>

#include "util/list.h"
#include "util/array.h"
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
#ifdef DEBUG_GAMESERVER
        virtual void Render() override;
#endif
        

        virtual void LoadLevel(int level=-1);

        Actor* FindPlayer(const Vector<s16>& position, int radius, Vector<s16>& outDelta);
        Actor* FindPlayer(const Vector<s16>& position, int radius)
        {
            Vector<s16> tmp;
            return FindPlayer(position, radius, tmp);
        }

        void SpawnEnemy(const Vector<s16>& position);
        void SpawnPlayer(int index, const Level::SpawnPoint& point);
        void SpawnMissile(int playerIndex, Direction direction, int speed);
        Actor* SpawnExplosion(const Vector<s16>& position, const class Direction& direction , int speed);
        void SpawnShot(const Vector<s16>& position, const class Direction& direction , int speed);


        void OnBaseChanged(class Base* base, u8 imageGroup, u8 imageIndex);
        void AddBase(class Base* base);

    protected:
        void SpawnFortress(const Level::FortressSpec& area);

        void UpdateScore(int player, int scoreChange);
        void OnPlayerDestroyed(int playerIndex);
        void OnBaseDestroyed(class Base* base);
        void PerformCollisionCheck();
        void AssignPartitions();
        void PerformPendingDeletes();
        void ClearLevel();

        struct PlayerInfo 
        {
            PlayerInfo(const Vector<s16>& initialCamera);
            class Player* actor;
            Vector<s16> camera;
            int score;
            int lives;
        };

        int BuildCommandBuffer(class PlayerInfo& player, class PlayerInfo& otherPlayer, CommandList& commandList);

        class NetworkReader : public CTask
        {
        public:
            NetworkReader(CSocket* inConnection, ProxyInput& inRemoteInput)
                : active(true)
                , connection(inConnection)
                , remoteInput(inRemoteInput)
            {}

            virtual ~NetworkReader();

            virtual void Run() override;
            volatile bool active;
        private:
            CSocket* connection;
            ProxyInput& remoteInput;
        };        
        
        static const int maxPlayerCount = 2;
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

        Actor* AddActor(class Actor* newActor);

        void SetMessage(int player, Message message, s16 level, s16 duration=-1);

        // Returns a range of indexes to pass to GetPartition(x,y) that potentially contain
        // actors that overlap the rectangle passed in. 
        void GetPartitionRange(const Rect<s16>& rect, int& x1, int& x2, int& y1, int& y2);

        // When actors are spawned or moved out of the bounding box of a partition,
        // they will be added to this list.
        Array<class Actor*> needsNewPartition;
        Array<class Actor*> pendingDelete;
        List<class Actor*> collisionSources;

        PlayerInfo player[maxPlayerCount];
        int baseCount;

        CSocket* client;
        CommandList clientCommands;
        ProxyInput    clientInput;
        NetworkReader* readerTask;
        int currentLevel;
        int loadLevelDelay;
        Levels levels;

        friend class Base;
    };
}
