#pragma once

namespace hfh3
{
    /** Utility class to iterate through arrays.
      * Should be referenced through the typedefs in Array<T>, such
      * as Array<T>::Iterator or Array<T>::ReverseIterator
      */
    template<typename T, bool reverse>
    class _ArrayIterator
    {
    public:
        _ArrayIterator(T* inPtr)
            : ptr(inPtr)
        {}

        _ArrayIterator(const _ArrayIterator& other)
            : ptr(other.ptr)
        {}

        explicit operator _ArrayIterator<T,!reverse> ()
        {
            return _ArrayIterator<T,!reverse>(ptr);
        }

        _ArrayIterator<T,reverse>& operator++()
        {
            operator+=(1);
            return *this;
        }

        _ArrayIterator<T,reverse> operator++(int)
        {
            auto tmp = *this;
            operator+=(1);
            return tmp;
        }

        _ArrayIterator<T,reverse>& operator--()
        {
            operator-=(1);
            return *this;
        }

        _ArrayIterator<T,reverse> operator--(int)
        {
            auto tmp = *this;
            operator-=(1);
            return tmp;
        }

        _ArrayIterator<T,reverse>& operator+=(int steps)
        {
            if(reverse)
            {
                ptr -= steps;
            }
            else
            {
                ptr += steps;
            }
            return *this;
        }

        _ArrayIterator<T,reverse>& operator-=(int steps)
        {
            if(reverse)
            {
                ptr += steps;
            }
            else
            {
                ptr -= steps;
            }
            return *this;
        }

        operator bool() const
        {
            return ptr;
        }

        bool operator==(const _ArrayIterator<T,reverse>& other) const
        {
            return ptr == other.ptr;
        }

        bool operator!=(const _ArrayIterator<T,reverse>& other) const
        {
            return ptr != other.ptr;
        }

        bool operator==(const T* other) const
        {
            return ptr == other;

        }

        bool operator!=(const T* other) const
        {
            return ptr != other;
        }

        T& operator*()
        {
            return *ptr;
        }

        const T& operator*() const
        {
            return *ptr;
        }

        T* operator->()
        {
            return ptr;
        }

        const T* operator->() const
        {
            return ptr;
        }

        operator T*()
        {
            return ptr;
        }

        operator const T*() const
        {
            return ptr;
        }

    private:

        void Forward()
        {
            ptr += reverse?-1:1;
        }

        void Backward()
        {
            ptr -= reverse?-1:1;
        }

        T* ptr;
    };
}
