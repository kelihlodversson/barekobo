#pragma once

namespace hfh3
{
    enum class CollisionMask : unsigned
    {
        None         = 0,
        EnemyNPC     = 1 << 0,
        EnemyBase    = 1 << 1,
        Player1      = 1 << 29,
        Player2      = 1 << 30,
        Enemy        = EnemyBase  | EnemyNPC,
        Player       = Player1    | Player2,
        Any          = ~0UL
    };

    static inline unsigned operator &(CollisionMask a, CollisionMask b)
    {
        return static_cast<unsigned>(a) & static_cast<unsigned>(b);
    }
}
