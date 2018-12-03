
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
#include "game/commandlist.h"

#include <circle/net/socket.h>
#include <circle/net/in.h>
#include <circle/net/ipaddress.h>


using namespace hfh3;

GameClient::GameClient(MainLoop& inMainLoop, class Input& inInput, Network& inNetwork)
    : World(inMainLoop, inInput, inNetwork)
    , lastInputState(0)
    , server(nullptr)
    , readerTask(nullptr)
    , active(true)
{
    Pause();
    if(overlay)
    {
        overlay->SetPlayerId(1);
    }
}

GameClient::~GameClient()
{
    if (active)
    {
        if(readerTask)
        {
            readerTask->active = false;
            readerTask = nullptr;
        }
        if(server)
        {
            delete server;
            server = nullptr;
        }
    }
}


void GameClient::Connect(ipv4_address_t address, ipv4_port_t port)
{
    assert(readerTask == nullptr);
    DEBUG("Connecting to %d", address);
    auto connection = network.ConnectToServer(address, port);

    if(connection)
    {
        DEBUG("Sending greeting");
        server = connection;
        server->Send("HI!", 3, 0);
        DEBUG("Waiting for greeting");
        u8 buffer[FRAME_BUFFER_SIZE];
        int count = server->Receive(buffer, FRAME_BUFFER_SIZE, 0);
        // TODO Verify reply
        DEBUG("Connected %d", count);
        readerTask = new NetworkReader(server, commands, this);
    }
    Resume();
}


void GameClient::Update()
{
    if(server && active)
    {
        u8 inputState = input.DumpInputState();
        if(inputState != lastInputState)
        {
            lastInputState = inputState;
            server->Send(&inputState, 1, MSG_DONTWAIT);
        }
    }
    else
    {
        mainLoop.DestroyClient(this);
    }
}

void GameClient::NetworkReader::Run()
{
    CScheduler* scheduler = CScheduler::Get();
    while(active && commands.Receive(server))
    {
        scheduler->Yield(); 
    }

    if (active) // Signal to the outer class that we lost connection if still active
    {
        DEBUG("Server disconnected");
        delete server;
        server = nullptr;
        outer->active = false;
    }
}