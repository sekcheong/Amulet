#ifndef AM_IO_H
#define AM_IO_H

extern "C" {
#include <stdio.h>
}

#include <iostream.h>

#if defined(TRACE)
#define Am_TRACE(A)	{ cout << A; }
#else
#define Am_TRACE(A)	{ ; }
#endif

#endif
