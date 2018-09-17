#pragma once

namespace hfh3
{
    /** Utility class to iterate through doubly linked lists.
      * Should be referenced through the typedefs in List<T>, such
      * as List<T>::Iterator or List<T>::ReverseIterator
      */
    template<typename T, typename Item, bool reverse>
    class _ListIterator
    {
    public:
        _ListIterator(Item* inCurrent=nullptr)
            : current(inCurrent)
        {}

        _ListIterator(const _ListIterator& other)
            : current(other.current)
        {}

        explicit operator _ListIterator<T,Item,!reverse> ()
        {
            return _ListIterator<T,Item,!reverse>(current);
        }

        void Remove()
        {
            assert(current != nullptr);
            current->parent->Remove(current);
            current = nullptr;
        }

        template<typename... Args>
        _ListIterator<T,Item,reverse> InsertAfter(Args&&... args)
        {
            assert(current != nullptr);
            return static_cast<_ListIterator<T,Item,reverse>> (
                   reverse?current->parent->InsertBefore(current, args...)
                          :current->parent->InsertAfter(current, args...));
        }

        template<typename... Args>
        _ListIterator<T,Item,reverse> InsertBefore(Args&&... args)
        {
            assert(current != nullptr);
            return static_cast<_ListIterator<T,Item,reverse>> (
                   reverse?current->parent->InsertAfter(current, args...)
                          :current->parent->InsertBefore(current, args...));
        }

        _ListIterator<T,Item,reverse>& operator++()
        {
            Forward();
            return *this;
        }

        _ListIterator<T,Item,reverse> operator++(int)
        {
            auto tmp = *this;
            Forward();
            return tmp;
        }

        _ListIterator<T,Item,reverse>& operator--()
        {
            Backward();
            return *this;
        }

        _ListIterator<T,Item,reverse> operator--(int)
        {
            auto tmp = *this;
            Backward();
            return tmp;
        }

        operator bool() const
        {
            return current != nullptr;
        }

        bool operator==(const _ListIterator<T,Item,reverse>& other) const
        {
            return current == other.current;
        }

        bool operator!=(const _ListIterator<T,Item,reverse>& other) const
        {
            return current != other.current;
        }

        bool operator==(const T* other) const
        {
            return current ? &current->payload == other : other == nullptr;
        }

        bool operator!=(const T* other) const
        {
            return !(current == other);
        }

        T& operator*()
        {
            assert(current != nullptr);
            return current->payload;
        }

        const T& operator*() const
        {
            assert(current != nullptr);
            return current->payload;
        }

        T* operator->()
        {
            return current != nullptr?&current->payload:nullptr;
        }

        const T* operator->() const
        {
            return current != nullptr?&current->payload:nullptr;
        }

        operator T*()
        {
            return current != nullptr?&current->payload:nullptr;
        }

        operator const T*() const
        {
            return current != nullptr?&current->payload:nullptr;
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

        Item* current;
    };
}
