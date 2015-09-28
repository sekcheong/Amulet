/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#include <am_inc.h>

#include UNIV_MAP__H
#if defined(GCC)
#include <stdlib.h>
#else
extern "C" {
#include <stdlib.h>
#include <memory.h>
}
#endif

Am_IMPL_MAP(Int2Ptr, int, 0, void*, NULL)
Am_IMPL_MAP(Int2Str, int, 0, char*, NULL)
Am_IMPL_MAP(Ptr2Int, void*, NULL, int, 0)
Am_IMPL_MAP(Str2Int, char*, NULL, int, 0)

int HashValue (const char* key, int size)
{
	//simply sum up first two and last two characters and normalize
	unsigned base;
	unsigned len = key ? strlen (key) : 0;
	switch (len) {
		case 0: return 0;
		case 1: base = key[0] * 4; break;
		default: base = key[0] + key[1] + key[len - 2] + key[len - 1]; break;
	}
	return base * unsigned(0x10000L / 4 / 0x100) % size;
}
