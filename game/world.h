#pragma once
#include "render/screenmanager.h"
#include "render/imagesheet.h"

#include "game/starfield.h"
#include "game/commandbuffer.h"

namespace hfh3
{

    /** Base class for GameClient and Server
      */
    class World
    {
    public:
        World(ScreenManager& inScreen, class Input& inInput, class Network& inNetwork);
        virtual ~World();

        void GameLoop();

        virtual void Update() = 0;
        
        class Stage& GetStage() 
        {
            return stage;
        }

    protected:
        Stage stage;
        ScreenManager& screen;
        class Input& input;
        class Network& network;
        ImageSheet imageSheet;
        Starfield background;
        CommandBuffer commands;
    };
}
