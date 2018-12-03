#include "util/serialization.h"
#include "util/log.h"

using namespace hfh3;

void ArrayReader::Serialize(u8* data, int length)
{
    assert(GetOffset()+length <= array.Size());
    memcpy(data, (u8*)iterator, length);
    iterator += length;
#ifdef DEBUG_SERIALIZATION
    DEBUG("Read *%08x {%02x, %02x, %02x, %02x} len: %d",
        data, data[0], data[1], data[2], data[3], length
    );
#endif
}

void ArrayWriter::Serialize(u8* data, int length)
{
#ifdef DEBUG_SERIALIZATION
    DEBUG("Write *%08x {%02x, %02x, %02x, %02x} len: %d",
        data, data[0], data[1], data[2], data[3], length
    );
#endif
    array.AppendRaw(data, length);
}