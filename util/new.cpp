#include "util/new.h"

// Replacement for placement new, missing from the circle library.
// Luckyly, the implementation is trivial.
void* operator new(size_t, void* placement)
{
    return placement;
}
