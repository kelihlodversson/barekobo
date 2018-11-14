#pragma once
#include "render/screenmanager.h"
#include "util/direction.h"
#include "util/callback.h"
#include "util/list.h"
#include "circle/types.h"
#include "circle/net/ipaddress.h"

namespace hfh3
{

    /** The main loop class calls Update on all main loop clients every frame
      * and handles refreshing the screen.
      */
    class MainLoop
    {
    public:

        MainLoop(ScreenManager& inScreen);
        ~MainLoop();

        void Run();

        /** IUpdatable should be implemented by all clients of MainLoop.
          * MainLoop will loop through all of its clients each frame and calls Update() on them.
          */
        class IUpdatable
        {
        public:
            IUpdatable(MainLoop& inMainLoop) 
                : active(true)
                , mainLoop(inMainLoop)
                , screen(mainLoop.GetScreenManager())
            {}

            virtual ~IUpdatable() 
            {
            }

            void Pause()  { active = false; }
            void Resume() { active = true;  }

        protected:

            virtual void Update() = 0;
            virtual void Render() {};
            virtual void PostRender() {};

            volatile bool active;

            MainLoop& mainLoop;
            ScreenManager& screen;
            friend MainLoop;
        };

        template<typename T, typename ...Args>
        T* CreateClient(Args&& ...args)
        {
            T* client = new T(*this, args...);
            clients.Append(client);
            return client;
        }

        void DestroyClient(IUpdatable* client);

        ScreenManager& GetScreenManager()
        {
            return screen;
        }

    private:

        void Update();
        void Render();
        void PostRender();

        List<IUpdatable*> clients;
        ScreenManager& screen;

    };
}