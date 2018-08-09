#include <circle/types.h>

/** Replacement for placement new, missing from the circle library
  * Usage: new(allocated_memory) Type(...);
  */
void* operator new(size_t, void*);
