#pragma once
#include "ui/mainloop.h"

#include "util/direction.h"
#include "util/callback.h"
#include "util/list.h"
#include "util/string.h"
#include "circle/types.h"
#include "circle/net/ipaddress.h"

namespace hfh3
{

    /** Generic base class for simple text based menus.
      */
    class Menu : public MainLoop::IUpdatable
    {
    public:
        Menu(MainLoop& mainLoop, class Input& inInput);
        virtual ~Menu();

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

            String text;
            u8 foreground;
            u8 background;
            Handler action;

        };

        void SelectFirst();
        void SelectLast();
        void SelectPrevious();
        void SelectNext();
        

    protected:
        virtual void Update() override;
        virtual void Render() override;

        using Entries = List<MenuEntry>;

        Entries entries;
        Entries::Iterator selectedEntry;
        Handler abortAction;
        class Input& input;
    
    private:

        Direction lastInput;

    };
}
