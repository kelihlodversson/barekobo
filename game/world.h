#pragma once
#include "ui/mainloop.h"
#include "ui/minimap.h"
#include "render/screenmanager.h"
#include "render/imagesheet.h"

#include "game/starfield.h"
#include "game/commandbuffer.h"

namespace hfh3
{

    /** Base class for GameClient and Server
      */
    class World : public MainLoop::IUpdatable
    {
    public:
        World(MainLoop& inMainLoop, class Input& inInput, class Network& inNetwork);
        virtual ~World();

        class Stage& GetStage() 
        {
            return stage;
        }

        virtual void Pause() override;
        virtual void Resume() override;

    protected:
        virtual void Render() override;
        virtual Rect<s16> GetBounds() const override;

        Stage stage;
        class Input& input;
        class Network& network;
        ImageSheet imageSheet;
        Starfield background;
        MiniMap* minimap;
        CommandBuffer commands;
    };
}
