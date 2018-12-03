#include "game/commandlist.h"
#include "game/view.h"
#include "game/background.h"

#include "render/image.h"
#include "render/imagesheet.h"

#include "util/log.h"
#include "util/serialization.h"

#include <circle/net/socket.h>
#include <circle/net/in.h>
#include <circle/sched/scheduler.h>

namespace hfh3 {

struct CommandContext 
{
    ImageSheet& imageSheet;
    View& view;
    Background& backround;
    MiniMap* map;
};

// Representation of game coordinates, which can be packed into
// 12 bits per component, or 3 bytes.
class VectorU12 
    : public Vector<s16>
    , public ISerializable
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

    virtual void Serialize(ISerializer& serializer) override 
    {
        u8 tmp[3];
        if (serializer.GetDirection() == ISerializer::Output)
        {
            tmp[0] = x & 0xff;
            tmp[1] = (x >> 4) & 0xf0;
            tmp[1] |= y & 0x0f;
            tmp[2] = (y >> 4) & 0xff;
        }

        serializer.Serialize(tmp, 3);
        if (serializer.GetDirection() == ISerializer::Input)
        {
            x = tmp[0] | ((tmp[1] & 0xf0) << 4);
            y = (tmp[1] & 0x0f) | (tmp[2] << 4);
        }
    }
};

enum class Opcode : u8
{
    SetViewOffset,
    DrawBackground,
    DrawSprite,
    SetPlayerPositions,
    SetBackgroundCell,
    ClearBackgroundCell,
    SetPlayerStat,
    FrameStart = 0xff
};

struct Command : public ISerializable
{
    Command(Opcode inOp)
        : opcode(inOp)
    {}

    virtual ~Command()
    {}

    virtual void Run(CommandContext& context) = 0;
    virtual void Serialize(ISerializer& serializer) override 
    {
        if(serializer.GetDirection() == ISerializer::Output)
        {
            Opcode tmp = opcode;
            hfh3::Serialize(serializer, tmp);
        }
        // We assume when deserializing that the opcode has already been read from the 
        // stream.
    }

    static Command* Parse(ArrayReader&);

    const Opcode opcode;
};

struct FrameStart : public Command
{
    FrameStart(s32 inSize = -1)
        : Command(Opcode::FrameStart)
        , size(inSize)
    {}

    virtual void Run(CommandContext& context) override
    {
        assert(0);
    }

    virtual void Serialize(ISerializer& serializer) override 
    {
        Command::Serialize(serializer);
        hfh3::Serialize(serializer, size);
    }

    void PatchSize(Array<u8>& array, s32 newSize)
    {

        // Read the existing value. Will assert if the array does not start with a 
        // FrameStart object.
        FrameStart existing;
        Opcode op;
        ArrayReader reader(array);
        hfh3::Serialize(reader, op);
        assert(op == Opcode::FrameStart);
        existing.Serialize(reader);

        // Assert we are modifying our own value
        assert(existing.size == size);

        // Update the value in the local copy and serialize it to a temporary array
        size = newSize;
        Array<u8> tmp;
        ArrayWriter writer(tmp);
        Serialize(writer);

        // Overwrite the header with the updated value
        memcpy((u8*)array, (u8*)tmp, tmp.Size());

    }

    s32 size;
};

struct SetViewOffset : public Command
{
    SetViewOffset()
        : Command(Opcode::SetViewOffset)
    {}

    SetViewOffset(const Vector<s16>& inPosition)
        : Command(Opcode::SetViewOffset)
        , position(inPosition)
    {}

    virtual void Run(CommandContext& context) override
    {
        context.view.SetOffset(position);
    }

    virtual void Serialize(ISerializer& serializer) override 
    {
        Command::Serialize(serializer);
        hfh3::Serialize(serializer, position);
    }

    // We use VectorU12 as view offset coordinates can be packed into 12 bits per component
    VectorU12 position; 
};

void CommandList::SetViewOffset(const Vector<s16>& position)
{
    commands.Append(new hfh3::SetViewOffset(position));
}

struct DrawBackground : public Command
{
    DrawBackground()
        : Command(Opcode::DrawBackground)
    {}

    virtual void Run(CommandContext& context) override
    {
        context.backround.Draw(context.view);
    }
};

void CommandList::DrawBackground()
{
    commands.Append(new hfh3::DrawBackground());
}


struct DrawSprite : public Command
{
    DrawSprite()
        : Command(Opcode::DrawSprite)
    {}

    DrawSprite(const Vector<s16>& inPosition, u8 imageGroup, u8 subImage)
        : Command(Opcode::DrawSprite)
        , position(inPosition)
        , image((imageGroup<<4) | (subImage&0xF))
    {}

    virtual void Run(CommandContext& context) override
    {
        context.view.DrawImage(position, context.imageSheet[image >> 4][image & 0xF]);
    }

    virtual void Serialize(ISerializer& serializer) override 
    {
        Command::Serialize(serializer);
        hfh3::Serialize(serializer, position);
        hfh3::Serialize(serializer, image);
    }

    // We use VectorU12 as view offset coordinates can be packed into 12 bits per component
    VectorU12 position;
    u8 image;
};

void CommandList::DrawSprite(const Vector<s16>& position, u8 imageGroup, u8 subImage)
{
    commands.Append(new hfh3::DrawSprite(position, imageGroup, subImage));
}

struct SetPlayerPositions : public Command
{
    SetPlayerPositions()
        : Command(Opcode::SetPlayerPositions)
    {}

    SetPlayerPositions(const Vector<s16>& p0, const Vector<s16>& p1)
        : Command(Opcode::SetPlayerPositions)
        , player0(p0)
        , player1(p1)
    {}

    virtual void Run(CommandContext& context) override
    {
        context.map->SetPlayerPosition(0, player0);
        context.map->SetPlayerPosition(1, player1);
    }

    virtual void Serialize(ISerializer& serializer) override 
    {
        Command::Serialize(serializer);
        hfh3::Serialize(serializer, player0);
        hfh3::Serialize(serializer, player1);
    }

    // We use VectorU12 as game pixel coordinates can be packed into 12 bits per component
    VectorU12 player0;
    VectorU12 player1;
};

void CommandList::SetPlayerPositions(const Vector<s16>& p0, const Vector<s16>& p1)
{
    commands.Append(new hfh3::SetPlayerPositions(p0, p1));
}

struct SetBackgroundCell : public Command
{
    SetBackgroundCell()
        : Command(Opcode::SetBackgroundCell)
    {}

    SetBackgroundCell(const Vector<u8>& inPosition, u8 imageGroup, u8 subImage)
        : Command(Opcode::SetBackgroundCell)
        , position(inPosition)
        , image((imageGroup<<4) | (subImage&0xF))
    {}

    virtual void Run(CommandContext& context) override
    {
        context.backround.SetCell(position, image >> 4, image & 0xF);
    }

    virtual void Serialize(ISerializer& serializer) override 
    {
        Command::Serialize(serializer);
        hfh3::Serialize(serializer, position);
        hfh3::Serialize(serializer, image);
    }

    Vector<u8> position;
    u8 image;
};

void CommandList::SetBackgroundCell(const Vector<u8>& pos, u8 imageGroup, u8 subImage)
{
    commands.Append(new hfh3::SetBackgroundCell(pos, imageGroup, subImage));
}

struct ClearBackgroundCell : public Command
{
    ClearBackgroundCell()
        : Command(Opcode::ClearBackgroundCell)
    {}

    ClearBackgroundCell(const Vector<u8>& inPosition)
        : Command(Opcode::ClearBackgroundCell)
        , position(inPosition)
    {}

    virtual void Run(CommandContext& context) override
    {
        context.backround.ClearCell(position);
    }

    virtual void Serialize(ISerializer& serializer) override 
    {
        Command::Serialize(serializer);
        hfh3::Serialize(serializer, position);
    }

    Vector<u8> position;
};


void CommandList::ClearBackgroundCell(const Vector<u8>& pos)
{
    commands.Append(new hfh3::ClearBackgroundCell(pos));
}

struct SetPlayerStat : public Command
{
    enum PlayerStat
    {
        Score = 0,
        Lives = 16,
    };

    SetPlayerStat()
        : Command(Opcode::SetPlayerStat)
    {}

    SetPlayerStat(u8 player, PlayerStat which, s32 inValue)
        : Command(Opcode::SetPlayerStat)
        , stat(which | player)
        , value(inValue)
    {}

    virtual void Run(CommandContext& context) override
    {
        switch (stat & 0xf0)
        {
            case Score:
            context.map->SetPlayerScore(stat &0x0f, value);
            break;
            case Lives:
            context.map->SetPlayerLives(stat &0x0f, value);
            break;
            default:
            assert(!"Unknown Stat");
            break;
        }
    }

    virtual void Serialize(ISerializer& serializer) override 
    {
        Command::Serialize(serializer);
        hfh3::Serialize(serializer, stat);
        hfh3::Serialize(serializer, value);
    }

    u8 stat;
    s32 value;
};

void CommandList::SetPlayerScore(u8 player, s32 score)
{
    commands.Append(new hfh3::SetPlayerStat(player, SetPlayerStat::Score, score));
}

void CommandList::SetPlayerLives(u8 player, s32 lives)
{
    commands.Append(new hfh3::SetPlayerStat(player, SetPlayerStat::Lives, lives));
}

Command* Command::Parse(ArrayReader& reader)
{
    Opcode op;
    Command* result;
    hfh3::Serialize(reader, op);
    switch(op)
    {
        case Opcode::SetViewOffset:
            result = new SetViewOffset;
        break;
        case Opcode::DrawBackground:
            result = new DrawBackground;
        break;
        case Opcode::DrawSprite:
            result = new DrawSprite;
        break;
        case Opcode::SetPlayerPositions:
            result = new SetPlayerPositions;
        break;
        case Opcode::SetBackgroundCell:
            result = new SetBackgroundCell;
        break;
        case Opcode::ClearBackgroundCell:
            result = new ClearBackgroundCell;
        break;
        case Opcode::SetPlayerStat:
            result = new SetPlayerStat;
        break;
        case Opcode::FrameStart:
            result = new FrameStart;
        break;
        default:
        {
            ERROR("Invalid command index %x", (u8)op);
            return nullptr;
        }    
    }
    result->Serialize(reader);
    return result;
}


void CommandList::Run(class View& view, Background& background, MiniMap* map)
{
    CommandContext context {imageSheet, view, background, map};
    for(Command* command : commands)
    {
        command->Run(context);
    }
    hasBeenRun = true;
}

void CommandList::Send (CSocket* stream, bool wait)
{
    hfh3::FrameStart header;
    serialized.ClearFast();
    readOffset=0;

    ArrayWriter writer(serialized);
    header.Serialize(writer); // Reserve space for the frame header
    // Serialize the actual commands
    for(Command* command : commands)
    {
        command->Serialize(writer);
    }

    // Write the correct byte size into the frame header
    header.PatchSize(serialized, serialized.Size());

    // Send the finished packet to the client.
    stream->Send(serialized, serialized.Size(), wait?0:MSG_DONTWAIT);
}

void CommandList::Clear ()
{
    // Commands are stored as pointers, so we need to free them
    for(Command*& command: commands)
    {
        delete command;

        // Save a null pointer back to the list item
        command = nullptr;
    }
    commands.ClearFast();
    hasBeenRun = false;    
}

bool CommandList::Receive (CSocket* stream)
{
    u8 tmp[FRAME_BUFFER_SIZE];

    int count = stream->Receive(tmp, FRAME_BUFFER_SIZE, MSG_DONTWAIT);
    CompilerBarrier();

    if (count > 0)
    {
        if (serialized.Size() == 0)
        {
            assert(tmp[0] == u8(Opcode::FrameStart));
        }
        serialized.AppendRaw(tmp, count);

        if(serialized.Size() < 5)
        {
            return true;
        }

        Command* command;
        ArrayReader reader(serialized, readOffset);
        for(int remaining = reader.GetRemaining(); remaining > 0; remaining = reader.GetRemaining())
        {
            // Save the current offset
            readOffset = reader.GetOffset();

            // Read the current command from the stream
            command = Command::Parse(reader);

            // If it's a frame start header, verify that we have enough data
            if (command->opcode == Opcode::FrameStart)
            {
                // Verify frame header
                auto header = reinterpret_cast<hfh3::FrameStart*>(command);
                
                // Stop parsing commands if we haven't received the entire frame yet.
                if (header->size > remaining)
                {
                    return true;
                }
                // If previously parsed frames have been executed, clear the command list
                else if (hasBeenRun)
                {
                    Clear();
                }
            }
            // Append all other command objects to the command list
            else
            {
                commands.Append(command);
            }
        }

        // If we have parsed the entire read buffer, clear it
        serialized.ClearFast();
        readOffset = 0;
    }
 
    return count >= 0;
}


}

