#pragma once
#include "render/screenmanager.h"
#include "util/direction.h"
#include "util/callback.h"
#include "util/list.h"
#include "circle/types.h"
#include "circle/net/ipaddress.h"

namespace hfh3
{

    /** The main menu at the start of the game
      */
    class GameMenu
    {
    public:
        GameMenu(ScreenManager& inScreen, class Input& inInput, class Network& inNetwork);
        ~GameMenu();

        void MainLoop();

        class MenuEntry;
        using Handler = Callback<void()>;

        class MenuEntry {

        public:
            MenuEntry() 
                : text(""), foreground(0), background(0), action()
            {} 

            MenuEntry(const char* inText, u8 inFG, u8 inBG) 
                : text(inText), foreground(inFG), background(inBG), action()
            {} 

            template<typename F>
            MenuEntry(const char* inText, u8 inFG, u8 inBG, F&& inAction) 
                : text(inText), foreground(inFG), background(inBG), action(inAction)
            {} 

        private:
            const char* text;
            u8 foreground;
            u8 background;
            Handler action;

            friend class GameMenu;
        };

        void SelectFirst();
        void SelectLast();
        void SelectPrevious();
        void SelectNext();

    private:
        void InitMenu();

        void StartClient();
        void StartHost(bool multiplayer);

        void SelectServer(class CIPAddress& address);
        void SetupAbortToMainMenu();

        void Update();
        void Draw();
        void HandleInput();

        using Menu = List<MenuEntry>;

        Menu menu;
        Menu::Iterator selected_entry;
        Handler abort_action;
        class Spotter* spotter;

        Direction lastInput;

        ScreenManager& screen;
        class Input& input;
        class Network& network;
    };
}
