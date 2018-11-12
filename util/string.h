#pragma once
#include "util/array.h"
#include <assert.h>

namespace hfh3
{
    /** Array<char> subclass for simple string manipulations
      */
    class String : public Array<char>
    {
    public:
        String() 
            : Array()
        {}

        String(const Array<char>& other) : Array(other)
        {}

        String(const char* inString) 
        {
            AppendRaw(inString, strlen(inString));
        }

        int Compare(String& other)
        {
            return strcmp(this->operator char*(), other.operator char*());
        }

        bool operator == (String& other)
        {
            return Compare(other) == 0;
        }

        bool operator < (String& other)
        {
            return Compare(other) < 0;
        }

        bool operator > (String& other)
        {
            return Compare(other) > 0;
        }

        bool operator != (String& other)
        {
            return Compare(other) != 0;
        }

        bool operator <= (String& other)
        {
            return Compare(other) <= 0;
        }

        bool operator >= (String& other)
        {
            return Compare(other) >= 0;
        }



        String& operator += (const char* other)
        {
            AppendRaw(other, strlen(other));
            return *this;
        }
        String& operator += (const String& other)
        {
            Append(static_cast<const Array<char>&>(other));
            return *this;
        }

        String& operator += (char c)
        {
            Append(c);
            return *this;
        }

        String& operator += (int i)
        {
            bool negative = i < 0;
            unsigned int u = i;
            if(negative)
            {
                u = -i;
                Append('-');
            }
            
            return this->operator += (u);
        }

        String& operator += (unsigned int u)
        {
            char digits[10];
            
            int pos = 10;
            do {
                pos --;
                assert(pos >= 0);
                int modulo = u % 10;
                digits[pos] = '0' + modulo;

                u /= 10;
            } while (u != 0);

            AppendRaw(digits+pos, 10-pos);
            return *this;   
        }

        template<typename T>
        String operator + (T& other) const
        {
            return (String(*this) += other);
        }

        operator char* () 
        {
            Terminate();
            return this->Array<char>::operator char *();
        }
    };
}