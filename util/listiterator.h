#pragma once

namespace hfh3
{
    /** Utility class to iterate through doubly linked lists.
      * Should be referenced through the type aliases in List<T>, such
      * as List<T>::Iterator or List<T>::ReverseIterator
      */
    template<typename LT, bool reverse>
    class _ListIterator
    {
    public:
        using List = LT;
        using Item = typename LT::Item;
        using Payload = typename LT::Payload;

        _ListIterator(Item* inCurrent=nullptr)
            : current(inCurrent)
        {}

        _ListIterator(const _ListIterator& other)
            : current(other.current)
        {}

        explicit operator _ListIterator<List,!reverse> ()
        {
            return _ListIterator<List,!reverse>(current);
        }

        void Remove()
        {
            assert(current != nullptr);
            current->parent->Remove(current);
            current = nullptr;
        }

        template<typename... Args>
        _ListIterator<List,reverse> InsertAfter(Args&&... args)
        {
            assert(current != nullptr);
            return static_cast<_ListIterator<List,reverse>> (
                   reverse?current->parent->InsertBefore(current, args...)
                          :current->parent->InsertAfter(current, args...));
        }

        template<typename... Args>
        _ListIterator<List,reverse> InsertBefore(Args&&... args)
        {
            assert(current != nullptr);
            return static_cast<_ListIterator<List,reverse>> (
                   reverse?current->parent->InsertAfter(current, args...)
                          :current->parent->InsertBefore(current, args...));
        }

        _ListIterator<List,reverse>& operator++()
        {
            Forward();
            return *this;
        }

        _ListIterator<List,reverse> operator++(int)
        {
            auto tmp = *this;
            Forward();
            return tmp;
        }

        _ListIterator<List,reverse>& operator--()
        {
            Backward();
            return *this;
        }

        _ListIterator<List,reverse> operator--(int)
        {
            auto tmp = *this;
            Backward();
            return tmp;
        }

        operator bool() const
        {
            return current != nullptr;
        }

        bool operator==(const _ListIterator<List,reverse>& other) const
        {
            return current == other.current;
        }

        bool operator!=(const _ListIterator<List,reverse>& other) const
        {
            return current != other.current;
        }

        bool operator==(const Payload* other) const
        {
            return current ? &current->payload == other : other == nullptr;
        }

        bool operator!=(const Payload* other) const
        {
            return !(current == other);
        }

        Payload& operator*()
        {
            assert(current != nullptr);
            return current->payload;
        }

        const Payload& operator*() const
        {
            assert(current != nullptr);
            return current->payload;
        }

        Payload* operator->()
        {
            return current != nullptr?&current->payload:nullptr;
        }

        const Payload* operator->() const
        {
            return current != nullptr?&current->payload:nullptr;
        }

        operator Payload*()
        {
            return current != nullptr?&current->payload:nullptr;
        }

        operator const Payload*() const
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
