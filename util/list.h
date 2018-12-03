#pragma once
#include "util/listitem.h"
#include "util/listiterator.h"
#include "util/log.h"
#include <assert.h>

/* std::initializer_list is available in freestanding mode.
  */
#include <initializer_list>

namespace hfh3
{
    /** Templated container class that implements a simple double link list.
      */
    template<typename T>
    class List
    {
    public:
        using Payload = T;
        using Item = _ListItem<List<T>>;
        using Iterator = _ListIterator<List<T>, false>;
        using ReverseIterator = _ListIterator<List<T>, true>;

        /** The number of items in the list.
          * Since the count is stored and updated on insertion and removal,
          * this is a constant time operation.
          */
        unsigned Size() const
        {
            return count;
        }

        /** Removes all elements from the list.
          */
        void Clear()
        {
            for(Item* item = first; item != nullptr;)
            {
                // This is a destructive loop, so we have to fetch the next item
                // before we destroy the current.
                Item* current = item;
                item = item->next;

                // Destroy and deallocate the item
                delete current;
            }

            // Mark the list as empty
            count = 0;
            first = last = nullptr;
        }

        /** Remove all elements from the list.
          * If Item pools are enabled, this will deallocate the list members
          * without calling any destructors. Otherwise it is the same as calling
          * Clear.
          */
        void ClearFast()
        {
#           if CONFIG_USE_ITEM_POOL
                Item::DeallocateRange_NoDestruct(first, last);
                count = 0;
                first = last = nullptr;
#           else
                Clear();
#           endif
        }

        /** If item pools are enabled, reserves memory for count
          * items and adds them to the pool.
          */
        static void Reserve(unsigned count)
        {
#           if CONFIG_USE_ITEM_POOL
            Item::PreallocItemPool(count);
#           endif
        }

        /** Returns true if the list is empty
          */
        bool IsEmpty() const
        {
            assert(count ? (first && last) : (!first && !last));
            return !count;
        }

        /** Returns true if the item is contained in the current list.
          * This is a O(n) operation, since it needs to scan through
          * the entire list, comparing each element to the item.
          * (or until found)
          */
        bool Contains(const T& item) const
        {
            return Contains([&item](const T& other){ return item == other; });
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
        template<typename... Args>
        Iterator Append(Args&&... args)
        {
            return InsertAfter(last, args...);
        }

        /** Prepends an item to the start of the list.
          * If the item is already a member of a list, it will be removed first.
          */
        template<typename... Args>
        Iterator Prepend(Args&&... args)
        {
            return InsertBefore(first, args...);
        }


        /** Find an element in the list, invoking predicate on each element
          * until found. Returns this->end() if the item was not found
          */
        template<typename F>
        Iterator FindFirst(F predicate)
        {
            Iterator result = begin();
            for(; result != end(); ++result)
            {
                if(predicate(*result))
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
        ReverseIterator FindLast(F predicate)
        {
            ReverseIterator result = rbegin();
            for(; result != rend(); ++result)
            {
                if(predicate(*result))
                {
                    break;
                }
            }
            return result;
        }

        /** Creates an iterator for iterating through the list from
          * the beginning to the end.
          */
        Iterator begin()
        {
            return Iterator(first);
        }

        /** Returns an empty iterator indicating a position one past
          * the end of the list.
          */
        Iterator end()
        {
            return Iterator(nullptr);
        }

        /** Creates an iterator for iterating through the list from
          * the end to the beginning.
          */
        ReverseIterator rbegin()
        {
            return ReverseIterator(last);
        }

        /** Returns an empty iterator indicating a position one past
          * the beginning of the list.
          */
        ReverseIterator rend()
        {
            return ReverseIterator(nullptr);
        }

        /** Wrapper around List<T> that swaps begin/end() with rbegin/rend()
          * Useful for iterating backwards through a list using the C++11
          * for each syntax:
          *  for(auto item : list.Reverse()) { ... } 
          */
        class ReverseAdapter
        {
        public:
            ReverseIterator begin() 
            {
                return list.rbegin();
            }

            ReverseIterator end() 
            {
                return list.rend();
            }

            Iterator rbegin() 
            {
                return list.begin();
            }

            Iterator rend() 
            {
                return list.end();
            }

        private:
            ReverseAdapter(List<T>& inList) : list(inList)
            {}
            List<T>& list;
            friend List<T>;
        };

        /** Returns a wrapper around List<T> that swaps begin/end() with rbegin/rend()
          * Useful for iterating backwards through a list using the C++11
          * for each syntax:
          *  for(auto item : list.Reverse()) { ... } 
          */
        ReverseAdapter Reverse()
        {
            return ReverseAdapter(*this);
        }

        List()
            : first(nullptr)
            , last(nullptr)
            , count(0)
        {}

        List(std::initializer_list<T> init)
            : first(nullptr)
            , last(nullptr)
            , count(0)
        {
#           if CONFIG_USE_ITEM_POOL
            Item::PreallocItemPool(init.size());
#           endif
            for(auto& value : init)
            {
                Append(value);
            }
        }

        ~List()
        {
            Clear();
        }

    private:
        /** Removes an item from the list.
          * The item must already be a part of the list.
          * Returns the value stored at the location.
          */
        void Remove(Item* item)
        {
            // The item passed in must be already a part of this list
            assert(item->parent == this);

            // Get the internal pointers from the item object
            Item* next = item->next;
            Item* previous = item->previous;

            // Fix up the pointers in the surrounding objects
            if(next)
            {
                assert(next->previous == item);
                assert(next->parent == this);
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
                assert(previous->parent == this);
                previous->next = next;
            }
            else
            {
                // if previous is null, it means that this is the first object in the chain
                // and we need to fix up our first pointer
                assert(first == item);
                first = next;
            }

            // Deallocate the item
            delete item;
            // Update the item count
            count--;
        }

        /** Inserts item after the item at position.
          * The position item must already be a member of the list. If position is
          * null, the list must be empty.
          */
        template<typename... Args>
        Iterator InsertAfter(Item* position, Args&&... args)
        {
            Item* item = new Item(this, args...);

            if(position == nullptr)
            {
                InsertFirstItem(item);
            }
            else
            {
                // Patch in the item between position and its next object
                Item* oldNext = position->next;
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
            }

            PostInsert(item);
            return Iterator(item);
        }

        /** Inserts item before the item at position.
          * The position item must already be a member of the list. If position is
          * null, the list must be empty.
          * If the item is already a member of a list, it will be removed first.
          * This can be used to move items within a list.
          */
        template<typename... Args>
        Iterator InsertBefore(Item* position, Args&&... args)
        {
            Item* item = new Item(this, args...);

            if(position == nullptr)
            {
                InsertFirstItem(item);
            }
            else
            {
                // Patch in the item between position and its previous object
                Item* oldPrevious = position->previous;
                position->previous = item;
                item->next = position;
                item->previous = oldPrevious;
                if(oldPrevious)
                {
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
            return Iterator(item);
        }

        // If the list is empty, this will make item the only element
        void InsertFirstItem(Item* item)
        {
            assert(IsEmpty());
            first = last = item;
        }

        // Ensure the item is marked as belonging to the current list
        // and update the count of elements
        void PostInsert(Item* item)
        {
            count++;
        }

        Item* first;
        Item* last;
        unsigned count;

        friend Iterator;
        friend ReverseIterator;
    };
}
