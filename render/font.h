#pragma once
#include <circle/types.h>
#include "util/vector.h"
#include "util/log.h"

namespace hfh3
{

    /** Representation of a simple 8xX bitmapped font. The font is
      * stored in a binary bitmap with all characters placed next to each other.
      */
    class Font
    {
    public:
        Font(const u8* inData, int inStride, int inHeight, char inOffset = 0);

        static Font& GetDefault();

        const Vector<s16>& GetSize(char c) const
        {
            return cellSize;
        }

        /** Returns whether a given pixel from a character is set or not */
        int GetPixel(char c, int x, int y) const
        {
            if (c < offset || c-offset > stride)
                return 1;
            if (x < 0 || x >= cellSize.x || y < 0 || y >= cellSize.y)
                return 1;

            int mask = 0x80 >> x;
            return fontData[(stride * y) + (c - offset)] & mask;
        }

        int GetHeight() const
        {
            return cellSize.y;
        }

    private:
        const u8 *fontData;
        int stride;
        Vector<s16> cellSize;
        char offset;
        static Font* defaultFont;
    };
}
