#include "game/base.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "game/gameserver.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"
#include "util/array.h"
#include "util/log.h"
#include "ui/minimap.h"

#include <limits.h>


using namespace hfh3;

static const ImageSet misc = ImageSet::Fort0;
static const ImageSet edges[2] = {
    ImageSet::Fort1, ImageSet::Fort2
};

Base::Base(GameServer& inWorld, Vector<s16> position, bool inCore) :
    Actor(inWorld,
          CollisionMask::EnemyBase, CollisionMask::None),
    isCore(inCore),
    north(nullptr),
    east(nullptr),
    south(nullptr),
    west(nullptr),
    destructible(false),
    delayAction(None),
    delay(60)
{
    SetPosition(position);
}

void Base::Update() 
{
    if(delayAction != None && --delay <= 0)
    {
        switch(delayAction)
        {
            case DestroyCore:
            case DestroyLeaf:
                Destroy(delayAction);
            break;
            case SpawnShot:
                Spawn(delayAction);
            break;
            default:
            break;
        }
    }
}

void Base::OnCollision(class Actor* other)
{
    if(destructible)
    {
        SetKiller(other->GetOwner());
        Destroy(isCore ? DestroyCore : DestroyLeaf);
    }
}

int Base::EdgeCount()
{
    int result = 0;
    Base* const siblings[4] = {north, east, south, west};
    for(Base* sibling : siblings)
    {
        if (sibling)
        {
            result ++;
        }
    }
    return result; 
}

Direction Base::MaskToDirection(u8 mask)
{
    switch(mask)
    {
        case 1:
            return Direction::North;
        case 2:
            return Direction::East;
        case 4:
            return Direction::South;
        case 8: 
            return Direction::West;
        case 3: // 1 + 2
            return Direction::NorthEast;
        case 6: // 2 + 4
            return Direction::SouthEast;
        case 9: // 1 + 8
            return Direction::NorthWest;
        case 12: // 4 + 8
            return Direction::SouthWest;
        case 7: // 1 + 2 + 4
            return Direction::East;
        case 11: // 1 + 2 + 8
            return Direction::North;
        case 13: // 1 + 4 + 8
            return Direction::West;
        case 14: // 2 + 4 + 8
            return Direction::South;
        default:
            return Direction::Stopped;
    }
}

void Base::Spawn(Action type)
{
    const int closeRange = 160*160;
    Vector<s16> myPosition = GetPosition();
    Vector<s16> delta;

    if(world.FindPlayer(myPosition, SHRT_MAX, delta))
    {
        Direction dir (delta);
        int sqrDistance = Vector<s32>(delta).SqrMagnitude();
        bool isCloseRange = sqrDistance <= closeRange;
        // If player is in close range, fire a shot 9 out of ten times
        if (isCloseRange && Rand() % 10 > 0)
        {
            world.SpawnShot(myPosition, dir, 1);
        }
        // 1 out of 10 close range, spawn an enemy.
        // Else spawn an enemy with a probability inverse of the distance squared.
        else if(isCloseRange || Rand() % sqrDistance < 40*40)
        {
            world.SpawnEnemy(myPosition + dir.ToDelta(16));
        }
    }

    delay = (Rand()%240)+30;
    delayAction = type;
}

void Base::Destroy(Action type)
{
    Array<Base*> needsUpdate;
    Actor::Destroy();
    u8 neigbourMask = 0;

    if(north)
    {
        north->south = nullptr;
        needsUpdate.Push(north);
        neigbourMask |= 1;
    }
    if(east)
    {
        east->west = nullptr;
        needsUpdate.Push(east);
        neigbourMask |= 2;
    }
    if(south)
    {
        south->north = nullptr;
        needsUpdate.Push(south);
        neigbourMask |= 4;
    }
    if(west)
    {
        west->east = nullptr;
        needsUpdate.Push(west);
        neigbourMask |= 8;
    }
    world.SpawnExplosion(GetPosition(), MaskToDirection(neigbourMask), 2);

    for(Base* base : needsUpdate)
    {
        // Destroy all edges if this was a core hit, else all edgges that would become new leaves. 
        if (type == DestroyCore || (!base->isCore && base->EdgeCount() <= 1) || base->EdgeCount() == 0)
        {
            base->SetKiller(GetKiller());
            base->delayAction = type;
            base->delay = 2;
        }
        else 
        {
            base->UpdateShape();
        }
    }
}

// Select the correct image based on which siblings are connected
void Base::UpdateShape()
{
    if(isCore)
    {
        world.OnBaseChanged(this, u8(misc), 7);
        destructible = true;
    }
    else
    {
        u8 subImage = (north ? 1 : 0 )
                    | (east  ? 2 : 0 )
                    | (south ? 4 : 0 ) ;
        u8 group = (u8)edges[west ? 1 : 0];

        world.OnBaseChanged(this, group, subImage);

        // The base node is destructible if there is only one edge connected to this node.
        destructible = EdgeCount() == 1;
    }

    if(destructible)
    {
        delayAction = SpawnShot;
    }
}


/* Create a collection of base elements using a random algorithm similar to
 * the one used by the original xkobo.
 */
static const Direction moves[4] = {
    Direction::North,
    Direction::East,
    Direction::South,
    Direction::West
};

// Simple array wrapper for storing a 2d grid of Base pointers
struct Grid
{
    Grid(const Vector<s16>& inSize)
        : size(inSize)
    {
        data.Reserve(size.x * size.y);
        for(int i = 0 ; i< size.x * size.y; i++)
        {
            data.Append(nullptr);
        }
    }

    Base*& operator[](const Vector<s16>& v)
    {
        return data[v.x + size.x * v.y];
    }

    bool isFree(const Vector<s16>& v)
    {
        return // return true iff the grid coordinate is ..
             (v.x >= 0 && v.y >= 0 && v.x < size.x && v.y < size.y) && // within range, and
             (this->operator[](v) == nullptr); // not assigned
    }

    Array<Base*> data;
    Vector<s16> size;
};

Base* Base::CreateNeighbor(Direction dir)
{
    Base* other = new Base(world, GetPosition() + dir.ToDelta(16), false);
    switch (int(dir))
    {
        case Direction::North:
            assert(!north);
            north = other;
            other->south = this;
            break;
        case Direction::South:
            assert(!south);
            south = other;
            other->north = this;
            break;
        case Direction::East:
            assert(!east);
            east = other;
            other->west = this;
            break;
        case Direction::West:
            assert(!west);
            west = other;
            other->east = this;
            break;
        default:
            assert(1); // Invalid direction
    }
    world.AddBase(other);
    return other;
}


void Base::CreateFort(GameServer& server, const Rect<s16>& area)
{
    // The grid size is the size of the area divided by 32 rounded to the nearest integer.
    Vector<s16> gridSize = (area.size + Vector<s16>(31,31)) / 32 ;
    
    // Minimum grid size is 3x3
    if (gridSize.x < 3)
    {
        gridSize.x = 3;
    }
    if (gridSize.y < 3)
    {
        gridSize.y = 3;
    }

    DEBUG("Grid size: %d,%d", gridSize.x, gridSize.y);

    Grid grid(gridSize); // Allocate a grid of null pointers
    Array<Vector<s16>> set; // Working set of nodes to visit later

    // Pick a random position for the root node
    Vector<s16> start = Vector<s16>((Rand() % (gridSize.x-2))+1, (Rand() % (gridSize.y-2)) + 1); 
    DEBUG("Start: %d,%d", start.x, start.y);
    // Create the initial node
    Base* core = new Base(server, start*32 + area.origin);
    server.AddBase(core);
    grid[start] = core;
    set.Push(start);
    
    Array<Direction> directions;
    while(!set.IsEmpty())
    {
        // pull a random item from the set
        auto current = set.Pull(Rand() % set.Size());
        assert(grid[current] != nullptr);

        // Select an available direction to move in
        directions.ClearFast();
        for(auto move : moves)
        {
            auto next = move.ToDelta() + current;
            if(grid.isFree(next))
            {
                directions.Append(move);
            }
        }

        if (directions.IsEmpty())
        {
            // The current node is already full, finish initializing the node and move to next one on the set.  
            grid[current]->UpdateShape();
            continue;
        }

        // Get the next coordinate
        auto dir = directions[Rand() % directions.Size()];
        auto next = dir.ToDelta() + current;
        
        // Create a bridge node between the current and the next node
        Base* bridge = grid[current]->CreateNeighbor(dir);

        // Create the new node and push it to the set
        assert(grid[next] == nullptr);
        grid[next] = bridge->CreateNeighbor(dir);
        bridge->UpdateShape();
        set.Push(next);

        // Return the current node to the set
        set.Push(current);
    }

}

/** The drawing of bases is handled by the background class
 */
void Base::Draw(class CommandList&)
{

}

// The bounds of a base object are determined by the number of connections
// and whether it is destructible.
Rect<s16> Base::GetBounds()
{
    Rect<s16> result {GetPosition(), {16,16}};
    if(destructible)
    {
        return result;
    }

    if( !east )
    {
        result.size.x   -= 3;
    }

    if( !south )
    {
        result.size.y   -= 3;
    }

    if( !west )
    {
        result.origin.x += 3;
        result.size.x   -= 3;
    }

    if( !north )
    {
        result.origin.y += 3;
        result.size.y   -= 3;
    }
    return result;
}
