#pragma once

namespace hfh3
{
    /** Utility class to iterate through doubly linked objects
      * The tyoe T must have the members T* previous and T* next.
      */
    template<typename T>
    class DLinkIterator
    {
    public:
        DLinkIterator(T* inCurrent, bool inReverse=false)
            : current(inCurrent)
            , reverse(inReverse)
        {}

        DLinkIterator(const DLinkIterator& other)
            : current(other.current)
            , reverse(other.reverse)
        {}


        DLinkIterator<T>& operator++()
        {
            Forward();
            return *this;
        }

        DLinkIterator<T> operator++(int)
        {
            auto tmp = *this;
            Forward();
            return tmp;
        }

        DLinkIterator<T>& operator--()
        {
            Backward();
            return *this;
        }

        DLinkIterator<T> operator--(int)
        {
            auto tmp = *this;
            Backward();
            return tmp;
        }

        operator bool() const
        {
            return current != nullptr;
        }

        bool operator==(const DLinkIterator<T>& other) const
        {
            return current == other.current;
        }

        bool operator!=(const DLinkIterator<T>& other) const
        {
            return current != other.current;
        }

        bool operator==(const T* other) const
        {
            return current == other;
        }

        bool operator!=(const T* other) const
        {
            return current != other;
        }

        T& operator*()
        {
            return *current;
        }

        const T& operator*() const
        {
            return *current;
        }

        T* operator->()
        {
            return current;
        }

        const T* operator->() const
        {
            return current;
        }

        operator T*()
        {
            return current;
        }

        operator const T*() const
        {
            return current;
        }

    private:

        void Forward()
        {
            current = reverse?current->previous:current->next;
        }

        void Backward()
        {
            current = reverse?current->next:current->previous;
        }

        T* current;
        bool reverse;
    };
}
