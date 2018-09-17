#pragma once
#include "util/vector.h"

namespace hfh3
{
    // Simple data stucture for rectangles
    template<typename T>
    struct Rect
    {
        Rect() :
            origin(),
            size()
        {}

        Rect(T in_x, T in_y, T in_w, T in_h) :
            origin(in_x, in_y),
            size(in_w, in_h)
        {}

        Rect(const Vector<T>& in_origin, const Vector<T>& in_size) :
            origin(in_origin),
            size(in_size)
        {}

        /** Alternate generator treating the vectors as opposite corners instead of a relative size.
          * Must be given as top-left and bottom-right corners, otherwise the returned rectangle will be
          * the empty rectangle.
          */
        static Rect<T> FromCorners(const Vector<T>& tl, const Vector<T>& br)
        {
            if(tl.x >= br.x || tl.y >= br.y)
            {
                return Rect<T>();
            }
            else
            {
                return Rect<T>(tl, br-tl);
            }
        }

        /** Returns true if the rectangle has a valid size.
          * For signed types, assumes negative sizes are invalid.
          */
        bool IsValid() const
        {
            return size.x > 0 && size.y > 0;
        }

        /** returns the origin x coordinate of the rectangle.
          */
        T Left() const
        {
            return origin.x;
        }

        /** returns the origin y coordinate of the rectangle.
          */
        T Top() const
        {
            return origin.y;
        }

        /** returns the width of the rectangle.
          */
        T Width() const
        {
            return size.x;
        }

        /** returns the height of the rectangle.
          */
        T Height() const
        {
            return size.y;
        }

        /** helper to calculate the rightmost x coordinate of the rectangle.
          * (Actually the first x coordinate after the edge of the rectangle.)
          */
        T Right() const
        {
            return Left()+Width();
        }

        /** helper to calculate the bottom y coordinate of the rectangle.
          * (Actually the first y coordinate after the edge of the rectangle.)
          */
        T Bottom() const
        {
            return Top()+Height();
        }

        /** helper to calculate the bottom rightmost coordinate of the rectangle.
          * (Actually one pixel outside the bottom right corner of the rectangle.)
          */
        Vector<T> Extent() const
        {
            return origin + size;
        }

        /** returns true if a vector is contained within the rectangle.
          */
        bool Contains(const Vector<T>& vector) const
        {
            return vector.x >= Left() && vector.y >= Top() &&
                   vector.x < Right() && vector.y < Bottom();
        }

        /** returns true if two rectangles intersect each other
          */
        bool Overlaps(const Rect<T>& other) const
        {
            return other.Left() < Right() && Left() < other.Right()  &&
                   other.Top() < Bottom() && Top() < other.Bottom();
        }

        /** returns true if two rectangles intersect when taking into account
          * wrapping around a passed in modulo.
          */
        bool OverlapsMod(const Rect<int>& other, const Vector<int>& modulo)
        {
            Rect<int> shifted (other.origin - origin, other.size);
            shifted.origin.x %= modulo.x;
            shifted.origin.y %= modulo.y;

            Rect<int> wrapped = shifted;
            if (wrapped.origin.x < 0)
            {
                wrapped.origin.x += modulo.x;
            }
            if (wrapped.origin.y < 0)
            {
                wrapped.origin.y += modulo.y;
            }

            return ((shifted.Left() < size.x && shifted.Right()  > 0) || (wrapped.Left() < size.x && wrapped.Right()  > 0)) &&
                   ((shifted.Top()  < size.y && shifted.Bottom() > 0) || (wrapped.Top()  < size.y && wrapped.Bottom() > 0)) ;

        }
    

        /** Returns the intersection between two rectangles.
          * Overides the (bitwise) AND operator as logical AND is also called Intersection.
          */
        Rect<T> operator&(const Rect<T>& other) const
        {
            auto intersect_origin = Max(origin, other.origin);
            auto intersect_extent = Min(Extent(), other.Extent());
            return FromCorners(intersect_origin, intersect_extent);
        }

        /** Mutating version of the intersection operator above */
        Rect<T>& operator&=(const Rect<T>& other)
        {
            return (*this = (*this & other));
        }

        /** Returns a new rect extended by N on each edge, moving the origin by
          * N units up and left and the bottom edge by the same amount in the
          * opposite direction.
          */
        Rect<T> Inflate(T n) const
        {
            return Rect<T>(origin - Vector<T>(n, n), size + Vector<T>(n+n, n+n));
        }

        Vector<T> origin;
        Vector<T> size;
    };
}
