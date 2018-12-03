#include "ui/gamemenu.h"

#include "network/network.h"
#include "network/beacon.h"
#include "network/spotter.h"
#include "util/vector.h"
#include "util/log.h"
#include "util/async.h"
#include "input/input.h"

#include "render/font.h"
#include "game/gameserver.h"
#include "game/gameclient.h"


using namespace hfh3;

GameMenu::GameMenu(MainLoop& mainLoop, Input& inInput, Network& inNetwork)
    : Menu(mainLoop, inInput)
    , spotter(nullptr)
    , network(inNetwork)
{
    InitMenu();
}

GameMenu::~GameMenu()
{
    if(spotter)
    { 
        delete spotter;
        spotter = nullptr;
    }
}

void GameMenu::InitMenu()
{
    if(spotter)
    { 
        delete spotter;
        spotter = nullptr;
    }

    entries.Clear();

    entries.Append("WELCOME TO MultiKobo",           80, 0);
    entries.Append("Please select an option below",  78, 0);
    entries.Append();
    entries.Append();
    entries.Append();
    entries.Append("Single Player:",                 82, 0);
    entries.Append("Start a Single Player Game",     22, 0, [&](){StartHost(false);});
    entries.Append();
    entries.Append("Multiplayer:",                   82, 0);
    entries.Append("Start Server",                   22, 0, [&](){StartHost(true);});
    entries.Append("Connect to Server",              22, 0, [&](){StartClient();});
    entries.Append();
    entries.Append();
    entries.Append();
    entries.Append();
    entries.Append();
    entries.Append();
    entries.Append();
    entries.Append("Compile time: " __DATE__ " " __TIME__, 48, 0);

    SelectFirst();
}

void GameMenu::SetupAbortToMainMenu()
{
    entries.Append();
    entries.Append();
    entries.Append("Press the B button to abort...", 48, 0);
    abortAction = [&](){InitMenu();};
}

void GameMenu::StartClient()
{
    DEBUG("GameMenu::StartClient");
    spotter = new Spotter([=](){
        DEBUG("GameMenu::StartClient callback");
    
        // Remember the text of the current selection.
        String save_selected = selectedEntry?selectedEntry->text:String();
        entries.Clear();
        entries.Append("Select one of the following hosts...", 80, 0);
        entries.Append();
        spotter->ForEachHost([&](Spotter::Host& host){
            ipv4_address_t address = host.GetIpAddress();
            entries.Append(host.GetIpString(), 22, 0, [=]()
            {
                SelectServer(address);
            });
        }); 
        
        selectedEntry = entries.FindFirst([&](MenuEntry& entry)
        {
            return entry.action && entry.text == save_selected;
        });

        if(!selectedEntry)
        {
            SelectFirst();
        }
        
        SetupAbortToMainMenu();
    });
    entries.Clear();
    entries.Append("Searching for available servers...", 80, 0);
    SelectFirst();
    SetupAbortToMainMenu();
    DEBUG("GameMenu::StartClient end");
}

void GameMenu::StartHost(bool multiplayer)
{
    entries.Clear();
    if(multiplayer)
    {
        entries.Append("Waiting for client connection...", 80, 0);
    }
    else
    {
        entries.Append("Initializing Single Player Game...", 80, 0);
    }
    SelectFirst();

    new Async([=]()
    {
        auto server = mainLoop.CreateClient<GameServer>(input, network);
        server->SetDestructionHandler([=](){Resume(); InitMenu();});
        if(multiplayer)
        {
            Beacon beacon; // Announce our presence to the local network
            server->Bind(); // Wait for connection
        }
        server->LoadLevel();
        Pause();
    });
}

void GameMenu::SelectServer(ipv4_address_t address)
{
    if(spotter)
    {
        delete spotter;
        spotter = nullptr;
    }

    entries.Clear();
    entries.Append("Connecting to server...", 80, 0);
    SelectFirst();

    new Async([=]()
    {
        auto client = mainLoop.CreateClient<GameClient>(input, network);
        client->SetDestructionHandler([=](){Resume(); InitMenu();});

        client->Connect(address);
        Pause();
    });

}