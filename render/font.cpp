#include "render/font.h"
#include "graphics/fontdata.h"
using namespace hfh3;

Font* Font::defaultFont = nullptr;

Font::Font(const u8* inData, int inStride, int inHeight, char inOffset)
    : fontData(inData)
    , stride(inStride)
    , cellSize(8, inHeight)
    , offset(inOffset)
{
}

Font& Font::GetDefault()
{
    if(!defaultFont)
    {
        defaultFont = new Font(font_data, font_width, font_height, font_offset);
    }
    return *defaultFont;
}
