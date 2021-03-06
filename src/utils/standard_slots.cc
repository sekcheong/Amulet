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

#include AM_IO__H

#include STANDARD_SLOTS__H
#include SYMBOL_TABLE__H

#include TYPES__H

#define SLOT_NAME_START Am_MAXIMUM_USER_SLOT_KEY+1
// == 30000.

Am_Symbol_Table* Am_Slot_Name_Key_Table = 0; //exported by symbol_table.h

inline void verify_slot_name_key_table()
{
  if (!Am_Slot_Name_Key_Table)
    Am_Slot_Name_Key_Table = Am_Symbol_Table::Create (500);
}

void Am_Register_Slot_Key (Am_Slot_Key key, const char* string)
{
  verify_slot_name_key_table();
  int prev_key = Am_Slot_Name_Key_Table->Get_Value (string);
  if (prev_key != -1) {
    cerr << "Slot Name, " << string << ", has been overwritten." << endl;
    Am_Error ();
  }
  const char* prev_name = Am_Slot_Name_Key_Table->Get_Key (key);
  if (prev_name) {
    cerr << "Slot Key, ";
    Am_Print_Key (cerr, key);
    cerr << ", has already been used." << endl;
    Am_Error ();
  }
  Am_Slot_Name_Key_Table->Add_Item (key, string);
}
     

Am_Slot_Key Am_Register_Slot_Name (const char* string)
{
  // current_name stores the next slot key to allocate
  static Am_Slot_Key current_name = SLOT_NAME_START;
  
  verify_slot_name_key_table();
  int prev_key = Am_Slot_Name_Key_Table->Get_Value (string);
  if (prev_key == -1) {
    Am_Slot_Name_Key_Table->Add_Item (current_name, string);
    if (current_name+1 < SLOT_NAME_START) // then we have overflow
      Am_Error("** Am_Register_Slot_Name: too many slot names registered!\n");
    return current_name++;
  }
  else
    return (Am_Slot_Key)prev_key;
}

// Returns 0 if slot name not found
Am_Slot_Key Am_From_Slot_Name (const char* string) {
  verify_slot_name_key_table();
  int prev_key = Am_Slot_Name_Key_Table->Get_Value (string);
  if (prev_key == -1) return 0;
  else return (Am_Slot_Key)prev_key;
}

const char* Am_Get_Slot_Name (Am_Slot_Key key)
{
  verify_slot_name_key_table();
  return Am_Slot_Name_Key_Table->Get_Key (key);
}

bool Am_Slot_Name_Exists (const char* string)
{
  verify_slot_name_key_table();
  return (Am_Slot_Name_Key_Table->Get_Value (string) != -1);
}

