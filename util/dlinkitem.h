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
    template <typename T, typename P>
    struct DLinkItem
    {
        template <typename... Args>
        DLinkItem(P* inParent, Args&&... args)
            : parent(inParent)
            , previous(nullptr)
            , next(nullptr)
            , payload(args...)
        {}

        P*              parent;
        DLinkItem<T,P>* previous;
        DLinkItem<T,P>* next;
        T               payload;

#if CONFIG_USE_ITEM_POOL
        static void* operator new (size_t size);
        static void operator delete (void* memory, size_t size);

        // An optimization for deallocating a whole range of items.
        // Note: it will not iterate through the items and call any destructors
        static void DeallocateRange_NoDestruct(DLinkItem<T,P>* first, DLinkItem<T,P>* last);
    private:
        /* reuse previously allocated items to save on allocations */
        static DLinkItem<T,P>* itemPool;
#endif
    };

#if CONFIG_USE_ITEM_POOL
    template <typename T, typename P>
    DLinkItem<T,P>* DLinkItem<T,P>::itemPool = nullptr;

    template <typename T, typename P>
    void* DLinkItem<T,P>::operator new (size_t size)
    {
        if(itemPool && size == sizeof(DLinkItem<T,P>))
        {
            DLinkItem<T,P>* result = itemPool;
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
    void DLinkItem<T,P>::operator delete (void* memory, size_t size)
    {
        if(memory == nullptr)
        {
            return;
        }

        if(size == sizeof(DLinkItem<T,P>))
        {
            // instead of deallocating the item, push it back to the item pool
            DLinkItem<T,P>* item = reinterpret_cast<DLinkItem<T,P>*>(memory);
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
    void DLinkItem<T,P>::DeallocateRange_NoDestruct(DLinkItem<T,P>* first, DLinkItem<T,P>* last)
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
