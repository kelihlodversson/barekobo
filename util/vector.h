#pragma once
#include "util/tmath.h"

namespace hfh3
{
    // Simple data stucture for 2d integer vectors
    template<typename T>
    struct Vector
    {
        Vector() :
            x(), y()
        {}

        Vector(T in_x, T in_y) :
            x(in_x),
            y(in_y)
        {}

        Vector<T>& operator+=(const Vector<T>& other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vector<T>& operator-=(const Vector<T>& other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vector<T>& operator*=(T scale)
        {
            x *= scale;
            y *= scale;
            return *this;
        }

        Vector<T>& operator/=(T dividend)
        {
            x /= dividend;
            y /= dividend;
            return *this;
        }

        Vector<T> operator+(const Vector<T>& other) const
        {
            return Vector<T>(x+other.x, y+other.y);
        }

        Vector<T> operator-(const Vector<T>& other) const
        {
            return Vector<T>(x-other.x, y-other.y);
        }

        Vector<T> operator*(T scale) const
        {
            return Vector<T>(x*scale, y*scale);
        }

        Vector<T> operator/(T dividend) const
        {
            return Vector<T>(x/dividend, y/dividend);
        }

        bool IsZero() const
        {
            return !x && !y;
        }

        /** Return the lowest x and y coordinate of two vectors
          * This is useful for instance when calculating the intersection of two rectangles
          */
        friend Vector<T> Min(const Vector<T>& a, const Vector<T>&b)
        {
            return Vector<T>(Min(a.x, b.x), Min(a.y,b.y));
        }

        /** Return the highest x and y coordinate of two vectors
          * This is useful for instance when calculating the intersection of two rectangles
          */
        friend Vector<T> Max(const Vector<T>& a, const Vector<T>&b)
        {
            return Vector<T>(Max(a.x, b.x), Max(a.y,b.y));
        }

        T x;
        T y;
    };

}
