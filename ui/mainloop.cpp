#include "ui/mainloop.h"

#include "util/vector.h"
#include "util/log.h"
#include "input/input.h"

#include "render/font.h"

using namespace hfh3;


MainLoop::MainLoop(ScreenManager& inScreen)
    : screen(inScreen)
{}

MainLoop::~MainLoop()
{
    for(IUpdatable* client: clients)
    {
        delete client;
    }
    clients.Clear();
}

void MainLoop::DestroyClient(IUpdatable* client)
{
    auto found = clients.FindFirst([=](IUpdatable* entry){ return client == entry; });
    assert(found);
    found.Remove();
    delete client;
}

void MainLoop::Run()
{
    while(true)
    {
        Update();
        Rect<s16> clippedArea(0, 10, screen.GetWidth(), screen.GetHeight()-10);
        CString message;
        CString pos;
        CString tmp;

        message.Format("FPS: %u. Missed: %d. Render:%3u%% Copy:%3u%%",
            screen.GetFPS(),
            screen.GetMissedFrames(),
            screen.GetGameTimePCT(),
            screen.GetFlipTimePCT()
        );

        screen.Clear(10);
        screen.DrawString({1,1}, message, 0, Font::GetDefault());
        screen.SetClip(clippedArea);
        screen.Clear(0);
        Render();
        screen.ClearClip();
        screen.Present();
        PostRender();
    }

}

void MainLoop::Update()
{
    for(IUpdatable* client : clients)
    {
        if(client->active)
        {
            client->Update();
        }
    }
}

void MainLoop::Render()
{
    for(IUpdatable* client : clients)
    {
        if(client->active)
        {
            client->Render();
        }
    }
}
void MainLoop::PostRender()
{
    for(IUpdatable* client : clients)
    {
        if(client->active)
        {
            client->PostRender();
        }
    }
}
