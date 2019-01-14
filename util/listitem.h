#pragma once
#include <circle/types.h>
#include "config.h"

namespace hfh3
{
    /** Internal utility class used by List<T>. Contains the payload
      * and the internal links between items.
      */
    template <typename LT>
    struct _ListItem
    {
        using List = LT;
        using Payload = typename LT::Payload;
        using Item = _ListItem<LT>;

        template <typename... Args>
        _ListItem(List* inParent, Args&&... args)
            : parent(inParent)
            , previous(nullptr)
            , next(nullptr)
            , payload(args...)
        {}

        List*   parent;
        Item*   previous;
        Item*   next;
        Payload payload;

#if CONFIG_USE_ITEM_POOL
        static void* operator new (size_t size);
        static void operator delete (void* memory, size_t size);

        // An optimization for deallocating a whole range of items.
        // Note: it will not iterate through the items and call any destructors
        static void DeallocateRange_NoDestruct(Item* first, Item* last);

        // Allocates memory for count items and adds them to the item pool
        static void PreallocItemPool(unsigned count);
    private:
        /* reuse previously allocated items to save on allocations */
        static Item* itemPool;
#endif
    };

#if CONFIG_USE_ITEM_POOL
    template <typename LT>
    _ListItem<LT>* _ListItem<LT>::itemPool = nullptr;

    template <typename LT>
    void* _ListItem<LT>::operator new (size_t size)
    {
        if(itemPool && size == sizeof(_ListItem<LT>))
        {
            _ListItem<LT>* result = itemPool;
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

    template <typename LT>
    void _ListItem<LT>::operator delete (void* memory, size_t size)
    {
        if(memory == nullptr)
        {
            return;
        }

        if(size == sizeof(_ListItem<LT>))
        {
            // instead of deallocating the item, push it back to the item pool
            _ListItem<LT>* item = reinterpret_cast<_ListItem<LT>*>(memory);
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

    template <typename LT>
    void _ListItem<LT>::DeallocateRange_NoDestruct(_ListItem<LT>* first, _ListItem<LT>* last)
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

    template <typename LT>
    void _ListItem<LT>::PreallocItemPool(unsigned count)
    {
        if(count < 1)
        {
            return;
        }

        // Allocate memory for count items in one block
        _ListItem<LT>* newItems = static_cast<_ListItem<LT>*>(::operator new(count * sizeof(_ListItem<LT>)));

        // Make sure the last item points to the current head of the item pool
        newItems[count-1].next = itemPool;

        // Link the remaining items to each other
        for(unsigned i = 1; i < count; i++)
        {
            newItems[i-1].next = &newItems[i];
        }

        // Update the head of the item pool
        itemPool = newItems;
    }

#endif

}
