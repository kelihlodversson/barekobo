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


            virtual void Pause()  { active = false; }
            virtual void Resume() { active = true;  }

            virtual Rect<s16> GetBounds() const
            {
                return screen.GetScreenRect();
            }

            template<typename T>
            void SetDestructionHandler(T callable)
            {
                destructionHandler = callable;
            }

        protected:
            virtual ~IUpdatable() 
            {
                if(destructionHandler)
                {
                    destructionHandler();
                }
            }

            virtual void Update() = 0;
            virtual void Render() {};
            virtual void PostRender() {};

            volatile bool active;

            MainLoop& mainLoop;
            ScreenManager& screen;
            Callback<void()> destructionHandler;
            friend MainLoop;
        };

        template<typename T, typename ...Args>
        T* CreateClient(Args&& ...args)
        {   
            // Grab the last client at the time of creation.
            // That way, any clients created while invoking the 
            // constructor will come after this one.
            auto last = clients.rbegin();
            T* client = new T(*this, args...);
            if (last != clients.rend())
            {
                last.InsertBefore(client);
            }
            else // Special case for the first client added
            {
                clients.Prepend(client);
            }
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
