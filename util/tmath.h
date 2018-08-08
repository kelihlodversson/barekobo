#pragma once

// Various math-related template functions
namespace hfh3
{

    template<typename T> T Min(const T& a, const T& b)
    {
        return (a < b)?a:b;
    }

    template<typename T> T Max(const T& a, const T& b)
    {
        return (a > b)?a:b;
    }

}
