#pragma once

// Set CONFIG_USE_ITER_POOL to 0 use the standard memory allocator for link list
// iterators. When set to 1, freed items will be added to a pool of items to be reused
// instead of deallocating the memory.
#ifndef CONFIG_USE_ITER_POOL
#define CONFIG_USE_ITER_POOL 1
#endif

namespace hfh3
{
    /** Utility class to iterate through doubly linked lists
      */
    template<typename T, typename Item, typename List>
    class DLinkIterator
    {
    public:
        DLinkIterator(Item* inCurrent=nullptr, bool inReverse=false)
            : current(inCurrent)
            , reverse(inReverse)
        {}

        DLinkIterator(const DLinkIterator& other)
            : current(other.current)
            , reverse(other.reverse)
        {}

        void Remove()
        {
            assert(current != nullptr);
            current->parent->Remove(current);
            current = nullptr;
        }

        template<typename... Args>
        DLinkIterator<T,Item,List> InsertAfter(Args&&... args)
        {
            assert(current != nullptr);
            return current->parent->InsertAfter(current, args...);
        }

        template<typename... Args>
        DLinkIterator<T,Item,List> InsertBefore(Args&&... args)
        {
            assert(current != nullptr);
            return current->parent->InsertBefore(current, args...);
        }

        DLinkIterator<T,Item,List>& operator++()
        {
            Forward();
            return *this;
        }

        DLinkIterator<T,Item,List> operator++(int)
        {
            auto tmp = *this;
            Forward();
            return tmp;
        }

        DLinkIterator<T,Item,List>& operator--()
        {
            Backward();
            return *this;
        }

        DLinkIterator<T,Item,List> operator--(int)
        {
            auto tmp = *this;
            Backward();
            return tmp;
        }

        operator bool() const
        {
            return current != nullptr;
        }

        bool operator==(const DLinkIterator<T,Item,List>& other) const
        {
            return current == other.current;
        }

        bool operator!=(const DLinkIterator<T,Item,List>& other) const
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

#if CONFIG_USE_ITER_POOL
        static void* operator new (size_t size);
        static void operator delete (void* memory, size_t size);
#endif
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
        bool reverse;
        friend List;
#if CONFIG_USE_ITER_POOL
        /* reuse previously allocated iterators to save on allocations */
        static DLinkIterator<T,Item,List>* iterPool;
#endif
    };

#if CONFIG_USE_ITER_POOL
    template<typename T, typename Item, typename List>
    DLinkIterator<T,Item,List>* DLinkIterator<T,Item,List>::iterPool = nullptr;

    template<typename T, typename Item, typename List>
    void* DLinkIterator<T,Item,List>::operator new (size_t size)
    {
        if(iterPool && size == sizeof(DLinkIterator<T,Item,List>))
        {
            DLinkIterator<T,Item,List>* result = iterPool;
            // Reuse the current pointer to link together unallocated items in the pool
            iterPool = reinterpret_cast<DLinkIterator<T,Item,List>*>(result->current);
            return result;
        }
        else
        {
            // Use the default new operator for unsupported sizes or when the item pool is empty
            return ::operator new(size);
        }
    }

    template<typename T, typename Item, typename List>
    void DLinkIterator<T,Item,List>::operator delete (void* memory, size_t size)
    {
        if(memory == nullptr)
        {
            return;
        }

        if(size == sizeof(DLinkIterator<T,Item,List>))
        {
            // instead of deallocating the item, push it back to the item pool
            DLinkIterator<T,Item,List>* iter = reinterpret_cast<DLinkIterator<T,Item,List>*>(memory);
            iter->current = reinterpret_cast<Item*>(iterPool);
            iterPool = iter;
        }
        else
        {
            // If the size doesn't match our object size, forward the object to the default deallocator
            // This can happen if the iterator class has been extended without overriding the custom allocators.
            ::operator delete(memory);
        }
    }
#endif
}
