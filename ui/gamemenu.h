#pragma once
#include "ui/menu.h"
#include "network/types.h"

namespace hfh3
{

    /** The main menu at the start of the game
      */
    class GameMenu : public Menu
    {
    public:
        GameMenu(MainLoop& inMainLoop, class Input& inInput, class Network& inNetwork);
        ~GameMenu();

    private:
        void InitMenu();

        void StartClient();
        void StartHost(bool multiplayer);

        void SelectServer(ipv4_address_t address);
        void SetupAbortToMainMenu();

        class Spotter* spotter;
        class Network& network;
    };
}
