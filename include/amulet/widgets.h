/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the definitions for widgets in the various 
   looks-and-feels
*/

#ifndef WIDGETS_H
#define WIDGETS_H

#include <am_inc.h>

#include OBJECT__H  //basic object definitions
#include OPAL__H
#include INTER__H  // command objects, etc.
#include VALUE_LIST__H
#include RICH_TEXT__H
#include UNDO_DIALOG__H
#include STR_STREAM__H

//This enum is used internally; users should use the Am_Widget_Look
// values instead.
enum Am_Widget_Look_vals
{
  Am_MOTIF_LOOK_val, Am_WINDOWS_LOOK_val, Am_MACINTOSH_LOOK_val,
  #if defined( _WINDOWS )
    Am_NATIVE_LOOK_val = Am_WINDOWS_LOOK_val
  #elif defined( _MACINTOSH )
    Am_NATIVE_LOOK_val = Am_MACINTOSH_LOOK_val
  #else
    Am_NATIVE_LOOK_val = Am_MOTIF_LOOK_val
  #endif
};

// options for the Am_WIDGET_LOOK slot of widgets to determine how it is drawn
Am_Define_Enum_Type(Am_Widget_Look, Am_Widget_Look_vals)
const Am_Widget_Look Am_MOTIF_LOOK (Am_MOTIF_LOOK_val);
const Am_Widget_Look Am_WINDOWS_LOOK (Am_WINDOWS_LOOK_val);
const Am_Widget_Look Am_MACINTOSH_LOOK (Am_MACINTOSH_LOOK_val);
const Am_Widget_Look Am_NATIVE_LOOK (Am_NATIVE_LOOK_val);

void Am_Set_Default_Look( Am_Widget_Look inLook = Am_NATIVE_LOOK );

extern Am_Input_Char Am_Default_Widget_Start_Char;

extern Am_Object Am_Widget_Aggregate;
extern Am_Object Am_Widget_Group;
extern Am_Object Am_Widget_Map;

extern Am_Object Am_Border_Rectangle;  // a rectangle with fancy edges
extern Am_Object Am_Button;  // a single button
extern Am_Object Am_Button_Panel;  // button panel
extern Am_Object Am_Checkbox_Panel;  // check box panel
extern Am_Object Am_Radio_Button_Panel;  // check box panel
extern Am_Object Am_Menu;
extern Am_Object Am_Menu_Bar;
extern Am_Object Am_Text_Input_Widget;
extern Am_Object Am_Password_Input_Widget;
extern Am_Object Am_Number_Input_Widget;
extern Am_Object Am_Option_Button;
extern Am_Object Am_Pop_Up_Menu_Interactor;
extern Am_Object Am_Combo_Box;

extern Am_Object Am_Menu_Line_Command; //command object to put a line in menu

extern Am_Object Am_Vertical_Scroll_Bar;
extern Am_Object Am_Horizontal_Scroll_Bar;
extern Am_Object Am_Scrolling_Group; // a group with 2 scroll bars
extern Am_Object Am_Vertical_Up_Down_Counter;

extern Am_Object Am_Tab_To_Next_Widget_Command;
extern Am_Object Am_Tab_To_Next_Widget_Interactor;

extern Am_Object Am_Selection_Widget;
extern Am_Object Am_Selection_Widget_Select_All_Command; //to select all
extern Am_Object Am_Drop_Target_Command;

// Sharing menubar accelerators

// new_window will use the same accelerators as source_window
extern void Am_Share_Accelerators(Am_Object &source_window,
				  Am_Object &new_window);

////////////////////////////////////////////////////////////////////////
// Starting, Aborting and Stopping Widgets
////////////////////////////////////////////////////////////////////////
// (these call Start,Stop, and Abort Interactor on the appropriate
//    internal interactors.

//Explicitly start a widget running.  If already running, does nothing.
//If an initial value is provided, then the widget is started with
//this as its value.  It is up to the programmer to make sure the
//Value is legal for the type of widget.  If no initial_value is
//supplied, the widget is started with its current value, if any.

extern void Am_Start_Widget(Am_Object widget,
			    Am_Value initial_value = Am_No_Value);

//Explicitly abort a widget, interactor or command object.  Usually,
//this will be called with a command object, and the system will find
//the associated widget or interactor and abort it.
// If not running, does nothing.  Tries to make sure the command
// object is not entered into the command history.

extern void Am_Abort_Widget(Am_Object widget_or_inter_or_command);

//Explicitly stop a widget.  If not running, raises an error.  If
//final_value is supplied, then this is the value used as the value of
//the widget.  If final_value is not supplied, the widget uses its current
//value.  Commands associated with the widget will be invoked just as
//if the widget had terminated normally.
extern void Am_Stop_Widget(Am_Object widget,
			   Am_Value final_value = Am_No_Value);

////////////////////////////////////////////////////////////////////////
/////// Graphical editing operations////////////////
////////////////////////////////////////////////////////////////////////

// For the property command, to get and set the object's properties
Am_Define_Method_Type(Am_Get_Widget_Property_Value_Method, void,
		      (Am_Object command_obj, Am_Value &new_value))
Am_Define_Method_Type(Am_Get_Object_Property_Value_Method, void,
		      (Am_Object command_obj, Am_Object object, 
		       Am_Value &old_value))
Am_Define_Method_Type(Am_Set_Object_Property_Value_Method, void,
		      (Am_Object command_obj, Am_Object object,
		       Am_Value new_value))

//put these into the Am_HANDLE_OPEN_SAVE_METHOD slot of the command
Am_Define_Method_Type(Am_Handle_Loaded_Items_Method, void,
		      (Am_Object command_obj, Am_Value_List &contents))
Am_Define_Method_Type(Am_Items_To_Save_Method, Am_Value_List,
		      (Am_Object command_obj))

Am_Define_Method_Type(Am_Drop_Target_Interim_Do_Method, bool,
		      (Am_Object& command_obj, const Am_Value& value))
Am_Define_Method_Type(Am_Drop_Target_Do_Method, void,
		      (Am_Object& command_obj, const Am_Value& new_value))

enum Am_Background_Drop_Result
{
  Am_DROP_NORMALLY, Am_DROP_NOT_ALLOWED, Am_DROP_IN_TARGET
};

Am_Define_Method_Type(Am_Drop_Target_Background_Interim_Do_Method,
		      Am_Background_Drop_Result,
		      (Am_Object& command_obj, const Am_Inter_Location& loc,
		       const Am_Value& value))
Am_Define_Method_Type(Am_Drop_Target_Background_Do_Method, void,
		      (Am_Object& command_obj, const Am_Inter_Location& loc,
		       const Am_Value& new_value))

extern Am_Where_Method Am_In_Target;

extern Am_Object Am_Global_Clipboard;

extern Am_Object Am_Graphics_Set_Property_Command; //for color, font, etc.

extern Am_Object Am_Graphics_Clear_Command; //delete
extern Am_Object Am_Graphics_Clear_All_Command;
extern Am_Object Am_Graphics_Copy_Command;
extern Am_Object Am_Graphics_Cut_Command;
extern Am_Object Am_Graphics_Paste_Command;
extern Am_Object Am_Undo_Command;
extern Am_Object Am_Redo_Command;
extern Am_Object Am_Graphics_To_Bottom_Command;
extern Am_Object Am_Graphics_To_Top_Command;
extern Am_Object Am_Graphics_Duplicate_Command;
extern Am_Object Am_Graphics_Group_Command;
extern Am_Object Am_Graphics_Ungroup_Command;
extern Am_Object Am_Quit_No_Ask_Command; //quits immediately without asking
extern Am_Object Am_Cycle_Value_Command; //loops through strings and values

extern Am_Object Am_Open_Command;
extern Am_Object Am_Save_Command;
extern Am_Object Am_Save_As_Command;

//pops up the About Amulet dialog box
extern Am_Object Am_About_Amulet_Command; //put this in a menu

//for use in dialog boxes
extern Am_Object Am_Standard_OK_Command;
extern Am_Object Am_Standard_Cancel_Command;


/* **** Not implemented yet ******
extern Am_Object Am_Repeat_Last_Command;
extern Am_Object Am_Graphics_Refresh_Command;
extern Am_Object Am_Pop_Up_Dialog_Box_Command;
extern Am_Object Am_Font_Dialog_Box_Command;
extern Am_Object Am_Color_Dialog_Box_Command;
extern Am_Object Am_Print_Command;
********** */

//put into Am_ACTIVE slot a widget or command that has a
//Am_SELECTION_WIDGET slot holding the widget.  This is used by many
//of the above editing commands.
extern Am_Formula Am_Active_If_Selection;

// default for the Am_SELECT_CLOSEST_POINT_METHOD of Selection Widgets
extern Am_Where_Method Am_Default_Closest_Select_Point;

//gets the selection widget out of the command object, or if not
//there, then out of the Am_SAVED_OLD_OWNER widget
extern Am_Object Am_GV_Selection_Widget_For_Command(Am_Object cmd);
extern Am_Object Am_Get_Selection_Widget_For_Command(Am_Object cmd);

//put into the Am_REGION_WHERE_TEST of a selection handles to select
//all the objects in the region. Default method is Am_Group_Parts_Inside
Am_Define_Method_Type(Am_In_Region_Method, Am_Value_List,
		      (Am_Object widget, Am_Object group,
		       Am_Inter_Location region))

extern Am_In_Region_Method Am_Group_Parts_Inside;

////////////////////////////////////////////////////////////////////////
/// Dialog box routines
////////////////////////////////////////////////////////////////////////

//Put this method in the Am_DESTROY_WINDOW_METHOD of pop-up windows so
//they will exit nicely (as if the user hit CANCEL)

extern Am_Object_Method Am_Default_Pop_Up_Window_Destroy_Method;

// dialog_widgets.cc
extern Am_Object Am_Text_Input_Dialog;
extern Am_Object Am_Choice_Dialog;
extern Am_Object Am_Alert_Dialog;

extern Am_Object Am_OK_Button; //put this in a dialog box for OK 
extern Am_Object Am_OK_Cancel_Buttons;//put this in a dialog box for OK/Cancel

#define Am_AT_CENTER_SCREEN -10000

// For all of these routines, the Am_Value_List is a list of char* or 
// Am_String objects which will be displayed in the dialog box above any
// text input or buttons in the box.  They create a new dialog box
// and then destroy it at the end of the routine.
extern void Am_Show_Alert_Dialog (Am_Value_List alert_texts, 
				  int x = Am_AT_CENTER_SCREEN,
				  int y = Am_AT_CENTER_SCREEN,
				  bool modal = true);
extern void Am_Show_Alert_Dialog (Am_String alert_text, 
				  int x = Am_AT_CENTER_SCREEN,
				  int y = Am_AT_CENTER_SCREEN,
				  bool modal = true);

extern Am_Value Am_Get_Input_From_Dialog (Am_Value_List prompt_texts, 
					  Am_String initial_value = "", 
					  int x = Am_AT_CENTER_SCREEN,
					  int y = Am_AT_CENTER_SCREEN, 
					  bool modal= true);

extern Am_Value Am_Get_Choice_From_Dialog (Am_Value_List prompt_texts, 
					   int x = Am_AT_CENTER_SCREEN,
					   int y = Am_AT_CENTER_SCREEN,
					   bool modal = true);

//  Given a user-made dialog box, this sets its stop method, brings it
//  up, and returns its value using Am_Pop_Up_Window_And_Wait.
//  This is useful for efficiency if a certain dialog box is used many
//  times and you don't need to destroy it after use.
//Returns the value of Any input widgets or true if hit OK, and
//returns false if the user hits cancel

extern Am_Value Am_Show_Dialog_And_Wait (Am_Object the_dialog,
					 bool modal = true);

extern Am_Object Am_File_Dialog;

// these are called automatically from the Am_Open_Command, Am_Save_Command and
// Am_Save_As_Command
extern Am_String Am_Show_File_Dialog_For_Open(Am_String initial_value="");
extern Am_String Am_Show_File_Dialog_For_Save(Am_String initial_value="");

//This might be called for a file types as the command line argument.
//The command object should be the standard open command object
//attached to the menu_bar (needed to get the default selection and
//the Am_HANDLE_OPEN_SAVE_METHOD).
extern void Am_Standard_Open_From_Filename(Am_Object command_obj,
					   Am_String file_name);

inline void Am_Pop_Up_Error_Window(const char *error_string) {
  Am_Beep();
  Am_Show_Alert_Dialog(error_string);
}

#define Am_POP_UP_ERROR_WINDOW(error_string)     \
{ char line[250];                                \
  OSTRSTREAM_CONSTR (oss,line, 250, ios::out);   \
  oss << error_string << ends;                   \
  OSTRSTREAM_COPY(oss,line,250);                 \
  Am_Pop_Up_Error_Window(line);                  \
}

//this returns the About Amulet window, which then might be popped up.
//Or else use the Am_About_Amulet_Command which does this automatically.
extern Am_Object Am_Get_About_Amulet_Window();


#endif
