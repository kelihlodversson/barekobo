
#include "game/gameclient.h"
#include "network/network.h"
#include "graphics/sprite_data.h"
#include "util/vector.h"
#include "util/log.h"

#include "render/imagesheet.h"
#include "render/image.h"
#include "render/font.h"

#include "game/view.h"
#include "game/commandbuffer.h"


using namespace hfh3;

GameClient::GameClient(ScreenManager& inScreen, class Input& inInput, Network& inNetwork)
    : World(inScreen, inInput, inNetwork)
    , server(nullptr)
    , idleTask()
{
}

GameClient::~GameClient()
{
    if(server)
    {
        delete server;
        server = nullptr;
    }
}

void GameClient::Connect()
{
    server = network.ConnectToServer();

    if(server)
    {
        DEBUG("Sending greeting");
        server->Send("HI!", 3, 0);
        DEBUG("Waiting for greeting");
        u8 buffer[FRAME_BUFFER_SIZE];
        int count = server->Receive(buffer, FRAME_BUFFER_SIZE, 0);
        // TODO Verify reply
        DEBUG("Connected %d", count);
    }
}


void GameClient::Update()
{
    // send input events to server
}

void GameClient::Draw()
{
    commands.Receive(server);
}

