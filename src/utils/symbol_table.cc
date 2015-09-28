/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

extern "C" {
#include <stdlib.h>
}

#include <am_inc.h>

#include SYMBOL_TABLE__H

Am_IMPL_MAP(CStr2Int, const char*, NULL, int, -1)

const char* Am_Symbol_Table::operator [] (int value) const
{
	Am_SymTable_Iterator next(this);
	for (int i = next(); next.Key() && (i != value); i = next())
		;
	return next.Key();
}

