#include "game/commandbuffer.h"

#include "game/view.h"
#include "game/starfield.h"

#include "util/log.h"
#include "render/image.h"
#include "render/imagesheet.h"

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

static void operator >> (CommandIterator& iter, int& value)
{
    u8 tmp;
    iter >> tmp; value |= tmp;
    iter >> tmp; value |= tmp << 8;
    iter >> tmp; value |= tmp << 16;
    iter >> tmp; value |= tmp << 24;
}

static void operator << (CommandArray& array, int value)
{
    array << static_cast<u8>(((value >>  0) & 0xff));
    array << static_cast<u8>(((value >>  8) & 0xff));
    array << static_cast<u8>(((value >> 16) & 0xff));
    array << static_cast<u8>(((value >> 24) & 0xff));
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

void CommandBuffer::SetViewOffset(const Vector<int>& position)
{
    commands << Opcode::SetViewOffset;
    commands << position;
}

void CommandBuffer::DrawBackground()
{
    commands << Opcode::DrawBackground;
}

void CommandBuffer::DrawSprite(const Vector<int>& position, u8 imageGroup, u8 subImage)
{
    commands << Opcode::DrawSprite;
    commands << position;
    commands << imageGroup;
    commands << subImage;
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
                Vector<int> offset;
                iter >> offset;
                view.SetOffset(offset);
            }
            break;
            case Opcode::DrawSprite:
            {
                Vector<int> position;
                u8 imageGroup;
                u8 imageIndex;
                iter >> position;
                iter >> imageGroup;
                iter >> imageIndex;
                view.DrawImage(position, imageSheet[imageGroup][imageIndex]);
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
    commands.ClearFast();
}
