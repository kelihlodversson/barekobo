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

Base::Base(GameServer& inWorld) :
    Actor(inWorld,
          CollisionMask::EnemyBase, CollisionMask::None),
    north(nullptr),
    east(nullptr),
    south(nullptr),
    west(nullptr),
    destructible(false),
    delayAction(None),
    delay(60)
{
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
        Destroy(this == core ? DestroyCore : DestroyLeaf);
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
        if (type == DestroyCore || (base != core && base->EdgeCount() <= 1) || base->EdgeCount() == 0)
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
    if(core == this)
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
 * the one used by te original xkobo.
 */

static const Vector<s16> halfV (0,1);
static const Vector<s16> halfH (1,0);
static const Vector<s16> fullV (0,2);
static const Vector<s16> fullH (2,0);

static const Vector<s16> moves[4] = {
    fullV * -1,
    fullH,
    fullV,
    fullH * -1
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

    bool isValid(const Vector<s16>& v)
    {
        return // return true iff the grid coordinate is ..
             (v.x >= 0 && v.y >= 0 && v.x < size.x && v.y < size.y) && // within range, and
             (this->operator[](v) != nullptr); // is assigned
    }

    Array<Base*> data;
    Vector<s16> size;
};

void Base::CreateFort(GameServer& server, const Rect<s16>& area)
{
    // The grid size is the size of the area divided by 16 rounded to the nearest integer and then
    // to the nearest odd number.
    Vector<s16> gridSize = ((area.size + Vector<s16>(15,15)) / 32)  * 2 + Vector<s16>(1,1);
    
    // Minimum grid size is 5x5
    if (gridSize.x < 5)
    {
        gridSize.x = 5;
    }
    if (gridSize.y < 5)
    {
        gridSize.y = 5;
    }

    DEBUG("Grid size: %d,%d", gridSize.x, gridSize.y);


    // Get the center of the grid and the area
    Vector<s16> start = gridSize / 2; 
    DEBUG("Start: %d,%d", start.x, start.y);

    Grid grid(gridSize); // Allocate a grid of null pointers
    Array<Vector<s16>> set; // Temporary set of nodes to visit later

    // Create the initial node
    grid[start] = new Base(server);

    set.Push(start);
    
    Array<Vector<s16>> directions;
    while(!set.IsEmpty())
    {
        // pull a random item from the set
        auto current = set.Pull(Rand() % set.Size());
        assert(grid[current] != nullptr);

        // Select an available direction to move in
        directions.ClearFast();
        for(auto move : moves)
        {
            auto next = move + current;
            if(grid.isFree(next))
            {
                directions.Push(next);
            }
        }

        if (directions.IsEmpty())
        {
            // The current node is already full, move to next one on the set  
            continue;
        }

        // Get the next coordinate
        auto next = directions[Rand() % directions.Size()];
        auto midpoint = (next + current) / 2;
        
        // Create a corridor node between the current and the next node
        grid[midpoint] = new Base(server);

        // Create the new node and push it to the set
        assert(grid[next] == nullptr);
        grid[next] = new Base(server);
        set.Push(next);

        // Return the current node to the set
        set.Push(current);
    }

    // Finalize the maze and add the base elements to the current game
    Vector<s16> v(0,0);
    for (v.y = 0; v.y < gridSize.y; v.y++)
    {
        for(v.x = 0; v.x < gridSize.x; v.x++)
        {
            if( grid[v] )
            {
                if(grid.isValid(v-halfV))
                {
                    grid[v]->north = grid[v-halfV];
                }
                if(grid.isValid(v+halfH))
                {
                    grid[v]->east = grid[v+halfH];
                }
                if(grid.isValid(v+halfV))
                {
                    grid[v]->south = grid[v+halfV];
                }
                if(grid.isValid(v-halfH))
                {
                    grid[v]->west = grid[v-halfH];
                }
                grid[v]->core = grid[start];
                grid[v]->SetPosition(v*16 + area.origin);
                grid[v]->UpdateShape();

                server.AddBase(grid[v]);
            } 
        }
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
