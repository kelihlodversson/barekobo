#pragma once
#include "game/mover.h"
#include "util/direction.h"

namespace hfh3
{
    class Enemy : public Mover
    {
    public:
        Enemy(class GameServer& inWorld, class ImageSheet& imageSheet);

        virtual void Update() override;
        virtual void OnCollision(class Actor* other) override;
        virtual void OnBaseDestroyed(int basesLeft) override;
        
        virtual int GetScore() const override
        {
            return 5;
        }

    private:
        enum State 
        {
            Roaming, // Roaming state: The enemy will roam around randomly looking for a player object.
            Locked,  // Locked: The enemy has found a player object and will now try to follow it.
            Fleeing, // Fleeing: The enemy will try to flee away from players.
        };

        int relaxed; // The odds of the enemy object will change direction each frame
        State state;
    };
}
