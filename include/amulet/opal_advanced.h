/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifndef OPAL_ADVANCED_H
#define OPAL_ADVANCED_H

#include <am_inc.h>

#include OBJECT_ADVANCED__H  // for Am_Demon_Queue
#include OBJECT__H
#include GEM__H
#include STANDARD_SLOTS__H

extern Am_Object Am_Aggregate;  // Usually you want to create instances of
                                // Am_Group or Am_Map
enum {
  // Opal internal slot keys
  Am_DRAW_METHOD = Am_FIRST_OPAL_INTERNAL_SLOT,
  Am_MASK_METHOD,
  Am_DRAWONABLE,                   
  Am_DRAW_BUFFER,
  Am_MASK_BUFFER,
  Am_SCREEN,                       
  Am_TODO,                         
  Am_INVALID_METHOD,               
  Am_PREV_STATE,                   
  Am_POINT_IN_OBJ_METHOD,          
  Am_POINT_IN_PART_METHOD,         
  Am_POINT_IN_LEAF_METHOD,         
  Am_TRANSLATE_COORDINATES_METHOD, 
  Am_INIT_WANT_ENTER_LEAVE,         // want enter-leave events
  Am_INIT_WANT_MULTI_WINDOW,        // want multi-window events
  Am_OLD_WIDTH,		           // for resize_groups
  Am_OLD_HEIGHT,                   // for resize_groups
  Am_WAITING_FOR_COMPLETION,	   //pop_up windows
  Am_COMPLETION_VALUE,
  Am_FADE_DEPTH,       // Num fade groups from this obj up to window
  Am_OBJECT_IN_PROGRESS //for checking whether crashed last time
};
//counting down, don't go below biggest in opal section of standard_slots.h

// update all windows- used by event loop
extern void Am_Update_All ();

// update a single window
extern void Am_Update (Am_Object window);

// Creation and use of Draw Methods:
//  - Store method in Am_DRAW_METHOD slot.
//  - Method gets called by owner object when it gets drawn.
//  - Self points to own data, drawonable is the window being drawn in,
//    x and y offsets are for converting the object's position to window
//    coordinates.
//  - Do not call Set or any other demon invoking operation in the method.
Am_Define_Method_Type(Am_Draw_Method, void,
		      (Am_Object self, Am_Drawonable* drawonable,
		       int x_offset, int y_offset))

extern void Am_Draw (Am_Object object, Am_Drawonable* drawonable,
		     int x_offset, int y_offset);

// Creation and use of Invalidation Methods:
//  - The method is stored in the Am_INVALID_METHOD slot
//  - Method is called up from parts to their corresponding owner
//  - Parameters are the self object, the part which has changed,
//    and the rectangular region to invalidate
//  - The invalid region is converted to the coordinates of the object's owner
//    and passed to the owner of the next object.
//  - Do not call Set or any other demon invoking operation in the method.
Am_Define_Method_Type(Am_Invalid_Method, void,
		      (Am_Object self, Am_Object which_part, int left,
		       int top, int width, int height))

extern void Am_Invalidate (Am_Object owner, Am_Object which_part,
			   int left, int top, int width, int height);

// Point in Object Methods:
//  - Stored in slot Am_POINT_IN_OBJ_METHOD
//  - Parameters are self and a point x,y.
//  - The point is in the coordinate system of the ref_obj
//  - Returns the object if the point lies inside the object, false otherwise.
//  - Do not call Set or any other demon invoking operation in the method.
Am_Define_Method_Type(Am_Point_In_Method, Am_Object,
		      (const Am_Object& in_obj, int x, int y,
		       const Am_Object& ref_obj))

// Point in Part/Leaf Methods:
//  - Parameters are self and a point x,y.
//  - The point is in the coordinate system of the ref_obj
//  - if inside in_obj but NOT in part or leaf, then if want_self
//  		returns in_obj otherwise returns NULL
//  - If want_groups is true, the finds the leaf-most element even if
//        it is a group.  If want_groups is false, then will not return a
//        group (if x,y is not over a "primitive" object, returns Am_No_Object)
//  - Returns the object if the point lies inside the object, false otherwise.
//  - Do not call Set or any other demon invoking operation in the method.
Am_Define_Method_Type(Am_Point_In_Or_Self_Method, Am_Object,
		      (const Am_Object& in_obj, int x, int y,
		       const Am_Object& ref_obj,
		       bool want_self, bool want_groups))

// Am_Translate_Coordinates_Method:
//  - Stored in slot Am_TRANSLATE_COORDINATES_METHOD
//  - Given a point in the coordinate system of obj, converts it to
//    be in the coordinate system of its owner.  for_part is the
//    (optional) part of obj that is being translated for
//  - For almost all objects, it is x+obj.Get(Am_LEFT), y+obj.Get(Am_TOP)
//  - For scrolling groups, it is more complicated.  For windows,
//    uses the gem function.
Am_Define_Method_Type(Am_Translate_Coordinates_Method, void,
		      (const Am_Object& obj, const Am_Object& for_part,
		       int in_x, int in_y, int& out_x, int& out_y))

class Am_State_Store {
 public:
  Am_State_Store (Am_Object self, Am_Object owner, bool visible, int left,
                  int top, int width, int height);
#ifdef AMULET2_INSTRUMENT
#undef Add
#endif
  void Add (bool needs_update);
#ifdef AMULET2_INSTRUMENT
  Am_Instrumentation(Am_State_Store)
#define Add Am_Instrumented(Add)
#endif
  void Remove ();
  static Am_State_Store* Narrow (Am_Ptr ptr)
    { return (Am_State_Store*)ptr; }

  bool Visible (Am_Drawonable* drawonable, int x_offset, int y_offset);

  static void Invoke ();
  static void Shutdown ()
  { shutdown = true; }

 private:
  Am_Object owner;
  bool visible;
  int left;
  int top;
  int width;
  int height;

  bool in_list;
  bool needs_update;
  Am_Object self;
  Am_State_Store* next;

  void Invalidate ();

  static Am_State_Store* invalidation_list;
  static bool shutdown;
};

// Used in Am_Map.  When a component is created, the map calls this method
// to allow instance specific to be performed on the item.
// Stored in slot Am_ITEM_METHOD.
Am_Define_Method_Type(Am_Item_Method, Am_Object,
		      (int rank, Am_Value& value, Am_Object item_instance))

// These are the standard prototype function for setting up each individual
// item in a map.  Stores the value in the slot Am_ITEM.
extern Am_Item_Method Am_Standard_Item_Method;

// Useful for scroll groups (in widgets)
extern void Am_Invalid_Rectangle_Intersect
	(int left, int top, int width, int height,
	 int my_left, int my_top, int my_width, int my_height,
	 int& final_left, int& final_top, int& final_width, int& final_height);

#define Am_WINTODO_CREATE     0x00000001
#define Am_WINTODO_DESTROY    0x00000002
#define Am_WINTODO_EXPOSE     0x00000004
#define Am_WINTODO_REPARENT   0x00000008
#define Am_WINTODO_VISIBLE    0x00000010
#define Am_WINTODO_POSITION   0x00000020
#define Am_WINTODO_SIZE       0x00000040
#define Am_WINTODO_TITLE      0x00000080
#define Am_WINTODO_ICON_TITLE 0x00000100
#define Am_WINTODO_TITLE_BAR  0x00000200
#define Am_WINTODO_FILL_STYLE 0x00000400
#define Am_WINTODO_MIN_SIZE   0x00000800
#define Am_WINTODO_MAX_SIZE   0x00001000
#define Am_WINTODO_ICONIFY    0x00002000
#define Am_WINTODO_SAVE_UNDER 0x00004000
#define Am_WINTODO_CURSOR     0x00008000
#define Am_WINTODO_CLIP             0x00010000

class Am_Window_ToDo {
 public:
  Am_Object window;
  unsigned long flags;
  int left, top;     // Invalid rectangle
  int width, height;
  Am_Window_ToDo* prev;
  Am_Window_ToDo* next;
  static Am_Window_ToDo* Narrow (Am_Ptr input)
  { return (Am_Window_ToDo*)input; }
  void Merge_Rectangle (int left, int top, int width, int height);
#ifdef AMULET2_INSTRUMENT
#undef Add
#endif
  void Add ();
#ifdef AMULET2_INSTRUMENT
  Am_Instrumentation(Am_Window_ToDo)
#define Add Am_Instrumented(Add)
#endif
  void Remove ();
};

#define Am_EAGER_DEMON       0x0001
#define Am_STATIONARY_REDRAW 0x0004
#define Am_MOVING_REDRAW     0x0008
#define Am_GROUP_RESIZE_PARTS 0x0010
#define Am_COMMON_SLOT       0x0004
#define Am_UNCOMMON_SLOT     0x0008

// Declarations and functions needed to coordinate opal.cc and windows.cc
extern Am_Demon_Queue Main_Demon_Queue;
extern void am_generic_renew_components (Am_Object object);
extern void am_generic_renew_copied_comp (Am_Object object);
extern void am_generic_add_part (Am_Object owner, Am_Object old_object,
			      Am_Object new_object);
extern void am_translate_coord_to_me(Am_Object in_obj, Am_Object ref_obj,
				     int &x, int &y);
extern Am_Point_In_Or_Self_Method am_group_point_in_part;
extern Am_Point_In_Or_Self_Method am_group_point_in_leaf;

extern bool am_is_group_and_not_pretending(Am_Object in_obj);

extern void Am_Initialize_Aux();

extern Am_Drawonable* GV_a_drawonable (Am_Object obj);

extern Am_Drawonable* Get_a_drawonable (Am_Object obj);

// Timing event stuff

// registers a periodic timer event.  Each time the event times out, opal
// will call the method stored in slot method_slot of object obj.
// if once, the event is only called once.  Otherwise it's called repeatedly
// until the timer event is removed with Am_Stop_Timer.
void Am_Register_Timer(Am_Time wait_time, Am_Object obj,
		       Am_Slot_Key method_slot, bool once);
// All timer events in Opal's list with a matching object and slot are 
// removed from the timer list.
void Am_Stop_Timer(Am_Object obj, Am_Slot_Key slot);

//resets the start time associated with this obj and slot to be NOW.
void Am_Reset_Timer_Start(Am_Object obj, Am_Slot_Key slot); 

extern enum Am_Timer_State {
  Am_TIMERS_RUNNING,
  Am_TIMERS_SUSPENDED,
  Am_TIMERS_SINGLE_STEP
} Am_Global_Timer_State;

void Am_Set_Timer_State (Am_Timer_State new_state);

// // // // // // // // // // // // // // // // // // // // // //
//  Things needed to have external controlled drawables
// // // // // // // // // // // // // // // // // // // // // //

//the standard event handler functions for opal and interactors
extern Am_Input_Event_Handlers *Am_Global_Opal_Handlers;

// set this into the Am_DOUBLE_BUFFERED slot to disable clearing of
// the area before redrawing
#define Am_WIN_DOUBLE_BUFFER_EXTERNAL 2

#endif
