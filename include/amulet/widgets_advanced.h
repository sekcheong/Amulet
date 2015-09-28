/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* Advanced and internal stuff for widgets
   
   Designed and implemented by Brad Myers
*/

#ifndef WIDGETS_ADVANCED_H
#define WIDGETS_ADVANCED_H

#include <am_inc.h>

#include WIDGETS__H 
#include GEM__H
#include TYPES__H
#include UNIV_MAP__H
#include WEB__H

#include VALUE_LIST__H

extern void Am_Widgets_Initialize ();

////////////////////////////////////////////////////////////////////////
//  Widget Look
////////////////////////////////////////////////////////////////////////

// get value of Am_WIDGET_LOOK from Am_Screen
extern Am_Formula Am_Default_Widget_Look;

////////////////////////////////////////////////////////////////////////
//  Color Utility Functions
////////////////////////////////////////////////////////////////////////

extern Am_Formula Am_Default_Motif_Fill_Style;
     
class Computed_Colors_Record_Data : public Am_Wrapper {
  Am_WRAPPER_DATA_DECL (Computed_Colors_Record)
public:
  // constructor
  Computed_Colors_Record_Data ()
  {
    Note_Reference ();
  }
  Computed_Colors_Record_Data (const Am_Style& foreground);
  Computed_Colors_Record_Data (Computed_Colors_Record_Data*);
  operator== (Computed_Colors_Record_Data&)
  { return false; }
  // destructor
  virtual ~Computed_Colors_Record_Data ();

  void Print (ostream& out) const;

  Am_Style key;
  Am_Style foreground_style;
  Am_Style background_style;
  Am_Style shadow_style;
  Am_Style highlight_style;
  Am_Style highlight2_style;
  bool light; //true if foreground color is light, so should use black text
};

ostream& operator<< (ostream& os, Computed_Colors_Record_Data& rec);

class Computed_Colors_Record {
public:
  // constructors
  Computed_Colors_Record ()
  {
    data = NULL;
  }
  Computed_Colors_Record (const Am_Style& foreground);
  Computed_Colors_Record (const Am_Value& in_value)
  {
    data = (Computed_Colors_Record_Data*)in_value.value.wrapper_value;
    if (data)
      data->Note_Reference ();
  }
  Computed_Colors_Record (Computed_Colors_Record_Data* in_data)
  {
    data = in_data;
  }

  // destructor
  ~Computed_Colors_Record ()
  {
    if (data)
      data->Release ();
    data = NULL;
  }

  operator Am_Wrapper* () const
  {
    if (data)
      data->Note_Reference ();
    return data;
  }

  Computed_Colors_Record_Data* data;
};

//////// define the hash table from a style to a Motif_Colors_Record //////

inline int HashValue (Am_Wrapper* key, int size)
{
  return HashValue ((const void*)key, size);
}

inline int KeyComp (Am_Wrapper* key1, Am_Wrapper* key2)
{
  return KeyComp ((const void*)key1, (const void*)key2);
}

Am_DECL_MAP (Style2MotifRec, Am_Wrapper*, Computed_Colors_Record_Data*)

extern Am_Object Am_Button_In_Panel; // button that is part of a panel

extern Am_Style Am_Motif_Inactive_Stipple; // text draw style when inactive
extern Am_Style Am_Motif_White_Inactive_Stipple; // same, but for white
extern Am_Style Am_Key_Border_Line;

extern Am_Object Am_Pop_Up_Menu_From_Widget_Proto;

////////////////// Interface between widgets.cc and the various widget files

extern Am_Formula Am_Get_Computed_Colors_Record_Form;

extern void Am_Draw_Motif_Box (int left, int top, int width, int height,
			       bool depressed,
			       const Computed_Colors_Record& rec,
			       Am_Drawonable* draw);

struct am_rect
{
  am_rect( int l=0, int t=0, int w=0, int h=0 )
    { left = l; top = t, width = w, height = h; }
  int left, top, width, height;
};
typedef struct am_rect am_rect;

extern void Inset_Rect( am_rect& r, int inset );
extern void Am_Draw_Rect_Border( am_rect r,
                                 Am_Style upper_left,
                                 Am_Style lower_right,
                                 Am_Drawonable* draw );
extern void draw_down_arrow( int left, int top, int width, int height,
                             Am_Widget_Look look, bool depressed, bool active,
                             bool option, const Computed_Colors_Record& color_rec,
                             Am_Drawonable* draw );

extern Am_Formula Am_Active_From_Command;
extern Am_Formula Am_Active_And_Active2;
extern Am_Formula Am_Get_Owners_Command;
extern Am_Formula Am_Get_Real_String_Or_Obj;
extern Am_Formula Am_Get_Button_Command_Value;
extern Am_Formula Am_Font_From_Owner;

extern void Am_Widget_General_Undo_Redo(Am_Object command_obj,
					bool undo, bool selective);
extern Am_Object_Method Am_Widget_Inter_Command_Undo;
extern Am_Object_Method Am_Widget_Inter_Command_Selective_Undo;
extern Am_Object_Method Am_Widget_Inter_Command_Selective_Repeat;

//buttons
enum Am_Button_Type { Am_PUSH_BUTTON, Am_RADIO_BUTTON, Am_CHECK_BUTTON,
		      Am_MENU_BUTTON };

extern void Am_Button_Widgets_Initialize ();

//scroll bars

enum Am_Scroll_Arrow_Direction { Am_SCROLL_ARROW_UP, Am_SCROLL_ARROW_DOWN, 
				 Am_SCROLL_ARROW_LEFT, Am_SCROLL_ARROW_RIGHT};

//used to make the individual scroll-indicator drags not queued for undo
#define Am_MARKER_FOR_SCROLL_INC -2

extern void Am_Scroll_Widgets_Initialize ();

//text input

  ///////////////////////////////////////////////////////////////////////////
  // Scrolling Text Input Widget
  ///////////////////////////////////////////////////////////////////////////

extern void Am_Text_Widgets_Initialize ();
extern Am_Text_Check_Legal_Method Am_Number_Input_Filter_Method;

  ///////////////////////////////////////////////////////////////////////////
  // Selection Handles
  ///////////////////////////////////////////////////////////////////////////

extern Am_Object Am_One_Selection_Handle;
extern void Am_Selection_Widget_Initialize ();

  ///////////////////////////////////////////////////////////////////////////
  // Graphical Editing Commands
  ///////////////////////////////////////////////////////////////////////////
extern void Am_Editing_Commands_Initialize ();

  // useful utilities for editing commands
extern Am_Value_List Am_Copy_Object_List (Am_Value_List orig,
           const Am_Object& ref_obj = Am_No_Object, int offset = 0);
					
extern Am_Value_List Am_Sort_Obs_In_Group(Am_Value_List unsorted_sel_objs,
					  Am_Object group);
extern void Am_Get_Selection_In_Display_Order(Am_Object selection_widget,
					      Am_Value_List &selected_objs,
					      Am_Object &group);
extern Am_Object Am_Find_Part_Place(Am_Object obj, Am_Object group);

///////////////////////////////////////////////////////////////////////////
// Accelerators for buttons
///////////////////////////////////////////////////////////////////////////

extern Am_Object Am_Accelerator_Inter;

//returns Am_No_Object if OK to set an accelerator with that character.
//Returns a command if that character is already in use by that command in
//that window.  You might use the command to print out an error message.
extern Am_Object Am_Check_Accelerator_Char_For_Window(Am_Input_Char accel,
						    Am_Object window);
extern void Am_Add_Accelerator_Command_To_Window(Am_Object command,
						 Am_Object window);
extern void Am_Remove_Accelerator_Command_From_Window(Am_Object command,
						      Am_Object window);

///////////////////////////////////////////////////////////////////////////
// Methods for start, stop and abort widget
///////////////////////////////////////////////////////////////////////////

Am_Define_Method_Type(Am_Explicit_Widget_Run_Method, void,
		      (Am_Object widget, Am_Value new_value))

extern Am_Explicit_Widget_Run_Method Am_Standard_Widget_Start_Method;
extern Am_Object_Method 	     Am_Standard_Widget_Abort_Method;
extern Am_Explicit_Widget_Run_Method Am_Standard_Widget_Stop_Method;

///////////////////////////////////////////////////////////////////////////
// Dialog boxes
///////////////////////////////////////////////////////////////////////////

extern Am_Object am_empty_dialog;
extern void Am_Dialog_Widgets_Initialize ();

#endif
