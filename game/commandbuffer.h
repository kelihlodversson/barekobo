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
        CommandBuffer(class ImageSheet& inImageSheet) 
            : imageSheet(inImageSheet)
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
        void SetBackgroundCell(const Vector<u8>& pos, u8 imageGroup, u8 subImage);
        void ClearBackgroundCell(const Vector<u8>& pos);
        void Clear() 
        {
            commands.ClearFast();
            FrameStart(0);
            hasBeenRun = false;
        }

        // This method will execute the buffered commands
        void Run(class View& view, class Background& backround, class MiniMap* map);

        // Utility methods for sending and receiving command buffers
        void Send (CSocket* stream, bool wait=false);
        bool Receive (CSocket* stream);
        
        
       
    private:
        int GetExpectedSize();
        void PatchSize();
        class ImageSheet& imageSheet;
        Array<u8> commands;
        volatile bool hasBeenRun;
    };

}