
#include "game/gameclient.h"
#include "network/network.h"
#include "input/input.h"
#include "graphics/sprite_data.h"
#include "util/vector.h"
#include "util/log.h"

#include "render/imagesheet.h"
#include "render/image.h"
#include "render/font.h"

#include "game/view.h"
#include "game/commandbuffer.h"

#include <circle/net/socket.h>
#include <circle/net/in.h>
#include <circle/net/ipaddress.h>


using namespace hfh3;

GameClient::GameClient(ScreenManager& inScreen, class Input& inInput, Network& inNetwork)
    : World(inScreen, inInput, inNetwork)
    , lastInputState(0)
    , readerTask(nullptr)
{
}

GameClient::~GameClient()
{
    if(readerTask)
    {
        delete readerTask;
        readerTask = nullptr;
    }

    if(server)
    {
        delete server;
        server = nullptr;
    }
}


void GameClient::Connect(CIPAddress& address)
{
    assert(readerTask == nullptr);
    server = network.ConnectToServer(address);

    if(server)
    {
        DEBUG("Sending greeting");
        server->Send("HI!", 3, 0);
        DEBUG("Waiting for greeting");
        u8 buffer[FRAME_BUFFER_SIZE];
        int count = server->Receive(buffer, FRAME_BUFFER_SIZE, 0);
        // TODO Verify reply
        DEBUG("Connected %d", count);
        readerTask = new NetworkReader(server, commands);
    }
}


void GameClient::Update()
{
    if(server)
    {
        u8 inputState = input.DumpInputState();
        if(inputState != lastInputState)
        {
            lastInputState = inputState;
            DEBUG("Sending %x", lastInputState);
            server->Send(&lastInputState, 1, MSG_DONTWAIT);
        }
    }

}

void GameClient::NetworkReader::Run()
{
    CScheduler* scheduler = CScheduler::Get();
    while(true)
    {
        if(!commands.Receive(server))
        {
            // When there is no data ready, sleep for 60th of a second
            // before trying again. This will also yield control to other threads
            scheduler->MsSleep(16); 
        }
    }  
}