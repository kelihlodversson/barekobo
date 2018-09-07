#pragma once
#include "util/dlinklist.h"

namespace hfh3
{
    /** Utility base class to implement item classes
      * that can be contained in a linked list.
      * The template parameters specify the elemtent type
      * and the container class. The second parameter you must use a class
      * that is derived from DLinkList<T, LT>.
      */
    template<typename T, typename LT>
    class DLinkItem
    {

    public:

        void InsertAfter(T* other)
        {
            assert(parent);
            parent->InsertAfter(DerivedThis(), other);
        }

        void InsertBefore(T* other)
        {
            assert(parent);
            parent->InsertBefore(DerivedThis(), other);
        }

    protected:
        /* Even though this class is non-virtual, it is meant as a mix-in
         * class to be derived by other classes. Therefore the constructor
         * is defined as protected.
         */
        DLinkItem()
            : parent(nullptr)
            , previous(nullptr)
            , next(nullptr)
        {}

        /* Deleting DLinkItem derived objects will automatically remove them
         * from their lists. Note: since this destructor is non-virtual,
         * one should never delete derived virtual instances through pointers
         * of this class. Therefore the destructor is declared as protected.
         */
        ~DLinkItem()
        {
            if(parent != nullptr)
            {
                parent->Remove(DerivedThis());
            }
        }

        LT* parent;
        T*  previous;
        T*  next;
    private:

        T* DerivedThis()
        {
            return static_cast<T*>(this);
        }

        // Grant the list and iterator classes access to double linked list elements.
        friend class DLinkList<T, LT>;
        friend class DLinkIterator<T>;
    };
}
