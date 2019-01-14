#include "ui/menu.h"

#include "util/vector.h"
#include "util/log.h"
#include "input/input.h"

#include "render/font.h"


using namespace hfh3;

Menu::Menu(MainLoop& mainLoop, Input& inInput)
    : MainLoop::IUpdatable(mainLoop)
    , abortAction()
    , input(inInput)
    , lastInput(Direction::Stopped)
{
    entries.Clear();
}

Menu::~Menu()
{
}

void Menu::SelectFirst()
{
    selectedEntry = entries.FindFirst([](MenuEntry& entry)
    {
        return static_cast<bool>(entry.action); 
    });
}

void Menu::SelectLast()
{
    selectedEntry = static_cast<Entries::Iterator>(entries.FindLast([](MenuEntry& entry)
    {
        return static_cast<bool>(entry.action); 
    }));
}

void Menu::SelectPrevious()
{
    while(selectedEntry)
    {
        selectedEntry--;
        if(selectedEntry && selectedEntry->action)
        {
            break;
        }
    }
    
    // Wrap around to the last item if we move past the first one
    if (!selectedEntry)
    {
        SelectLast();
    }
}

void Menu::SelectNext()
{
    while(selectedEntry)
    {
        selectedEntry++;
        if(selectedEntry && selectedEntry->action)
        {
            break;
        }
    }
    
    // Wrap around to the first item if we move past the last one
    if (!selectedEntry)
    {
        SelectFirst();
    }
}

void Menu::Update()
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

    if (abortAction && input.GetButtonState(Input::ButtonB) == Input::ButtonPressed)
    {
        abortAction();
    }
    if (selectedEntry && input.GetButtonState(Input::ButtonA) == Input::ButtonPressed)
    {
        assert(selectedEntry->action);
        (selectedEntry->action)();
    }
}

void Menu::Render()
{
    Font& font = Font::GetDefault();
    Vector<s16> size = screen.GetSize();
    int line_height = font.GetHeight();

    // Clear the menu background
    screen.DrawRect(Rect<s16>( 0, 10 , size.x , size.y - 10),0);

    // Center the entries vertically
    Vector<s16> pos (0, (size.y - entries.Size() * line_height) / 2);

    // Print each entry
    for (MenuEntry& entry : entries)
    {
        int line_width = font.GetWidth(entry.text);
        pos.x = (size.x - line_width) / 2;

        u8 fg,bg;
        if (!selectedEntry || &entry != &*selectedEntry)
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

