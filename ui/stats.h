#pragma once
#include "ui/mainloop.h"

namespace hfh3
{

    /** Generic base class for simple text based menus.
      */
    class Stats : public MainLoop::IUpdatable
    {
    public:
        Stats(MainLoop& mainLoop);
        virtual ~Stats();
        
    protected:
        virtual void Update() override;
        virtual void Render() override;
        virtual Rect<s16> GetBounds() const override;
    
    private:


    };
}
