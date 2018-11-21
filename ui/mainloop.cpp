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
        screen.Clear(0);
        Render();
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
            screen.SetClip(client->GetBounds());
            client->Update();
            screen.ClearClip();
        }
    }
}

void MainLoop::Render()
{
    for(IUpdatable* client : clients)
    {
        if(client->active)
        {
            screen.SetClip(client->GetBounds());
            client->Render();
            screen.ClearClip();
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
