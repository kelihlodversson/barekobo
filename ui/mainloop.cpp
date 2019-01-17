#include "ui/mainloop.h"

#include "util/vector.h"
#include "util/log.h"
#include "input/input.h"

#include "render/font.h"

using namespace hfh3;


MainLoop::MainLoop(ScreenManager& inScreen)
    : frame(0)
    , current({0,0,0})
    , sum({0,0,0})
    , screen(inScreen)
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
        screen.WaitForScreenBufferReady();
        Render();
        screen.Present();
        PostRender();
    }

}

void MainLoop::Update()
{
    current = {0,0,0};
    CTimer *timer = CTimer::Get();
    const unsigned updateStart = timer->GetClockTicks();
    for(IUpdatable* client : clients)
    {
        if(client->active)
        {
            screen.SetClip(client->GetBounds());
            client->Update();
            screen.ClearClip();
        }
    }
    current.update = timer->GetClockTicks() - updateStart;
}

void MainLoop::Render()
{
    CTimer *timer = CTimer::Get();
    const unsigned renderStart = timer->GetClockTicks();
    for(IUpdatable* client : clients)
    {
        if(client->active)
        {
            screen.SetClip(client->GetBounds());
            client->Render();
            screen.ClearClip();
        }
    }
    current.render = timer->GetClockTicks() - renderStart;
}

void MainLoop::PostRender()
{
    CTimer *timer = CTimer::Get();
    const unsigned postRenderStart = timer->GetClockTicks();
    for(IUpdatable* client : clients)
    {
        if(client->active)
        {
            client->PostRender();
        }
    }
    current.postRender = timer->GetClockTicks() - postRenderStart;

    sum.update += current.update;
    sum.render += current.render;
    sum.postRender += current.postRender;
    frame++;
}

void MainLoop::ClearTimers()
{
    frame = 0;
    sum = {0,0,0};
}

unsigned MainLoop::GetTimers(Timer& outSum)
{
    outSum = sum;
    return frame;
}
