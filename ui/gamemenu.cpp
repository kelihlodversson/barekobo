#include "ui/gamemenu.h"

#include "network/network.h"
#include "network/beacon.h"
#include "network/spotter.h"
#include "util/vector.h"
#include "util/log.h"
#include "input/input.h"

#include "render/font.h"
#include "game/gameserver.h"
#include "game/gameclient.h"


using namespace hfh3;

GameMenu::GameMenu(ScreenManager& inScreen, Input& inInput, Network& inNetwork)
    : abort_action()
    , spotter(nullptr)
    , lastInput(Direction::Stopped)
    , screen(inScreen)
    , input(inInput)
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

    menu.Clear();

    menu.Append("WELCOME TO MultiKobo",           80, 0);
    menu.Append("Please select an option below",  78, 0);
    menu.Append();
    menu.Append();
    menu.Append();
    menu.Append("Single Player:",                 82, 0);
    menu.Append("Start a Single Player Game",     22, 0, [&](){StartHost(false);});
    menu.Append();
    menu.Append("Multiplayer:",                   82, 0);
    menu.Append("Start Server",                   22, 0, [&](){StartHost(true);});
    menu.Append("Connect to Server",              22, 0, [&](){StartClient();});
    menu.Append();
    menu.Append();
    menu.Append();
    menu.Append();
    menu.Append();
    menu.Append();
    menu.Append();
    menu.Append("Compile time: " __DATE__ " " __TIME__, 48, 0);

    SelectFirst();
}

void GameMenu::SelectFirst()
{
    selected_entry = menu.FindFirst([](MenuEntry& entry)
    {
        return static_cast<bool>(entry.action); 
    });
}

void GameMenu::SelectLast()
{
    selected_entry = static_cast<Menu::Iterator>(menu.FindLast([](MenuEntry& entry)
    {
        return static_cast<bool>(entry.action); 
    }));
}

void GameMenu::SelectPrevious()
{
    while(selected_entry)
    {
        selected_entry--;
        if(selected_entry && selected_entry->action)
        {
            break;
        }
    }
    
    // Wrap around to the last item if we move past the first one
    if (!selected_entry)
    {
        SelectLast();
    }
}

void GameMenu::SelectNext()
{
    while(selected_entry)
    {
        selected_entry++;
        if(selected_entry && selected_entry->action)
        {
            break;
        }
    }
    
    // Wrap around to the first item if we move past the last one
    if (!selected_entry)
    {
        SelectFirst();
    }
}

void GameMenu::SetupAbortToMainMenu()
{
    menu.Append();
    menu.Append();
    menu.Append("Press the B button to abort...", 48, 0);
    abort_action = [&](){InitMenu();};
}

void GameMenu::StartClient()
{
    DEBUG("GameMenu::StartClient");
    spotter = new Spotter([&](){
        DEBUG("GameMenu::StartClient callback");
        // Remember the text of the current selection.
        const char* save_selected = selected_entry?selected_entry->text:nullptr;
        menu.Clear();
        menu.Append("Select one of the following hosts...", 80, 0);
        menu.Append();
        spotter->ForEachHost([&](Spotter::Host& host){
            menu.Append(host.GetIpString(), 22, 0, [&](){
                CIPAddress ipAddress;
                host.GetIpAddress(ipAddress);
                SelectServer(ipAddress);
            });
        }); 
        
        selected_entry = menu.FindFirst([&](MenuEntry& entry)
        {
            return entry.action && strcmp(entry.text, save_selected) >= 0;
        });

        if(!selected_entry)
        {
            SelectFirst();
        }
        
        SetupAbortToMainMenu();
    });
    menu.Clear();
    menu.Append("Searching for available servers...", 80, 0);
    SelectFirst();
    SetupAbortToMainMenu();
    DEBUG("GameMenu::StartClient end");
}

void GameMenu::StartHost(bool multiplayer)
{
    const int enemyCount = 3000;
    menu.Clear();
    if(multiplayer)
    {
        menu.Append("Waiting for client connection...", 80, 0);
    }
    else
    {
        menu.Append("Initializing Single Player Game...", 80, 0);
    }
    SelectFirst();
    Update();

    GameServer server(screen, input, network);
    server.SpawnFortress();
    for (int i = 0; i < enemyCount; i++)
    {
        server.SpawnEnemy();
    }

    if(multiplayer)
    {
        Beacon beacon; // Announce our presence to the local network
        server.Bind(); // Wait for connection
    }

    menu.Clear();
    SelectFirst();
    Update();
    
    server.SpawnPlayer();
    server.GameLoop();

    // Return to main menu
    InitMenu();
}

void GameMenu::SelectServer(CIPAddress& address)
{
    if(spotter)
    {
        delete spotter;
        spotter = nullptr;
    }

    GameClient client(screen, input, network);
    menu.Clear();
    menu.Append("Connecting to server...", 80, 0);
    SelectFirst();
    Update();

    client.Connect(address);

    menu.Clear();
    SelectFirst();
    Update();

    client.GameLoop();

    // Return to main menu
    InitMenu();
}

void GameMenu::HandleInput()
{
    Direction dir = input.GetPlayerDirection();
    if(lastInput == dir)
    {
        dir = Direction::Stopped;
    }
    else
    {
        lastInput = dir;
    }

    switch(dir)
    {
        case Direction::East: // pressing right selects the last entry
            SelectLast();
            break;

        case Direction::South:
            SelectNext();
            break;

        case Direction::North:
            SelectPrevious();
            break;

        case Direction::West: // pressing left selects the first entry
            SelectFirst();
            break;

        default:
            break;
    }

    if (abort_action && input.GetButtonState(Input::ButtonB) == Input::ButtonPressed)
    {
        abort_action();
    }
    if (selected_entry && input.GetButtonState(Input::ButtonA) == Input::ButtonPressed)
    {
        assert(selected_entry->action);
        (selected_entry->action)();
    }
}

void GameMenu::Draw()
{
    Font& font = Font::GetDefault();
    Vector<s16> size = screen.GetSize();
    int line_height = font.GetHeight();

    // Center the menu vertically
    Vector<s16> pos (0, (size.y - menu.Size() * line_height) / 2);

    // Print each entry
    for (MenuEntry& entry : menu)
    {
        int line_width = font.GetWidth(entry.text);
        pos.x = (size.x - line_width) / 2;

        u8 fg,bg;
        if (!selected_entry || &entry != &*selected_entry)
        {
            fg = entry.foreground;
            bg = entry.background;
        }
        else
        {
            fg = entry.background;
            bg = entry.foreground;
        }
 
        screen.DrawRect(Rect<s16>(pos.x-1, pos.y, line_width+2, line_height), bg);
        screen.DrawString(pos, entry.text, fg, font);
        pos.y += line_height;
    }
}

void GameMenu::Update()
{
    Rect<s16> clippedArea(0, 10, screen.GetWidth(), screen.GetHeight()-10);
    CString message;
    CString pos;
    CString tmp;

    u32 ip = network.GetIPAddress();

    message.Format("IP: %u.%u.%u.%u. FPS: %u. Missed: %d. Render:%3u%% Copy:%3u%%",
        (ip & 0xff),
        (ip & 0xff00)>>8,
        (ip & 0xff0000)>>16,
        (ip & 0xff000000)>>24,
        screen.GetFPS(),
        screen.GetMissedFrames(),
        screen.GetGameTimePCT(),
        screen.GetFlipTimePCT()
    );

    screen.Clear(10);
    screen.DrawString({1,1}, message, 0, Font::GetDefault());
    screen.SetClip(clippedArea);
    screen.Clear(0);

    Draw();

    screen.ClearClip();
    screen.Present();

    HandleInput();
}

void GameMenu::MainLoop()
{
    while(true)
    {
        Update();
    }

}