#pragma once
#include "ui/mainloop.h"
#include "util/vector.h"
#include "util/array.h"
#include "render/image.h"

namespace hfh3
{

    /** Shows a map of the level in the lower left screen.
      */
    class MiniMap : public MainLoop::IUpdatable
    {
    public:
        MiniMap(MainLoop& mainLoop, const Vector<s16>& worldSize, s16 scaleDown = 16);
        virtual ~MiniMap();
        
        virtual Rect<s16> GetBounds() const override;

        void Clear(u8 color = 143);
        void Plot(const Vector<u8>& at, u8 color);
        void SetPlayerPosition(u8 player, const Vector<s16>& position);

    protected:
        virtual void Update() override;
        virtual void Render() override;
    
    private:
        s16 scale;
        Vector<s16> size;
        Vector<s16> player_position[2];
        u8* pixels;
        Image image;
    };
}
