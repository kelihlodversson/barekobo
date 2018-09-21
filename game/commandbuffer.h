#pragma once
#include "circle/types.h"
#include <circle/net/socket.h>

#include "util/vector.h"
#include "util/rect.h"
#include "util/array.h"


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
        {

        }

        void SetViewOffset(const Vector<s16>& position);
        void DrawBackground();
        void DrawSprite(const Vector<s16>& position, u8 imageGroup, u8 subImage);

        void Run(class View& view, class Starfield& backround);

        // Utility methods for sending and receiving command buffers
        void Send (CSocket* stream);
        void Receive (CSocket* stream);
        
       
    private:
        class ImageSheet& imageSheet;
        Array<u8> commands;
    };

}