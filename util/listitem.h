#pragma once
#include <circle/types.h>

// Set CONFIG_USE_ITEM_POOL to 0 use the standard memory allocator for link list
// items. When set to 1, freed items will be added to a pool of items to be reused
// instead of deallocating the memory.
#ifndef CONFIG_USE_ITEM_POOL
#define CONFIG_USE_ITEM_POOL 1
#endif

namespace hfh3
{
    /** Internal utility class used by List<T>. Contains the payload
      * and the internal links between items.
      */
    template <typename T, typename P>
    struct _ListItem
    {
        template <typename... Args>
        _ListItem(P* inParent, Args&&... args)
            : parent(inParent)
            , previous(nullptr)
            , next(nullptr)
            , payload(args...)
        {}

        P*              parent;
        _ListItem<T,P>* previous;
        _ListItem<T,P>* next;
        T               payload;

#if CONFIG_USE_ITEM_POOL
        static void* operator new (size_t size);
        static void operator delete (void* memory, size_t size);

        // An optimization for deallocating a whole range of items.
        // Note: it will not iterate through the items and call any destructors
        static void DeallocateRange_NoDestruct(_ListItem<T,P>* first, _ListItem<T,P>* last);
    private:
        /* reuse previously allocated items to save on allocations */
        static _ListItem<T,P>* itemPool;
#endif
    };

#if CONFIG_USE_ITEM_POOL
    template <typename T, typename P>
    _ListItem<T,P>* _ListItem<T,P>::itemPool = nullptr;

    template <typename T, typename P>
    void* _ListItem<T,P>::operator new (size_t size)
    {
        if(itemPool && size == sizeof(_ListItem<T,P>))
        {
            _ListItem<T,P>* result = itemPool;
            // Reuse the next pointer to link together unallocated items in the pool
            itemPool = result->next;
            return result;
        }
        else
        {
            // Use the default new operator for unsupported sizes or when the item pool is empty
            return ::operator new(size);
        }
    }

    template <typename T, typename P>
    void _ListItem<T,P>::operator delete (void* memory, size_t size)
    {
        if(memory == nullptr)
        {
            return;
        }

        if(size == sizeof(_ListItem<T,P>))
        {
            // instead of deallocating the item, push it back to the item pool
            _ListItem<T,P>* item = reinterpret_cast<_ListItem<T,P>*>(memory);
            item->next = itemPool;
            itemPool = item;
        }
        else
        {
            // If the size doesn't match our object size, forward the object to the default deallocator
            // This can happen if the Item class has been extended without overriding the custom allocators.
            ::operator delete(memory);
        }
    }

    template <typename T, typename P>
    void _ListItem<T,P>::DeallocateRange_NoDestruct(_ListItem<T,P>* first, _ListItem<T,P>* last)
    {
        if (first != nullptr)
        {
            assert(last != nullptr);
            last->next = itemPool;
            itemPool = first;
        }
        else
        {
            assert(last == nullptr);
        }
    }

#endif

}
