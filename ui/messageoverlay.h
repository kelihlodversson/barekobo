#pragma once
#include "ui/mainloop.h"
#include "util/vector.h"
#include "util/array.h"
#include "render/image.h"

namespace hfh3
{

    /** Shows a predefined message overlay on top of the game window.
      */
    enum class Message : u8
    {
        GetReady = 0,
        LevelCleared,
        GameOver,
        None     = 0xf,
    };

    class MessageOverlay : public MainLoop::IUpdatable
    {
    public:
        MessageOverlay(MainLoop& mainLoop, const Rect<s16>& inBounds);
        virtual ~MessageOverlay();

        
        virtual Rect<s16> GetBounds() const override
        { 
            return bounds;
        };

        void SetPlayerId(int inPlayer);
        void Clear();
        void SetMessage(Message message, s16 level, s16 timeout);

    protected:
        virtual void Update() override;
        virtual void Render() override;
    
    private:
        Rect<s16> bounds;
        Message current;
        s16 timer;
        s16 player;
        s16 level;
    };
}
