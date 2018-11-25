#pragma once
#include "game/actor.h"
#include "util/vector.h"
#include "util/rect.h"
#include "util/direction.h"

namespace hfh3
{
    class Base : public Actor
    {
    public:
        Base(class GameServer& inWorld);


        virtual void Update() override;
        virtual void Draw(class CommandBuffer& commands) override;
        virtual Rect<s16> GetBounds() override;

        virtual void OnCollision(class Actor* other) override;
        
        bool IsCore() const 
        {
            return this == core;
        }

        static void CreateFort(class GameServer& server, class Random& random, const Rect<s16>& area);
    private:
        enum DestructionType {
            None,
            Leaf,
            Core
        };

        void Destroy(DestructionType type);
        void UpdateShape();
        int EdgeCount();
        Direction MaskToDirection(u8 mask);

        // The parent core node
        class Base* core;
        // Sibling base nodes
        class Base* north;
        class Base* east;
        class Base* south;
        class Base* west;

        // Can this node be destroyed upon a hit?
        bool destructible;

        // Set on dangling nodes after a leaf node has been destroyed
        // to acheive a chain reaction when hit.
        DestructionType delayAction;
        int delay;
    };
}
