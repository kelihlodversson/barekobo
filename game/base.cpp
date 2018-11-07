#include "game/base.h"
#include "game/imagesets.h"
#include "game/stage.h"
#include "game/gameserver.h"
#include "render/image.h"
#include "render/imagesheet.h"
#include "util/random.h"
#include "util/array.h"
#include "util/log.h"

using namespace hfh3;

static const ImageSet misc = ImageSet::Fort0;
static const ImageSet edges[2] = {
    ImageSet::Fort1, ImageSet::Fort2
};

Base::Base(GameServer& inWorld, ImageSheet& imageSheet) :
    Sprite(inWorld,
          (u8)edges[0],
          imageSheet.GetGroupSize(),
          CollisionMask::EnemyBase, CollisionMask::None),
    north(nullptr),
    east(nullptr),
    south(nullptr),
    west(nullptr),
    destructible(false),
    delayAction(None),
    delay(2)
{
}

void Base::Update() 
{
    if(delayAction != None && --delay <= 0)
    {
        Destroy(delayAction);
    }
}

void Base::OnCollision(class Actor* other)
{
    if(destructible)
    {
        Destroy(this == core ? Core : Leaf);
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


void Base::Destroy(DestructionType type)
{
    Array<Base*> needsUpdate;
    Sprite::Destroy();
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
        if (type == Core || (base != core && base->EdgeCount() <= 1) || base->EdgeCount() == 0)
        {
            base->delayAction = type;
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
        SetImageGroup((u8)misc);
        SetImageIndex(7);
        destructible = true;
    }
    else
    {
        u8 subImage = (north ? 1 : 0 )
                    | (east  ? 2 : 0 )
                   | (south ? 4 : 0 ) ;
        u8 group = (u8)edges[west ? 1 : 0];

        SetImageGroup(group);
        SetImageIndex(subImage);
        
        // The base node is destructible if there is only one edge connected to this node.
        destructible = EdgeCount() == 1;
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

void Base::CreateFort(GameServer& server, ImageSheet& imageSheet, Random& random, Rect<s16>& area)
{
    
    // The grid size is the size of the area divided by 16 rounded to the nearest integer and then
    // to the nearest odd number.
    Vector<s16> gridSize = ((area.size + Vector<s16>(15,15)) / 32)  * 2 + Vector<s16>(1,1);

    DEBUG("Grid size: %d,%d", gridSize.x, gridSize.y);

    // Update the size of the rectangle to fit to the rounded grid size:
    area.size = gridSize * 16;

    // Get the center of the grid and the area
    Vector<s16> start = gridSize / 2; 
    DEBUG("Start: %d,%d", start.x, start.y);

    Grid grid(gridSize); // Allocate a grid of null pointers
    Array<Vector<s16>> stack; // Temporary stack of nodes to visit later

    // Create the initial node
    grid[start] = new Base(server, imageSheet);

    
    // Add the initial nodes above and below or left and right of the core to the stack
    {
        if(random.Get() & 0x800000)
        {
            stack.Push(start - halfV);
            stack.Push(start + halfV);
        }
        else
        {
            stack.Push(start - halfH);
            stack.Push(start + halfH);
        }

        for(const auto& v : stack)
        {
            grid[v] = new Base(server, imageSheet); 
        }
    }


    Array<Vector<s16>> directions;
    while(!stack.IsEmpty())
    {
        // pop a random item from the stack
        auto current = stack.Pop(random.Get() % stack.Size());
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
            // The current node is already full, move to next one on the stack  
            continue;
        }

        // Get the next coordinate
        auto next = directions[random.Get() % directions.Size()];
        auto midpoint = (next + current) / 2;
        
        // Create a corridor node between the current and the next node
        grid[midpoint] = new Base(server, imageSheet);

        // Create the new node and push it to the stack
        assert(grid[next] == nullptr);
        grid[next] = new Base(server, imageSheet);
        stack.Push(next);

        // Return the current node to the stack
        stack.Push(current);
    }

    // Finalize the maze and add the base elements to the current game
    Vector<s16> v(0,0);
    for (v.y = 0; v.y < gridSize.y; v.y++)
    {
        Array<u8> tmp;
        for(v.x = 0; v.x < gridSize.x; v.x++)
        {
            if( grid[v] )
            {
                tmp.Append('#');
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
                grid[v]->UpdateShape();
                grid[v]->SetPosition(v*16 + area.origin);

                server.AddActor(grid[v]);
            }        
            else 
            {
                tmp.Append('.');
            }
        }
        tmp.Append(0);
        DEBUG("%s", (u8*)tmp);

    }
}