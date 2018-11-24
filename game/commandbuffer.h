#pragma once
#include <circle/types.h>
#include <circle/net/socket.h>

#include "util/vector.h"
#include "util/rect.h"
#include "util/array.h"
#include "ui/minimap.h"


namespace hfh3
{
    /** A simple class for storing a sequence of actions to be run at a later time.
      * Commands are pushed to a byte array and later executed in sequence by calling Run(). 
      */
    class CommandBuffer
    {
    public:
        CommandBuffer(class ImageSheet& inImageSheet, class MiniMap* inMap) 
            : imageSheet(inImageSheet)
            , map(inMap)
            , hasBeenRun(false)
        {
            FrameStart(0);
        }

        // Methods for building the command buffer
        void SetViewOffset(const Vector<s16>& position);
        void FrameStart(s32 size);
        void DrawBackground();
        void DrawSprite(const Vector<s16>& position, u8 imageGroup, u8 subImage);
        void SetPlayerPositions(const Vector<s16>& p0, const Vector<s16>& p1);
        void PlotMap(const Vector<s16>& pos, MiniMap::EntryType type);
        void Clear() 
        {
            commands.ClearFast();
            FrameStart(0);
            hasBeenRun = false;
        }

        // This method will execute the buffered commands
        void Run(class View& view, class Starfield& backround);

        // Utility methods for sending and receiving command buffers
        void Send (CSocket* stream);
        bool Receive (CSocket* stream);
        
        
       
    private:
        int GetExpectedSize();
        void PatchSize();
        class ImageSheet& imageSheet;
        class MiniMap* map;
        Array<u8> commands;
        volatile bool hasBeenRun;
    };

}