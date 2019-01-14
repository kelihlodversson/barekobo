#include <circle/util.h>
#include "config.h"

namespace hfh3
{
#if CONFIG_OWN_MEMSET

void *MemSet(void *dest, int val, size_t length);

// Ensure we use our own memset
#define memset hfh3::MemSet
#endif
}