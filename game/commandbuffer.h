#pragma once
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

        void SetViewOffset(const Vector<int>& position);
        void DrawBackground();
        void DrawSprite(const Vector<int>& position, u8 imageGroup, u8 subImage);

        void Run(class View& view, class Starfield& backround);
    private:
        class ImageSheet& imageSheet;
        Array<u8> commands;
    };
}