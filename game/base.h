#pragma once
#include "game/sprite.h"
#include "util/vector.h"
#include "util/rect.h"
#include "util/direction.h"

namespace hfh3
{
    class Base : public Sprite
    {
    public:
        Base(class GameServer& inWorld, class ImageSheet& imageSheet);


        virtual void Update() override;
        virtual void OnCollision(class Actor* other) override;

        static void CreateFort(class GameServer& server, ImageSheet& imageSheet, class Random& random, const Rect<s16>& area);
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
