#pragma once
#include "util/dlinkiterator.h"
#include "util/log.h"
#include <assert.h>

namespace hfh3
{
    /** Utility class that implements a simple double link list.
      * The type T must have the members T* previous and T* next for instance
      * by extending DLinkItem<T, LT>.
      *
      * When added, the list takes ownership of the item. Any items on the list
      * during destruction will be destroyed. If an item must survive past the
      * lifetime of the list, it must be removed first.
      */
    template<typename T, typename LT>
    class DLinkList
    {
    public:

        /** The number of items in the list.
          * Since the count is stored and updated on insertion and removal,
          * this is a constant time operation.
          */
        bool Size() const
        {
            return count;
        }

        /** Removes all elements from the list.
          * If destroy is true (the default), each element on the list will
          * be destroyed.
          */
        void Clear(bool destroy = true)
        {
            if(destroy)
            {
                for(auto iter = begin(); iter != end();)
                {
                    // This is a destructive loop, so we have to fetch the next item
                    // before we destroy the current.
                    auto current = iter++;

                    // Break the connection with the current list.
                    current->parent = nullptr;
                    current->previous = current->next = nullptr;

                    // Destroy and deallocate the item
                    delete &*current;
                }
            }

            // Mark the list as empty
            count = 0;
            first = last = nullptr;
        }

        /** Returns true if the list is empty
          */
        bool IsEmpty() const
        {
            assert(count ? (first && last) : (!first && !last));
            return !count;
        }

        /** Returns true if the item is contained in the current list
          * Since the items contain a pointer to their parent,
          * this is a constant time operation
          */
        bool Contains(const T* item) const
        {
            return item->parent == this;
        }

        /** Searches through the list for items matchin a predicate.
          * This is a O(n) operation, since it needs to scan through
          * the entire list, evaluating the predicate for each one
          * (or until found)
          */
        template <typename F>
        bool Contains(F predicate)
        {
            return FindFirst(predicate) != end();
        }

        /** Appends an item to the end of the list.
          * If the item is already a member of a list, it will be removed first.
          */
        void Append(T* item)
        {
            InsertAfter(last, item);
        }

        /** Prepends an item to the start of the list.
          * If the item is already a member of a list, it will be removed first.
          */
        void Prepend(T* item)
        {
            InsertBefore(first, item);
        }

        /** Removes an item from the list.
          * The item must already be a part of the list.
          */
        void Remove(T* item)
        {
            assert(item);

            // The item passed in must be already a part of this list
            assert(item->parent == this);

            // Get the internal pointers from the item object
            T* next = item->next;
            T* previous = item->previous;

            // Fix up the pointers in the surrounding objects
            if(next)
            {
                assert(next->previous == item);
                next->previous = previous;
            }
            else
            {
                // if next is null, it means that this is the last object in the chain
                // and we need to fix up our last pointer
                assert(last == item);
                last = previous;
            }

            if(previous)
            {
                assert(previous->next == item);
                previous->next = next;
            }
            else
            {
                // if previous is null, it means that this is the first object in the chain
                // and we need to fix up our first pointer
                assert(first == item);
                first = item;
            }

            // Update the item count
            count--;

            // Clear the internal pointers in the object
            item->parent = nullptr;
            item->previous = item->next = nullptr;
        }

        /** Inserts item after the item at position.
          * The position item must already be a member of the list. If position is
          * null, the list must be empty.
          * If the item is already a member of a list, it will be removed first.
          * This can be used to move items within a list.
          */
        void InsertAfter(T* position, T* item)
        {
            assert(position != item);

            PreInsert(item);

            if(position == nullptr)
            {
                InsertFirstItem(item);
            }
            else
            {
                // Patch in the item between position and its next object
                T* oldNext = position->next;
                position->next = item;
                item->previous = position;
                item->next = oldNext;
                if(oldNext)
                {
                    assert(oldNext->previous == position);
                    oldNext->previous = item;
                }
                else
                {
                    // if the old next pointer was null, the "position"
                    // must have been the last object, update our last pointer
                    assert(last == position);
                    last = item;
                }

                item->parent = static_cast<LT*>(this);
            }

            PostInsert(item);
        }

        /** Inserts item before the item at position.
          * The position item must already be a member of the list. If position is
          * null, the list must be empty.
          * If the item is already a member of a list, it will be removed first.
          * This can be used to move items within a list.
          */
        void InsertBefore(T* position, T* item)
        {
            assert(position != item);

            PreInsert(item);

            if(position == nullptr)
            {
                InsertFirstItem(item);
            }
            else
            {
                // Patch in the item between position and its previous object
                T* oldPrevious = position->previous;
                position->previous = item;
                item->next = position;
                item->previous = oldPrevious;
                if(oldPrevious)
                {
                    DEBUG("oldPrevious: %p, oldPrevious->next: %p pos: %p, item: %p", oldPrevious, oldPrevious->next, position, item);
                    assert(oldPrevious->next == position);
                    oldPrevious->next = item;
                }
                else
                {
                    // if the old previous pointer was null, the "position"
                    // must have been the first object, update our first pointer
                    assert(first == position);
                    first = item;
                }
            }

            PostInsert(item);
        }

        /** Find an element in the list, invoking predicate on each element
          * until found. Returns this->end() if the item was not found
          */
        template<typename F>
        DLinkIterator<T> FindFirst(F predicate)
        {
            DLinkIterator<T> result = begin();
            for(; result != end(); ++result)
            {
                if(predicate(&*result))
                {
                    break;
                }
            }
            return result;
        }

        /** Find an element in the list, invoking predicate on each element
          * starting from the last element until found.
          * Returns this->rend() if the item was not found.
          */
        template<typename F>
        DLinkIterator<T> FindLast(F predicate)
        {
            DLinkIterator<T> result = rbegin();
            for(; result != rend(); ++result)
            {
                if(predicate(&*result))
                {
                    break;
                }
            }
            return result;
        }

        /** Creates an iterator for iterating through the list from
          * the beginning to the end.
          */
        DLinkIterator<T> begin()
        {
            return DLinkIterator<T>(first);
        }

        /** Returns an empty iterator indicating a position one past
          * the end of the list.
          */
        DLinkIterator<T> end()
        {
            return DLinkIterator<T>(nullptr);
        }

        /** Creates an iterator for iterating through the list from
          * the end to the beginning.
          */
        DLinkIterator<T> rbegin()
        {
            return DLinkIterator<T>(last, true);
        }

        /** Returns an empty iterator indicating a position one past
          * the beginning of the list.
          */
        DLinkIterator<T> rend()
        {
            return DLinkIterator<T>(nullptr, true);
        }

    protected:

        DLinkList()
            : first(nullptr)
            , last(nullptr)
            , count(0)
        {}

        ~DLinkList()
        {
            Clear();
        }

    private:

        // If the list is empty, this will make item the only element
        void InsertFirstItem(T* item)
        {
            assert(IsEmpty());

            first = last = item;
            item->parent = static_cast<LT*>(this);
        }

        // Remove the item from a parent list if it's already in one
        void PreInsert(T* item)
        {
            assert(item);
            if(item->parent)
            {
                item->parent->Remove(item);
            }
        }

        // Ensure the item is marked as belonging to the current list
        // and update the count of elements
        void PostInsert(T* item)
        {
            item->parent = static_cast<LT*>(this);
            ++count;
        }

        T* first;
        T* last;
        unsigned count;
    };
}
