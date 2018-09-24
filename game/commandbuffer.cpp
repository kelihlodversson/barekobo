#include "game/commandbuffer.h"

#include "game/view.h"
#include "game/starfield.h"

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
};

typedef Array<u8> CommandArray;
typedef CommandArray::Iterator CommandIterator;

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

static void operator >> (CommandIterator& iter, s16& value)
{
    u8 tmp;
    iter >> tmp; value |= tmp;
    iter >> tmp; value |= tmp << 8;
}

static void operator << (CommandArray& array, s16 value)
{
    array << static_cast<u8>(((value >>  0) & 0xff));
    array << static_cast<u8>(((value >>  8) & 0xff));
}

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

void CommandBuffer::SetViewOffset(const Vector<s16>& position)
{
    commands << Opcode::SetViewOffset;
    commands << position;
}

void CommandBuffer::DrawBackground()
{
    commands << Opcode::DrawBackground;
}

void CommandBuffer::DrawSprite(const Vector<s16>& position, u8 imageGroup, u8 subImage)
{
    commands << Opcode::DrawSprite;
    commands << position;
    commands << u8((imageGroup<<4)|(subImage&0xF));
}

void CommandBuffer::Run(class View& view, Starfield& background)
{
    Opcode op = Opcode::DrawBackground;
    for(auto iter = commands.begin(); iter != commands.end();)
    {
        iter >> op;
        switch(op)
        {
            case Opcode::SetViewOffset:
            {
                Vector<s16> offset;
                iter >> offset;
                view.SetOffset(offset);
            }
            break;
            case Opcode::DrawSprite:
            {
                Vector<s16> position;
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
            default:
            {
                ERROR("Invalid command index %x", (u8)op);
                iter = commands.end();
            }
            break;
        }
    }
}

// Utility methods for sending and receiving command buffers
void CommandBuffer::Send (CSocket* stream)
{
    stream->Send(&commands[0], commands.Size(), MSG_DONTWAIT);
}

bool CommandBuffer::Receive (CSocket* stream)
{
    static const int read_size = FRAME_BUFFER_SIZE;
    u8 tmp[read_size];
    int count;
    count = stream->Receive(tmp, read_size, MSG_DONTWAIT);
    if(count)
    {
        // TODO: There is no indication in the data where one frame
        // starts and the other ends in case we need to resynchronize
        // the stream after transmission errors.
        commands.ClearFast(); 
        commands.AppendRaw(tmp, count);
        return true;
    }
    return false;
}
