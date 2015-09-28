/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the inspector for debugging Amulet objects
   
   Designed and implemented by Brad Myers
*/
#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <am_inc.h>

#include OBJECT__H
#include IDEFS__H
#include TYPES__H

#include VALUE_LIST__H
#include OBJECT_ADVANCED__H	// for Am_Slot

#define Am_INSPECTOR_INTER_PRIORITY 300.0
#define Am_INTERNAL_SLOT_PREFIX '~'

// Sets up interactors so inspector can be brought up for any object in window
extern void Am_Initialize_Inspector();

// inspect the specific object, if a slot is supplied, the select it
extern void Am_Inspect(const Am_Object& object, Am_Slot_Key slot = Am_NO_SLOT);

// inspect the specific object and go into a main_loop
extern void Am_Inspect_And_Main_Loop(const Am_Object& object, 
				     Am_Slot_Key slot = Am_NO_SLOT);

// The next one takes the name of the object.  This is useful from the
// interpreter.
extern void Am_Inspect(const char * name); 

extern void Am_Flash (Am_Object o, ostream &flashout = cout);

extern void Am_Set_Inspector_Keys(Am_Input_Char show_key,
				  Am_Input_Char show_position_key,
				  Am_Input_Char ask_key);

////////////////////////////////////////////////////////////////////
// For tracing
////////////////////////////////////////////////////////////////////

#define Am_NOT_TRACING 0x00
#define Am_TRACING     0x01
#define Am_STORING_OLD 0x02
#define Am_BREAKING    0x04

typedef short Am_Trace_Status;

void Am_Notify_On_Slot_Set (Am_Object object = Am_No_Object, Am_Slot_Key key = 0,
			    Am_Value value = Am_No_Value);
void Am_Break_On_Slot_Set (Am_Object object = Am_No_Object, Am_Slot_Key key = 0,
			   Am_Value value = Am_No_Value);
void Am_Clear_Slot_Notify (Am_Object object = Am_No_Object, Am_Slot_Key key = 0,
			   Am_Value value = Am_No_Value);
void Am_Start_Slot_Value_Tracing(Am_Object object, Am_Slot_Key key, 
                          Am_Object old_values_object = Am_No_Object);
void Am_Stop_Slot_Value_Tracing(Am_Object object, Am_Slot_Key key);

Am_Trace_Status Am_Update_Tracer_String_Object(Am_Object obj, Am_Slot_Key key,
				    Am_Object string_obj);
void Am_Invalidate_All_Tracer_String_Objects(Am_Object the_obj);
Am_Value_List Am_Get_Tracer_Old_Values(Am_Object obj, Am_Slot_Key key,
				       Am_Object object);
Am_Trace_Status Am_Get_Tracer_Status (Am_Object obj, Am_Slot_Key key);

void Am_Add_Old_Values (Am_Value_List old_values, Am_Object value_string,
			Am_Value_List* group_iter);

void Am_Initialize_Tracer ();

void Am_Refresh_Inspector_If_Object_Changed(const Am_Slot& slot, 
					    Am_Set_Reason reason);

#endif
