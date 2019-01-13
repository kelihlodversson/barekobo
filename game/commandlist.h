#pragma once
#include <circle/types.h>
#include <circle/net/socket.h>

#include "util/vector.h"
#include "util/rect.h"
#include "util/array.h"
#include "util/list.h"
#include "ui/minimap.h"
#include "ui/messageoverlay.h"


namespace hfh3
{
    /** A simple class for storing a sequence of actions to be run at a later time.
      * Commands are represented by virtual subclasses that are pushed to the end of a 
      * list. They can either be executed in turn or serialized and later restored 
      * from a byte array
      */
    class CommandList
    {
    public:
        CommandList(class ImageSheet& inImageSheet) 
            : imageSheet(inImageSheet)
            , readOffset(0)
            , hasBeenRun(false)
        {
        }

        ~CommandList()
        {
            Clear();
        }

        // Methods for building the command buffer
        void SetViewOffset(const Vector<s16>& position);
        void DrawBackground();
        void DrawSprite(const Vector<s16>& position, u8 imageGroup, u8 subImage);
        void SetPlayerPositions(const Vector<s16>& p0, const Vector<s16>& p1);
        void SetBackgroundCell(const Vector<u8>& pos, u8 imageGroup, u8 subImage);
        void ClearBackgroundCell(const Vector<u8>& pos);
        void SetPlayerScore(u8 player, int score);
        void SetPlayerLives(u8 player, int lives);
        void SetMessage(Message message, s16 level, s16 timeout);
        void Clear();

        // This method will execute the buffered commands
        void Run(class View& view, class Background& backround, 
                 MessageOverlay* overlay, MiniMap* map);

        // Utility methods for sending and receiving command buffers
        void Send (CSocket* stream, bool wait=false);
        bool Receive (CSocket* stream);
        int Size() const { return commands.Size(); }
       
    private:
        class ImageSheet& imageSheet;
        List<class Command*> commands;
        Array<u8> serialized;
        int readOffset;
        volatile bool hasBeenRun;
    };

}