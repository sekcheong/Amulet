/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file describes a simple symbol_table class that maps strings
 * to ints and vice-versa.
   
   Designed and implemented by Brad Myers
*/

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <am_inc.h>

#include UNIV_MAP__H

Am_DECL_MAP(CStr2Int, const char*, int)

class Am_Symbol_Table : public Am_Map_CStr2Int {
public:
    Am_Symbol_Table (int initial_size = 89) :
    	Am_Map_CStr2Int (initial_size) { };
    
    static Am_Symbol_Table* Create (int initial_size = 89)
    	{ return new Am_Symbol_Table(initial_size); };

    void Add_Item (int value, const char* key)
    	{ SetAt(key, value); };
    //adds the (key value) pair to the symbol table.  If key is already
    //there, then the old value associated with key is replaced with
    //the new value.  More than one key can map to the same value.
    // Key is case significant.
    // You can also use operator[]:    my_sym_tbl[key] = value;

    int Get_Value (const char* key) const
    	{ return GetAt(key); };
    // returns the value associated with the key.  If the key is not
    // found, then -1 is returned
    // You can also use operator[]:    value = my_sym_tbl[key];
    
    int& operator [] (const char* key)
    	{ return Am_Map_CStr2Int::operator[](key); };

    const char* Get_Key (int value) const
    	{ return operator[] (value); };
    // returns the key associated with that value.  If more than one
    // key maps to the same value, then the LAST ADDED key is returned.
    // If not found, returns 0 (null pointer)
    // You can also use operator[]:    key = my_sym_tbl[value];
    
    const char* operator [] (int value) const;

    void Remove_Item (const char* key)
    	{ DeleteKey(key); };
    // removes the entry in the symbol table associated with that value

    int Current_Size () const { return Count(); }
    int Max_Size () const { return Size(); }
};

#define Am_SymTable_Iterator Am_MapIterator_CStr2Int

//the main symbol table for slots
extern Am_Symbol_Table* Am_Slot_Name_Key_Table;

#endif
