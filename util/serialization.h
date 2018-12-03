/** Utility classes for serializing and deserializing to and from a byte array.
     */
#pragma once
#include "util/array.h"
#include "util/endian.h"
#include "util/vector.h"
#include "util/rect.h"
#include <type_traits>

#define SERIALIZATION_BYTE_ORDER LITTLE_ENDIAN
#if SERIALIZATION_BYTE_ORDER == LITTLE_ENDIAN
#define SerialEndian(x) LittleEndian(x)
#else 
#define SerialEndian(x) BigEndian(x)
#endif

namespace hfh3
{
    class ISerializer 
    {
        public:
        enum Direction 
        {
            Input,
            Output,
        };
        virtual void Serialize(u8* data, int length) = 0; 
        virtual Direction GetDirection() const = 0;
    };

    class ISerializable
    {
        public:
        virtual void Serialize(ISerializer& serializer) = 0;
    };


    class ArrayReader : public ISerializer
    {
        public:
        ArrayReader(Array<u8>& inArray, int offset=0)
            : array(inArray), iterator(array.begin())
        {
            assert (offset >= 0 && offset < array.Size() );
            iterator += offset;
        }

        ArrayReader(Array<u8>& inArray, Array<u8>::Iterator inIter)
            : array(inArray), iterator(inIter)
        {
            int offset = GetOffset();
            assert (offset >= 0 && offset < array.Size() );
        }

        virtual void Serialize(u8* data, int length) override;
        virtual Direction GetDirection() const override
        {
            return Input;
        }

        Array<u8>::Iterator GetIterator() const 
        {
            return iterator;
        }

        int GetOffset() const
        {
            return (const u8*)iterator - (const u8*)array;
        }

        int GetRemaining() const
        {
            return array.Size() - GetOffset();
        }

        private:
            Array<u8>& array;
            Array<u8>::Iterator iterator;
    };

    class ArrayWriter : public ISerializer
    {
        public:
        ArrayWriter(Array<u8>& inArray)
            : array(inArray)
        {}

        virtual void Serialize(u8* data, int length) override;
        virtual Direction GetDirection() const override
        {
            return Output;
        }

        private:
            Array<u8>& array;
    };

    inline void Serialize(ISerializer& serializer, ISerializable& serializable)
    {
        serializable.Serialize(serializer);
    }

    /** Templated serialization for native scalar values */
    template<typename T>
    std::enable_if_t<std::is_arithmetic<T>::value || std::is_enum<T>::value>
    Serialize(ISerializer& serializer, T& value)
    {
#if BYTE_ORDER == SERIALIZATION_BYTE_ORDER
        serializer.Serialize(reinterpret_cast<u8*>(&value), sizeof(T));
#else
        if(serializer.GetDirection() == ISerializer::Output)
        {
            T tmp = SerialEndian(value);
            serializer.Serialize(reinterpret_cast<u8*>(&tmp), sizeof(T));
        }
        else
        {
            serializer.Serialize(reinterpret_cast<u8*>(&value), sizeof(T));
            value = SerialEndian(value);
        }
#endif
    }

    template<typename T>
    void Serialize(ISerializer& serializer, Vector<T>& value)
    {
        Serialize(serializer, value.x);
        Serialize(serializer, value.y);
    }

    template<typename T>
    void Serialize(ISerializer& serializer, Rect<T>& value)
    {
        Serialize(serializer, value.origin);
        Serialize(serializer, value.size);
    }

}

