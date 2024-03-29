#pragma once
#include "util/arrayiterator.h"
#include "util/callback.h"
#include "util/log.h"
#include "util/new.h"
#include <circle/alloc.h>
#include <circle/util.h>
#include <assert.h>

/* std::initializer_list is available in freestanding mode.
  */
#include <initializer_list>

namespace hfh3
{
    /** Templated container class that implements a dynamic array.
      */
    template<typename T>
    class Array
    {
    public:
        static const int MIN_RESERVE = 8;
        using Iterator = _ArrayIterator<T, false>;
        using ConstIterator = _ArrayConstIterator<T, false>;
        using ReverseIterator = _ArrayIterator<T, true>;

        /** The number of items in the list.
          * This is a constant time operation.
          */
        int Size() const
        {
            return count;
        }

        /** Empties the array destroying any elements in it.
          */
        void Clear()
        {
            // Run the item destructor on each element
            for(int i = 0; i < count; ++i)
            {
                data[i].~T();
            }

            // Mark the list as empty
            count = 0;
        }

        /** Remove all elements from the list.
          * This will deallocate the array members without calling any destructors. 
          */
        void ClearFast()
        {
            // Mark the list as empty
            count = 0;
        }

        /** Ensure the underlying array can hold at least num items
          * If the array is already larger than num, it will use the array count instead.
          */
        void Reserve(int num)
        {
            if(num < count)
            {
                num = count;
            }
            if(reserved != num)
            {
                T* oldData = data;
                reserved = num;
                data = static_cast<T*>(malloc(sizeof(T)*num));

                if(oldData)
                {
                    memcpy(data, oldData, sizeof(T)*count);
                    free(oldData);
                }
            }
        }

        /** Returns true if the list is empty
          */
        bool IsEmpty() const
        {
            return !count;
        }

        /** Returns true if the item is contained in the current array.
          * This is a O(n) operation, since it needs to scan through
          * the entire liarrayst, comparing each element to the item.
          * (or until found)
          */
        bool Contains(const T& item) const
        {
            return Contains([&item](const T& other){ return item == other; });
        }

        /** Searches through the list for items matching a predicate.
          * This is a O(n) operation, since it needs to scan through
          * the entire array, evaluating the predicate for each one
          * (or until found)
          */
        template <typename F>
        bool Contains(F predicate)
        {
            return FindFirst(predicate) != end();
        }

        /** Appends an item to the end of the array.
          */
        template<typename... Args>
        Iterator Append(Args&&... args)
        {
            if(reserved <= count)
            {
                Reserve(reserved>=MIN_RESERVE ? reserved*2 : MIN_RESERVE);
            }

            // Initialize the element using placement new
            new (&data[count]) T(args...);
            return Iterator(&data[count++]);
        }

        /** Adds a default element past the end of the array without incrementing the size.
          * Useful for the String subclass to ensure a zero terminator when converting to a
          * char* 
          */
        void Terminate()
        {
            if(reserved <= count)
            {
                Reserve(reserved>=MIN_RESERVE ? reserved*2 : MIN_RESERVE);
            }

            // Call the default constructor without incrementing the count
            new (&data[count]) T();
        }

        Iterator AppendRaw(const T* src, int num)
        {
            int old_count = count;
            count += num;
            if(reserved < count)
            {
                Reserve(count>=MIN_RESERVE ? count : MIN_RESERVE);
            }
            T* dest = &data[old_count];
            memcpy(dest, src, num*sizeof(T));
            return Iterator(dest);
        }

        Iterator Append(const Array<T>& other)
        {
            return AppendRaw(other.data, other.count);
        }

        /** Same as Append, except it doesn't return an iterator. */
        template<typename... Args>
        void Push(Args&&... args)
        {
            Append(args...);
        }

        /** Returns the last element from the list and reduces the size by one. */
        T Pop()
        {
            assert(count > 0);
            count--;
            T retval = data[count];
            data[count].~T();
            return retval;
        }

        /** Swaps element i with the last element and then pulls it out of the list. */
        T Pull(int index)
        {
            assert(index >=0 && index < count);
            T retval = data[index];
            count--;
            if (count != index)
            {
                data[index] = data[count];
            }
            data[count].~T();

            return retval;
        }

        /** Removes the first num entries off the front of the array, shortening the
          * array and moving the remaining items up front.
          */
        void RemoveFront(int num)
        {
            assert(num > 0);
            if (num >= count)
            {
                ClearFast();
            }
            else
            {
                count -= num;
                int items_to_move = count;
                T* src = &data[num];
                T* dst = data;
                // Note, we don't have memmove, so we have to call memcpy on each non-overlapping chunk_size of data
                while(items_to_move)
                {
                    int chunk_size = items_to_move > num ? num : items_to_move;
                    memcpy(dst, src, sizeof(T)*chunk_size);
                    items_to_move -= chunk_size;
                    dst += chunk_size;
                    src += chunk_size;
                }
            }
        }

        T& operator[](int index)
        {
            assert(index >=0 && index < count);
            return data[index];
        }

        const T& operator[](int index) const
        {
            assert(index >=0 && index < count);
            return data[index];
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
            return Iterator(data);
        }

        ConstIterator begin() const
        {
            return ConstIterator(data);
        }

        /** Returns an empty iterator indicating a position one past
          * the end of the list.
          */
        Iterator end()
        {
            return Iterator(&data[count]);
        }

        ConstIterator end() const
        {
            return ConstIterator(&data[count]);
        }

        /** Creates an iterator for iterating through the list from
          * the end to the beginning.
          */
        ReverseIterator rbegin()
        {
            return ReverseIterator(&data[count-1]);
        }

        /** Returns an empty iterator indicating a position one past
          * the beginning of the list.
          */
        ReverseIterator rend()
        {
            return ReverseIterator((&data) - 1);
        }

        /** Wrapper around Array<T> that swaps begin/end() with rbegin/rend()
          * Useful for iterating backwards through a list using the C++11
          * for each syntax:
          *  for(auto item : array.Reverse()) { ... } 
          */
        class ReverseAdapter
        {
        public:
            ReverseIterator begin() 
            {
                return array.rbegin();
            }

            ReverseIterator end() 
            {
                return array.rend();
            }

            Iterator rbegin() 
            {
                return array.begin();
            }

            Iterator rend() 
            {
                return array.end();
            }

        private:
            ReverseAdapter(Array<T>& inArray) : array(inArray)
            {}
            Array<T>& array;
            friend Array<T>;
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

        Array()
            : data(nullptr)
            , reserved(0)
            , count(0)
        {}

        Array(const Array<T>& other)
            : data((T*)malloc(other.count * sizeof(T)))
            , reserved(other.count)
            , count(other.count)
        {
            memcpy(data, other.data, count * sizeof(T));
        }

        Array(int reserve)
            : data((T*)malloc(reserve * sizeof(T)))
            , reserved(reserve)
            , count(0)
        {}

        Array(std::initializer_list<T> init)
            : data((T*)malloc(init.size() * sizeof(T)))
            , reserved(init.size())
            , count(0)
        {
            AppendRaw(init.begin(), init.size());
        }
        
        ~Array()
        {
            Clear();
        }

        operator T* ()
        {
            return data;
        }
    private:

        T* data;
        int reserved;
        int count;
    };
}
