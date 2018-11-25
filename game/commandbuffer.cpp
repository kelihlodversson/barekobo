#include "game/commandbuffer.h"

#include "game/view.h"
#include "game/background.h"

#include "util/log.h"
#include "render/image.h"
#include "render/imagesheet.h"

#include <circle/net/socket.h>
#include <circle/net/in.h>
#include <circle/sched/scheduler.h>

using namespace hfh3;

enum class Opcode : u8
{
    SetViewOffset,
    DrawBackground,
    DrawSprite,
    SetPlayerPositions,
    SetBackgroundCell,
    ClearBackgroundCell,
    FrameStart = 0xff
};

using CommandArray = Array<u8>;
using CommandIterator = CommandArray::Iterator;

static void operator >> (CommandIterator& iter, u8& value)
{
    value = *(iter++);
}

static void operator << (CommandArray& array, u8 value)
{
    array.Append(value);
}

static void operator >> (CommandIterator& iter, Opcode& value)
{
    value = static_cast<Opcode>(*(iter++));
}

static void operator << (CommandArray& array, Opcode value)
{
    array << static_cast<u8>(value);
}

// Representation of game coordinates, which can be packed into
// 12 bits per component, or 3 bytes.
class VectorU12 : public Vector<s16>
{
public:
    template<typename T>
    VectorU12(const Vector<T>& other)
        : Vector<s16>(other)
    {}

    VectorU12()
    {}

    template<typename T>
    operator Vector<T>()
    {
        return Vector<T>(static_cast<Vector<s16>>(*this));
    }

    friend void operator << (CommandArray& array, const VectorU12& value)
    {
        u8 x_ls8, x_ms4, y_ls4, y_ms8;
        x_ls8 = value.x & 0xff;
        x_ms4 = (value.x >> 4) & 0xf0;
        y_ls4 = value.y & 0x0f;
        y_ms8 = (value.y >> 4) & 0xff;

        array << x_ls8;
        array << u8(x_ms4 | y_ls4);
        array << y_ms8;
    }

    friend void operator >> (CommandIterator& iter, VectorU12& value)
    {
        u8 x_ls8, x_ms4_y_ls4, y_ms8;
        iter >> x_ls8;
        iter >> x_ms4_y_ls4;
        iter >> y_ms8;

        value.x = x_ls8 | ((x_ms4_y_ls4 & 0xf0) << 4);
        value.y = (x_ms4_y_ls4 & 0x0f) | (y_ms8 << 4);
    }
};

template<typename T>
static void operator >> (CommandIterator& iter, Vector<T>& value)
{
    iter >> value.x;
    iter >> value.y;
}

template<typename T>
static void operator << (CommandArray& array, const Vector<T>& value)
{
    array << value.x;
    array << value.y;
}

static void operator >> (CommandIterator& iter, s32& value)
{
    u8 tmp;
    iter >> tmp; value |= tmp;
    iter >> tmp; value |= tmp << 8;
    iter >> tmp; value |= tmp << 16;
    iter >> tmp; value |= tmp << 24;
}

static void operator << (CommandArray& array, s32 value)
{
    array << u8((value >>  0) & 0xff);
    array << u8((value >>  8) & 0xff);
    array << u8((value >> 16) & 0xff);
    array << u8((value >> 24) & 0xff);
}

void CommandBuffer::FrameStart(s32 size)
{
    commands << Opcode::FrameStart;
    commands << size;
}

void CommandBuffer::PatchSize()
{
    s32 size = commands.Size();
    assert (size >= 5);
    assert (commands[0] == static_cast<u8>(Opcode::FrameStart));
    commands[1] = u8((size >>  0) & 0xff);
    commands[2] = u8((size >>  8) & 0xff);
    commands[3] = u8((size >> 16) & 0xff);
    commands[4] = u8((size >> 24) & 0xff);
}

void CommandBuffer::SetViewOffset(const Vector<s16>& position)
{
    commands << Opcode::SetViewOffset;
    commands << VectorU12(position);
}

void CommandBuffer::DrawBackground()
{
    commands << Opcode::DrawBackground;
}

void CommandBuffer::DrawSprite(const Vector<s16>& position, u8 imageGroup, u8 subImage)
{
    commands << Opcode::DrawSprite;
    commands << VectorU12(position);
    commands << u8((imageGroup<<4)|(subImage&0xF));
}

void CommandBuffer::SetPlayerPositions(const Vector<s16>& p0, const Vector<s16>& p1)
{
    commands << Opcode::SetPlayerPositions;
    commands << VectorU12(p0);
    commands << VectorU12(p1);
}

void CommandBuffer::SetBackgroundCell(const Vector<u8>& pos, u8 imageGroup, u8 subImage)
{
    commands << Opcode::SetBackgroundCell;
    commands << pos;
    commands << u8((imageGroup<<4)|(subImage&0xF));
}

void CommandBuffer::ClearBackgroundCell(const Vector<u8>& pos)
{
    commands << Opcode::ClearBackgroundCell;
    commands << pos;
}


void CommandBuffer::Run(class View& view, Background& background, MiniMap* map)
{
    Opcode op = Opcode::DrawBackground;
    int retryCount = 4;
    for(auto iter = commands.begin(); iter < commands.end();)
    {
        int offset = (u8*)iter - (u8*)commands;
        iter >> op;
        switch(op)
        {
            case Opcode::FrameStart:
            {
                s32 size = 0;
                iter >> size;
                // Stop executing the commands if we encounter a frame start indicating
                // that there are more commands left than we have in the array.
                if (size+offset > commands.Size())
                {

                    if(!offset && retryCount--) 
                    {
                        // Yield the thread and try again from when more data has arrived
                        CScheduler::Get()->MsSleep(2); 
                        CompilerBarrier();
                        iter = commands.begin();
                    }
                    else
                    {
                        // Return immediately so hasRun will not be set to true.
                        return;
                    }

                }
                #if 0
                else if (offset)
                {
                    // If we have a complete frame not at the beginning of the
                    // buffer. Trim of the old data and contintue.
                    s32 sameSize;
                    commands.RemoveFront(offset);

                    // Since we've moved everything, we have to reset the iterator
                    iter = commands.begin();
                    iter >> op;
                    iter >> sameSize;

                    // Sanity check that we still have the same data now at the head
                    // of the command list.
                    assert(op == Opcode::FrameStart);
                    if(size != sameSize)
                    {
                        ERROR("%d != %d", size, sameSize);
                        assert(size == sameSize);
                    }
                }
                #endif
            }
            break;
            case Opcode::SetViewOffset:
            {
                VectorU12 offset;
                iter >> offset;
                view.SetOffset(offset);
            }
            break;
            case Opcode::DrawSprite:
            {
                VectorU12 position;
                u8 image;
                iter >> position;
                iter >> image;
                view.DrawImage(position, imageSheet[image >> 4][image & 0xF]);
            }
            break;
            case Opcode::DrawBackground:
            {
                background.Draw(view);
            }
            break;
            case Opcode::SetPlayerPositions:
            {
                VectorU12 p0;
                VectorU12 p1;
                iter >> p0;
                iter >> p1;
                if (map)
                {
                    map->SetPlayerPosition(0, p0);
                    map->SetPlayerPosition(1, p1);
                }
            }
            break;
            case Opcode::SetBackgroundCell:
            {
                Vector<u8> position;
                u8 image;
                iter >> position;
                iter >> image;
                background.SetCell(position, image >> 4, image & 0xF);
            }
            break;
            case Opcode::ClearBackgroundCell:
            {
                Vector<u8> position;
                iter >> position;
                background.ClearCell(position);
            }
            break;
            default:
            {
                ERROR("Invalid command index %x offset %d. (size %d)", (u8)op, offset, commands.Size());
                iter = commands.end();
            }
            break;
        }
    }
    hasBeenRun = true;
}

// Utility methods for sending and receiving command buffers
void CommandBuffer::Send (CSocket* stream, bool wait)
{
    PatchSize();
    stream->Send(commands, commands.Size(), wait?0:MSG_DONTWAIT);
}

int CommandBuffer::GetExpectedSize()
{
    int size = 0;
    if (commands.Size() >= 5)
    {
        CommandIterator iter = commands.begin();
        Opcode op;
        iter >> op;
        if (op == Opcode::FrameStart)
        {
            iter >> size;
        }
    }
    return size;
}

bool CommandBuffer::Receive (CSocket* stream)
{
    u8 tmp[FRAME_BUFFER_SIZE];

    int count = stream->Receive(tmp, FRAME_BUFFER_SIZE, MSG_DONTWAIT);
    CompilerBarrier();

    if (count > 0)
    {
        if (hasBeenRun && tmp[0] == u8(Opcode::FrameStart))
        {
            commands.ClearFast();
        }
        commands.AppendRaw(tmp, count);
        hasBeenRun = false;
    }

    return count > 0;
}
