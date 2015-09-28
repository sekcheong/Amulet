#include <string.h>

#include "gemW_misc.h"

char* strnew (const char* src)
{
  return src? strcpy((char*)malloc(strlen(src) + 1), src) : NULL;
}

Am_IMPL_LIST(Long, long, 0L)
Am_IMPL_MAP(Ptr2Ptr, void*, NULL, void*, NULL)
