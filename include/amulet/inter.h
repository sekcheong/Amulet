/* ****************************** -*-c++-*- *******************************
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the definitions for using the Interactors input 
   handling and the command objects for when events happen
   
   Designed and implemented by Brad Myers
*/

#ifndef INTER_H
#define INTER_H

#include <am_inc.h>

#include OBJECT__H  // basic object definitions
#include IDEFS__H   // Am_Input_Char
#include FORMULA__H

//Global variable controlling the number of pixels that the mouse has
//to move before it is considered a DRAG event (like "LEFT_DRAG").
//Default value = 3 pixels.
//Am_Minimum_Move_For_Drag is the maximum the mouse can move
//between the down and the up and still be classified as a "CLICK",
//like "LEFT_CLICK".
extern int Am_Minimum_Move_For_Drag;

////////////////////////////////////////////////////////////////////////
// Am_Inter_Location used in move_grow and new_points interactors
// and commands to store old value, etc.
////////////////////////////////////////////////////////////////////////

class Am_Inter_Location_Data;
  
class Am_Inter_Location {
  Am_WRAPPER_DECL (Am_Inter_Location)
public:
  Am_Inter_Location (); // empty
  Am_Inter_Location (bool as_line, Am_Object ref_obj,
		     int a, int b, int c, int d); 
  Am_Inter_Location (const Am_Object& object);
  void Set_Location (bool as_line, Am_Object ref_obj,
		     int a, int b, int c, int d, bool make_unique = true);
  void Set_Location (bool as_line, Am_Object ref_obj,
		     int a, int b, bool make_unique = true);
  void Get_Location (bool &as_line, Am_Object &ref_obj,
		     int &a, int &b, int &c, int &d) const;
  void Get_Points (int &a, int &b, int &c, int &d) const;
  Am_Object Get_Ref_Obj () const;
  void Get_As_Line (bool &as_line) const;
  void Set_Points (int a, int b, int c, int d, bool make_unique = true);
  void Set_Ref_Obj (const Am_Object& ref_obj, bool make_unique = true);
  void Set_As_Line (bool as_line, bool make_unique = true);
  void Copy_From (Am_Inter_Location& other_obj, bool make_unique = true);
  void Swap_With (Am_Inter_Location& other_obj, bool make_unique = true);
  Am_Inter_Location Copy() const;
  bool Translate_To(Am_Object dest_obj); //trans_coord from ref_obj to dest_obj
  bool operator== (const Am_Inter_Location& test);
  bool operator!= (const Am_Inter_Location& test);
  bool operator>= (const Am_Inter_Location& test); // A >= B is A contains B
  bool operator&& (const Am_Inter_Location& test); // test overlapping

  void Install (Am_Object& object, bool growing = true) const;
};

extern Am_Inter_Location Am_No_Location;

ostream& operator<< (ostream& os, Am_Inter_Location& loc);

////////////////////////////////////////////////////////////////////////

// type of method in the Am_START_WHERE_TEST and Am_RUNNING_WHERE_TEST slots
// x and y are w.r.t. event_window
Am_Define_Method_Type(Am_Where_Method, Am_Object,
		      (Am_Object inter, Am_Object object,
		       Am_Object event_window, int x, int y))

// type of method that can be stored in the Am_START_WHEN, Am_STOP_WHEN, and
// Am_ABORT_WHEN slots.
Am_Define_Method_Type(Am_Event_Method, bool,
		      (Am_Object inter, Am_Object event_window,
		       Am_Input_Char ic))

// type of method in the Am_GRID_METHOD slot (to handle custom gridding)
Am_Define_Method_Type(Am_Custom_Gridding_Method, void,
		      (Am_Object inter, const Am_Object& ref_obj, int x, int y,
		       int& out_x, int & out_y))

// type of method in the Am_CREATE_NEW_OBJECT_METHOD slot of
//     Am_New_Points_Interactor.
// Should return the new object created.
// ** old_object is Valid if this is being called as a result of a
// Repeat undo call, and means that a new object should be created
// like old_object.  
Am_Define_Method_Type(Am_Create_New_Object_Method, Am_Object,
		      (Am_Object inter, Am_Inter_Location location,
		       Am_Object old_object))

extern Am_Input_Char Am_Default_Start_Char;
extern Am_Input_Char Am_Default_Stop_Char;

// The actual interactor objects

extern Am_Object Am_Interactor;  //base of the Interactor hierarchy

// the next ones are the ones you usually would use

extern Am_Object Am_Choice_Interactor;  // choosing one or more from a set
extern Am_Object Am_One_Shot_Interactor;  // goes immediately at input event
extern Am_Object Am_Move_Grow_Interactor;  // moving and growing with mouse
extern Am_Object Am_New_Points_Interactor;  // entering new points with mouse
extern Am_Object Am_Text_Edit_Interactor;  // text editing
extern Am_Object Am_Rotate_Interactor;  // rotating
extern Am_Object Am_Gesture_Interactor;  // handling gestures
extern Am_Object Am_Animation_Interactor;  // handling animations

// Some methods for the _Where tests.
// x and y are with respect to event_window
extern Am_Where_Method Am_Inter_In;
extern Am_Where_Method Am_Inter_In_Part;
extern Am_Where_Method Am_Inter_In_Object_Or_Part; //default in interactors
extern Am_Where_Method Am_Inter_In_Leaf;
extern Am_Where_Method Am_Inter_In_Text;
extern Am_Where_Method Am_Inter_In_Text_Part;// in text part?
extern Am_Where_Method Am_Inter_In_Text_Object_Or_Part; //default in text inter
extern Am_Where_Method Am_Inter_In_Text_Leaf;

//Methods of this type go into the Am_TEXT_EDIT_METHOD slot for
//text_edit_interactors
Am_Define_Method_Type(Am_Text_Edit_Method, void,
		      (Am_Object text, Am_Input_Char ic, Am_Object inter))

enum Am_Text_Abort_Or_Stop_Code { Am_TEXT_OK, Am_TEXT_ABORT_AND_RESTORE,
				  Am_TEXT_KEEP_RUNNING, Am_TEXT_STOP_ANYWAY};

//Methods of this type go into the Am_TEXT_CHECK_LEGAL_METHOD slot for
//text_edit_interactors
Am_Define_Method_Type(Am_Text_Check_Legal_Method, Am_Text_Abort_Or_Stop_Code,
		      (Am_Object &text, Am_Object &inter))

// Other functions

//Explicitly abort an interactor.  If not running, does nothing.
extern void Am_Abort_Interactor(Am_Object inter, bool update_now = true);

//Explicitly stop an interactor.  If not running, raises an error.
//If not supply stop_obj, uses last one from inter.  
// If not supply stop_window, uses stop_obj's and sets stop_x and stop_y
//to stop_obj's origin.  If supply stop_window, must also supply stop_x _y
extern void Am_Stop_Interactor(Am_Object inter,
			       Am_Object stop_obj = Am_No_Object,
			       Am_Input_Char stop_char = Am_Default_Stop_Char,
			       Am_Object stop_window = Am_No_Object, int stop_x = 0,
			       int stop_y = 0, bool update_now = true);

//Explicitly start an interactor.  If already running, does nothing.
//If not supply start_obj, uses inter's owner.
// If not supply start_window, uses start_obj's and sets start_x and start_y
//to start_obj's origin.  If supply start_window, must also supply start_x _y
extern void Am_Start_Interactor(Am_Object inter,
			Am_Object start_obj = Am_No_Object,
		       	Am_Input_Char start_char = Am_Default_Start_Char,
			Am_Object start_window = Am_No_Object, int start_x = 0,
			int start_y = 0, bool update_now = true);

/// The Am_Clip_And_Map procedure works as follows:
///    (Am_Clip_And_Map val, val_1, val_2, target_val_1, target_val_2) takes
///    val, clips it to be in the range val_1 .. val_2, and if target_val_1 and
///    target_val_2 are provided, then scales and
///    translates the value (using linear_interpolation) to be between
///    target_val_1 and target_val_2.  There are integer and float versions of
//     this function.
///   val_1 is allowed to be less than or greater than val_2.

extern long Am_Clip_And_Map(long val, long val_1, long val_2, 
			   long target_val_1 = 0, long target_val_2 = 0);
extern float Am_Clip_And_Map(float val, float val_1, float val_2,
			     float target_val_1 = 0.0,
			     float target_val_2 = 0.0);

// debugging functions

enum Am_Inter_Trace_Options { Am_INTER_TRACE_NONE, Am_INTER_TRACE_ALL, 
			      Am_INTER_TRACE_EVENTS, Am_INTER_TRACE_SETTING, 
			      Am_INTER_TRACE_PRIORITIES, Am_INTER_TRACE_NEXT,
			      Am_INTER_TRACE_SHORT };

void Am_Set_Inter_Trace(); //prints current status
void Am_Set_Inter_Trace(Am_Inter_Trace_Options trace_code); //add trace of that
void Am_Set_Inter_Trace(Am_Object inter_to_trace);  //add trace of that inter
void Am_Clear_Inter_Trace(); //set not trace, same as Am_Set_Inter_Trace(0)

// type of the Am_HOW_SET slot for Choice Interactors
Am_Define_Enum_Long_Type(Am_Choice_How_Set)
const Am_Choice_How_Set Am_CHOICE_SET (0);
const Am_Choice_How_Set Am_CHOICE_CLEAR (1);
const Am_Choice_How_Set Am_CHOICE_TOGGLE (2);
const Am_Choice_How_Set Am_CHOICE_LIST_TOGGLE (3);

//This enum is used internally; users should use the Am_Move_Grow_Where_Attach
// values instead.
enum Am_Move_Grow_Where_Attach_vals
{ Am_ATTACH_WHERE_HIT_val, Am_ATTACH_NW_val,
  Am_ATTACH_N_val, Am_ATTACH_NE_val,
  Am_ATTACH_E_val, Am_ATTACH_SE_val, Am_ATTACH_S_val, 
  Am_ATTACH_SW_val, Am_ATTACH_W_val, 
  Am_ATTACH_END_1_val, Am_ATTACH_END_2_val,  
  Am_ATTACH_CENTER_val };

// type of the Am_WHERE_ATTACH slot for Move_Grow Interactors
Am_Define_Enum_Type(Am_Move_Grow_Where_Attach, Am_Move_Grow_Where_Attach_vals)
const Am_Move_Grow_Where_Attach Am_ATTACH_WHERE_HIT(Am_ATTACH_WHERE_HIT_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_NW(Am_ATTACH_NW_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_N(Am_ATTACH_N_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_NE(Am_ATTACH_NE_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_E(Am_ATTACH_E_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_SE(Am_ATTACH_SE_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_S(Am_ATTACH_S_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_SW(Am_ATTACH_SW_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_W(Am_ATTACH_W_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_END_1(Am_ATTACH_END_1_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_END_2(Am_ATTACH_END_2_val);
const Am_Move_Grow_Where_Attach Am_ATTACH_CENTER(Am_ATTACH_CENTER_val);

////////////////////////////////////////////////////////////////////////
// The command objects you can make instances of:

extern Am_Object Am_Command;  //base of the Command object hierarchy

////////////////////////////////////////////////////////////////////////
// UNDO stuff
////////////////////////////////////////////////////////////////////////

//set this into the Am_IMPLEMENTATION_PARENT of operations like
//selection and scrolling that are not usually queued for undoing
#define Am_NOT_USUALLY_UNDONE -1

//set this into the Am_IMPLEMENTATION_CHILD if don't want children
//commands undone when this is undone
#define Am_DONT_UNDO_CHILDREN -2

////////////////////////////////////////////////////////////////////////
// The UNDO objects you can make instances of.  These should be in the
//  Am_UNDO_HANDLER slots of either a window or an application object
//  which is in the Am_APPLICATION slot of a window

extern Am_Object Am_Undo_Handler; // general, prototype undo handler obj
extern Am_Object Am_Single_Undo_Object;    // can only undo last command
extern Am_Object Am_Multiple_Undo_Object;  // can undo all top-level commands

// Methods in UNDO objects include REGISTER_COMMAND, PERFORM_UNDO,
// PERFORM_UNDO_THE_UNDO

// REGISTER_COMMAND takes a procedure of the form:
Am_Define_Method_Type(Am_Register_Command_Method, bool,
		      (Am_Object undo_handler, Am_Object command_obj))

// PERFORM_UNDO and UNDO_THE_UNDO slots are of type
//  Am_Object_Method (in types.h)

//////  Method types for selective undo
// For the Am_SELECTIVE_UNDO_ALLOWED and Am_SELECTIVE_REPEAT_SAME_ALLOWED
// slots:
Am_Define_Method_Type(Am_Selective_Allowed_Method, bool,
		      (Am_Object command_obj))

// For the Am_SELECTIVE_REPEAT_NEW_ALLOWED slot:
Am_Define_Method_Type(Am_Selective_New_Allowed_Method, bool,
		      (Am_Object command_obj, Am_Value new_selection))

// for the Am_SELECTIVE_REPEAT_ON_NEW_METHOD slot
Am_Define_Method_Type(Am_Selective_Repeat_New_Method, void,
		      (Am_Object command_obj, Am_Value new_selection))

// for the Am_SELECTIVE_UNDO_METHOD and
// Am_SELECTIVE_REPEAT_SAME_METHOD slots of the UNDO HANDLER
Am_Define_Method_Type(Am_Handler_Selective_Undo_Method, void,
		      (Am_Object undo_handler, Am_Object command_obj))

// for the Am_SELECTIVE_REPEAT_ON_NEW_METHOD slot of the UNDO HANDLER
Am_Define_Method_Type(Am_Handler_Selective_Repeat_New_Method, void,
		      (Am_Object undo_handler, Am_Object command_obj,
		       Am_Value new_selection))

///////////////////////////////////////////////////////////////////////////

//method types that can go into the inter DO action slots:
// Am_Object_Method (just passed the command or inter object)
// Am_Mouse_Event_Method: passed command or inter object and mouse point
// Am_Current_Location_Method: passed cmd/inter obj, obj modified, and
// 				points for obj
// These types can also go into the command objects directly put into
// an interactor, but the command objects put into widgets must be of 
// Am_Object_Method type only

//this one can be used with any interactor, but not with high-level commands
Am_Define_Method_Type(Am_Mouse_Event_Method, void,
		      (Am_Object inter_or_cmd, int mouse_x, int mouse_y,
		       Am_Object ref_obj, Am_Input_Char ic))

//this one can only be used with move_grow or new_points interactors
Am_Define_Method_Type(Am_Current_Location_Method, void,
		      (Am_Object inter_or_cmd, Am_Object obj_modified,
		       Am_Inter_Location points))

/////////////////////////////////////////////////////////////////////////
//  Pop Up Windows
/////////////////////////////////////////////////////////////////////////
//Typical usage: programmer calls Am_Pop_Up_Window_And_Wait, and
//the window has a call to Am_Finish_Pop_Up_Waiting as part of the
//do_method of the OK and CANCEL buttons, with OK setting the value to
//something, and cancel setting the value to NULL.

// Sets the visible of the window to true, and then waits for someone
// to call Am_Finish_Pop_Up_Waiting on that window.  Returns the value
// passed to Am_Finish_Pop_Up_Waiting.
extern void Am_Pop_Up_Window_And_Wait(Am_Object window,
				      Am_Value &return_value,
				      bool modal = true);

//Sets window's visible to FALSE, and makes the
//Am_Pop_Up_Window_And_Wait called on the same window return
extern void Am_Finish_Pop_Up_Waiting(Am_Object window, Am_Value return_value);

#endif
