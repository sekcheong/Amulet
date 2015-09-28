/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the definitions for IMPLEMENTING Interactors.
   Normally, this won't be used by users, unless they are implementing
   a new kind of interactor
   
   Designed and implemented by Brad Myers
*/

#ifndef INTER_ADVANCED_H
#define INTER_ADVANCED_H

#include <am_inc.h>

#include GEM__H
#include INTER__H
#include FORMULA__H
#include VALUE_LIST__H  // for Am_Value_List type

// Amount priority of interactors added when running
#define Am_INTER_PRIORITY_DIFF 100.0f

// call this before doing any interactor stuff.  This should be
// called automatically from Am_Initialize();
extern void Am_Initialize_Interactors ();

// for the Am_CURRENT_STATE slot of interactors
enum Am_Inter_State { Am_INTER_WAITING, Am_INTER_RUNNING, Am_INTER_OUTSIDE, 
		       Am_INTER_ABORTING, Am_INTER_ANIMATING };

// for the Am_CURRENT_STATE slot of interactors
enum Am_Modal_Waiting { Am_INTER_NOT_WAITING, Am_INTER_WAITING_MODAL,
			 Am_INTER_WAITING_NOT_MODAL };

// type of the function in the various internal _METHOD slots of
// interactors (not commands) 
Am_Define_Method_Type(Am_Inter_Internal_Method, void, 
		      (Am_Object& inter, Am_Object& object,
		       Am_Object& event_window, Am_Input_Event *ev))

/////////////////////////////////////////////////////////////////////////////
// Main top-level input handler.  Figures out which interactors to call for
// the event.  This is called from the Input_Event_Notify method for the
// default event handler class defined in Opal.
/////////////////////////////////////////////////////////////////////////////

extern void Interactor_Input_Event_Notify(Am_Object event_window,
					  Am_Input_Event *ev);

/////////////////////////////////////////////////////////////////////////////
//Debugging: These are used internally to see if should print something
/////////////////////////////////////////////////////////////////////////////

extern bool Am_Inter_Tracing (Am_Inter_Trace_Options trace_code);
extern bool Am_Inter_Tracing (Am_Object inter_to_trace);

#ifdef DEBUG

#define Am_INTER_TRACE_PRINT(condition, printout) \
 if (Am_Inter_Tracing(condition))        \
   cout << printout << endl << flush

#define Am_INTER_TRACE_PRINT_NOENDL(condition, printout) \
 if (Am_Inter_Tracing(condition))        \
   cout << printout

extern void Am_Report_Set_Vis(Am_Object inter, Am_Object obj, bool value);

#define Am_REPORT_VIS_CONDITION(condition, inter, obj, value) \
 if (Am_Inter_Tracing(condition))       		   \
     Am_Report_Set_Vis(inter, obj, value);

#else

#define Am_INTER_TRACE_PRINT(condition, printout)
    /* if not debugging, define it to be nothing */

#define Am_INTER_TRACE_PRINT_NOENDL(condition, printout) 
    /* if not debugging, define it to be nothing */

#define Am_REPORT_VIS_CONDITION(condition, inter, obj, value)
    /* if not debugging, define it to be nothing */

#endif

/////////////////////////////////////////////////////////////////////////////
// Generally useful functions
/////////////////////////////////////////////////////////////////////////////

//checks that object & all owner's are visible (all the way to the Am_Screen).
extern bool Am_Object_And_Owners_Valid_And_Visible(Am_Object obj);
extern bool Am_Object_And_Owners_Valid_And_Visible(
						   Am_Object obj);

// calls Am_Object_And_Owners_Valid_And_Visible on each object in
// value, which can be a single object or a value list of objects
bool Am_Valid_and_Visible_List_Or_Object(Am_Value value,
					 bool want_visible = true);

// Applies the gridding, if any, defined by the inter's GRID slots
extern void Am_Get_Filtered_Input(Am_Object inter, const Am_Object& ref_obj,
				  int x, int y, int& out_x, int & out_y);

// sets the left,top,width,height or x1,y1,x2,y2 of obj from data
extern void Am_Modify_Object_Pos(Am_Object& obj, const Am_Inter_Location& data,
				 bool growing);

//turn_off saves the old value of the animator on the left slot, if any, and
//turns it off.  Restore sets it back to its original value.
// extern void Am_Temporary_Turn_Off_Animator(Am_Object &obj);
// extern void Am_Temporary_Restore_Animator(Am_Object &obj);

extern void Am_Choice_Set_Value (Am_Object inter, bool set_selected);

extern bool Am_Inter_Call_Both_Method(Am_Object& inter, Am_Object& command_obj,
				      Am_Slot_Key method_slot, int x, int y,
				      Am_Object& ref_obj, Am_Input_Char& ic, 
				      Am_Object& object_modified,
				      Am_Inter_Location& points);
extern void Am_Inter_Call_Method(Am_Object& inter_or_cmd,
				 Am_Slot_Key method_slot,
				 int x, int y, Am_Object& ref_obj,
				 Am_Input_Char& ic, Am_Object& object_modified,
				 Am_Inter_Location& points);

typedef Am_Object Am_Impl_Command_Setter(Am_Object inter,
					 Am_Object object_modified,
					 Am_Inter_Location data);
extern void Am_Call_Final_Do_And_Register(Am_Object inter,
				  Am_Object command_obj,
				  int x, int y, Am_Object ref_obj,
				  Am_Input_Char ic,
		 		  Am_Object object_modified,
				  Am_Inter_Location data,
				  Am_Impl_Command_Setter* impl_command_setter);
extern void Am_Register_For_Undo(Am_Object inter, Am_Object command_obj,
				 Am_Object object_modified,
				 Am_Inter_Location data,
				 Am_Impl_Command_Setter* impl_command_setter);

//copies Am_VALUE, Am_OBJECT_MODIFIED and Am_SAVED_OLD_OWNER from
//from_object to its Am_COMMAND part
extern void Am_Copy_Values_To_Command(Am_Object from_object);

//exported for when want to pretend a move-grow interim method is a
//final method, like for scroll bar indicators (scroll_widgets.cc)  
extern void Am_Move_Grow_Register_For_Undo(Am_Object inter);

// formula used in the Am_SET_COMMAND_OLD_OWNER to set the old_owner
// slot of command objects
extern Am_Formula Am_Set_Old_Owner_To_Me;

/////// used for multi-window moves (move-grow and new-points)

// Checks if the window that feedback is in matches the
// window in the interactor, and if not, moves the feedback object to the
// corresponding owner in Am_MULTI_FEEDBACK_OWNERS or Am_MULTI_OWNERS
// returns the owner if change something
extern Am_Object Am_Check_And_Fix_Feedback_Group (Am_Object& feedback,
					     const Am_Object& inter);

// Checks if the window that object is in matches the
// window in interactor, and if not, moves
// the object to the corresponding owner in Am_MULTI_OWNERS.
// Returns true if everything is OK.  Returns false if should abort.
// Sets new_owner if changes the owner, otherwise new_owner is Am_No_Object
extern bool Am_Check_And_Fix_Object_Group (Am_Object& obj,
					   const Am_Object &inter,
					   Am_Object &new_owner);

// Like Check_And_Fix_Object_Group this procedure determines what group the
// given object belongs to.  This returns the group instead of automatically
// setting it.  Returns Am_No_Object instead of returning false.
extern Am_Object Am_Find_Destination_Group (const Am_Object& obj,
					    const Am_Object& inter);

//Used when interactor needs to be added or removed from an extra
//window's list, for example when the feedback object is a window
//itself, call this on the window  If add and want_move, then turns
//on mouse-moved events for the window.
extern void Am_Add_Remove_Inter_To_Extra_Window(Am_Object inter,
						Am_Object window, bool add,
						bool want_move);

//Use when Feedback is a window.  Make sure window in inter isn't feedback,
//and if so, find a new window and return it (also fixes the window
//and interim_x and _y in the inter).  Call this AFTER feedback
//has disappeared.  Otherwise, just returns inter_window.
extern Am_Object
  Am_Check_And_Fix_Point_In_Feedback_Window(Am_Object &inter,
					    Am_Object &feedback);

//Use to replace owner of object when undoing and redoing.  Makes sure owner
// of obj is data.ref_obj and if not, moves it
extern bool Am_Check_And_Fix_Owner_For_Object(Am_Object &obj,
					      Am_Inter_Location &data);

//returns true if valid.  I.e. returns false if object is marked as
//invalid for slot_for_inactive 
extern bool Am_Check_One_Object_For_Inactive(Am_Object &object,
	      Am_Slot_Key slot_for_inactive);
//returns false if object is marked as invalid for slot_for_inactive
//uses Am_SLOT_FOR_THIS_COMMAND_INACTIVE of self
extern bool Am_Check_One_Object_For_Inactive_Slot(Am_Object &object,
		   Am_Object &self);

extern bool Am_Check_All_Objects_For_Inactive(Am_Value_List &selection,
	      Am_Slot_Key slot_for_inactive);
extern bool Am_Check_All_Objects_For_Inactive_Slot(Am_Value_List &selection,
		   Am_Object &self);
extern bool Am_Check_One_Or_More_For_Inactive(Am_Value list_or_obj,
	      Am_Slot_Key slot_for_inactive);
extern bool Am_Check_One_Or_More_For_Inactive_Slot(Am_Value list_or_obj,
		   Am_Object &self);

//returns false is inactive so should abort, true if OK
extern bool Am_Check_Inter_Abort_Inactive_Object(Am_Object &object,
					 Am_Slot_Key slot_for_inactive,
					 Am_Object &inter);

//////////////////////////////////////////////////////////////////
// Initialization Routines
//////////////////////////////////////////////////////////////////

extern void Am_Initialize_Top_Command ();
extern void Am_Initialize_Move_Grow_Interactor ();
extern void Am_Initialize_Choice_Interactor ();
extern void Am_Initialize_New_Points_Interactor ();
extern void Am_Initialize_Text_Interactor ();
extern void Am_Initialize_Animation_Interactor ();
extern void Am_Initialize_Animated_Constraints ();

extern void Am_Cleanup_Animated_Constraints ();

////////////////////////////////////////////////////////////////////////
// For the UNDO mechanisms
////////////////////////////////////////////////////////////////////////

//returns the undo handler attached to the window of inter_or_widget
extern Am_Object Am_Inter_Find_Undo_Handler(const Am_Object& inter_or_widget);

// command_obj's DO has already been "done", now do the DO methods of
// all the Am_IMPLEMENTATION_PARENT's of command_obj, recursively on up
extern void Am_Process_All_Parent_Commands(Am_Object& command_obj,
			   Am_Object& undo_handler,
			   const Am_Register_Command_Method &reg_method);

// Call the command's do method and process all the parents as well.  The
// widget parameter is provided to find the needed undo handler.
extern void Am_Execute_Command (Am_Object& command, const Am_Object& widget);

extern Am_Selective_Allowed_Method Am_Selective_Allowed_Return_True;
extern Am_Selective_Allowed_Method Am_Selective_Allowed_Return_False;
extern Am_Selective_New_Allowed_Method 
			Am_Selective_New_Allowed_Return_False;
extern Am_Selective_New_Allowed_Method 
			Am_Selective_New_Allowed_Return_True;

extern void Am_Modify_Command_Label_Name(Am_Object cmd, Am_Slot_Key slot);

////////////////////////////////////////////////////////////////////////
// Miscellaneous
////////////////////////////////////////////////////////////////////////

//Initialize the state if not there yet
extern Am_Inter_State Am_Get_Inter_State(Am_Object inter);

//global list of all modal windows currently in force
extern Am_Value_List Am_Modal_Windows;
extern void Am_Push_Modal_Window(Am_Object &window);
extern void Am_Remove_Modal_Window(Am_Object &window);

//like choice interactor, but when repeat on new, actually does repeat
//on same.  This is used by button widgets.
extern Am_Object Am_Choice_Interactor_Repeat_Same;

extern Am_Object Am_Choice_Internal_Command;
extern Am_Object Am_Move_Grow_Internal_Command;
extern Am_Object Am_New_Points_Internal_Command;
extern Am_Object Am_Edit_Text_Internal_Command;

//for a text_interactor, calls the Am_TEXT_CHECK_LEGAL_METHOD
extern Am_Text_Abort_Or_Stop_Code am_check_text_legal(Am_Object &inter,
						      Am_Object &text);

#endif
