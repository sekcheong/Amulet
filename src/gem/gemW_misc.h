#if !defined(GWMISC_H)
#define GWMISC_H

#include <stdlib.h>
#include <am_inc.h>

#ifdef AMULET2_INSTRUMENT
#undef Set
#undef Add
#endif

#include UNIV_MAP__H
#include UNIV_LST__H


//#pragma pack()

#if !defined(DEBUG)
#define GWFASTCALL	__fastcall
#else
#define GWFASTCALL
#endif

char* strnew (const char* src);

inline void strdel(char* s)
{
	if (s) free((void*)s);
}

#if !defined(_WIN32)
#define CODE_BASED	__based(__segname("_CODE"))
#else
#define CODE_BASED
#endif

#define	LOSHORT(X)	((SHORT)(LOWORD(X)))
#define	HISHORT(X)	((SHORT)(HIWORD(X)))

Am_DECL_MAP(Ptr2Ptr, void*, void*)
Am_DECL_LIST(Long, long, 0L)

#endif
