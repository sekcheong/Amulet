/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the selection_widget
   
   Designed and implemented by Brad Myers
*/

#include <am_inc.h>

#include AM_IO__H

#include WIDGETS_ADVANCED__H
#include STANDARD_SLOTS__H
#include VALUE_LIST__H
#include OPAL_ADVANCED__H  // for Am_DRAWONABLE, Am_Window_Coordinate
#include INTER_ADVANCED__H //needed for Am_Inter_Tracing

#include WIDGETS__H
#include GEM__H
#include INTER__H
#include OPAL__H
#include INTER__H
#include REGISTRY__H
#include ANIM__H // for Am_NO_ANIMATION

#define MOVE_THRESHHOLD 6 //number of pixels before allowed to move an object

inline int IABS (int a) { return (a < 0)? -a : a; }
inline int IMIN (int a, int b) { return (a < b)? a : b; }
inline int IMAX (int a, int b) { return (a > b)? a : b; }

///////////////////////////////////////////////////////////////////////////
// Handles
///////////////////////////////////////////////////////////////////////////

#define HANDLE_SIZE 9  //should be odd
#define HANDLE_SIZE_D2 4  // HANDLE_SIZE / 2

void draw_handle (int left, int top, bool filled, Am_Drawonable* draw,
		  Am_Style style) {
  if (filled) {
    draw->Draw_Rectangle (Am_No_Style, style, left, top,
			  HANDLE_SIZE, HANDLE_SIZE);
    //outline in white
    draw->Draw_Rectangle (Am_White, Am_No_Style, left, top,
			  HANDLE_SIZE, HANDLE_SIZE);
  }
  else draw->Draw_Rectangle (style, Am_No_Style, left, top,
			  HANDLE_SIZE, HANDLE_SIZE);
}

void draw_8_handles_for_rect(int left, int top, int width, int height,
		     bool filled, Am_Drawonable* draw,
		     Am_Style style,
		     Am_Style special_style = Am_No_Style,
		     Am_Move_Grow_Where_Attach special_style_where = Am_ATTACH_WHERE_HIT // 0
		     ) {
  int width_d2 = width / 2;
  int height_d2 = height / 2;
  
  
  // left-top
  draw_handle(left, top, filled, draw, 
	      (special_style_where == Am_ATTACH_NW)? special_style: style);
  //left-middle
  draw_handle(left, top + height_d2 - HANDLE_SIZE_D2, filled, draw, 
	      (special_style_where == Am_ATTACH_W)? special_style: style);
  //left-bottom
  draw_handle(left, top+height-HANDLE_SIZE, filled, draw, 
	      (special_style_where == Am_ATTACH_SW)? special_style: style);

  // middle-top
  draw_handle(left + width_d2 - HANDLE_SIZE_D2, top, filled, draw, 
	      (special_style_where == Am_ATTACH_N)? special_style: style);
  //middle-bottom
  draw_handle(left + width_d2 - HANDLE_SIZE_D2, top+height-HANDLE_SIZE,
	      filled, draw, 
	      (special_style_where == Am_ATTACH_S)? special_style: style);

  // right-top
  draw_handle(left + width - HANDLE_SIZE, top, filled, draw, 
	      (special_style_where == Am_ATTACH_NE)? special_style: style);

  // right-middle
  draw_handle(left + width - HANDLE_SIZE, top + height_d2 - HANDLE_SIZE_D2,
	      filled, draw, 
	      (special_style_where == Am_ATTACH_E)? special_style: style);
  // right-bottom
  draw_handle(left + width - HANDLE_SIZE, top + height - HANDLE_SIZE,
	      filled, draw, 
	      (special_style_where == Am_ATTACH_SE)? special_style: style);

}
	 
void draw_2_handles_for_line(int x1, int y1, int x2, int y2,
		     bool filled, Am_Drawonable* draw,
		     Am_Style style,
		     Am_Style special_style = Am_No_Style,
		     Am_Move_Grow_Where_Attach special_style_where = Am_ATTACH_WHERE_HIT // 0
		     ) {
  draw_handle(x1-HANDLE_SIZE_D2, y1-HANDLE_SIZE_D2, filled, draw, 
	      (special_style_where == Am_ATTACH_END_1)? special_style: style);

  draw_handle(x2-HANDLE_SIZE_D2, y2-HANDLE_SIZE_D2, filled, draw, 
	      (special_style_where == Am_ATTACH_END_2)? special_style: style);
}

//Set into the top-level widget to validate all objects in the values list
Am_Define_Formula(int, remove_from_value_if_invalid) {
  self.Make_Unique(Am_VALUE);
  Am_Value_List values;
  values = self.Get(Am_VALUE);
  Am_Object obj, o;
  Am_Value v;
  bool changed = false;
  for(values.Start(); !values.Last(); values.Next()) {
    obj = values.Get();
    if (!Am_Object_And_Owners_Valid_And_Visible( obj)) {
      values.Delete(false);
      changed = true;
    }
    else {
      //set up dependencies so this formula will be re-evaluated if
      //the object is modified
      v = obj.Peek(Am_LEFT);
      v = obj.Peek(Am_VISIBLE);
      o = obj.Get_Owner();
    }
  }
  if (changed) {
    self.Note_Changed(Am_VALUE);
  }
  return 0;
}

// Set selection handles' left and top. 
// Put it in the Am_LEFT slot.
Am_Define_Formula (int, selections_handles_lt) {
  Am_Object for_obj, sel_group;
  int left, top;
  for_obj = self.Get(Am_ITEM);
  sel_group = self.Get_Owner(); 
  bool valid;
  if (Am_Object_And_Owners_Valid_And_Visible( for_obj)) {
    left = - HANDLE_SIZE_D2;
    top = - HANDLE_SIZE_D2;
    if (sel_group.Valid())
      valid=Am_Translate_Coordinates(for_obj, left, top,
				     sel_group, left, top);
    /* ****
    if (for_obj.Is_Instance_Of (Am_Line)) {
      int x1, x2, obj_left, y1, y2, obj_top;
      x1 = for_obj.GV (Am_X1);
      x2 = for_obj.GV (Am_X2);
      obj_left = for_obj.Get (Am_LEFT);
      if (x1 < x2)
        left += x1 - obj_left;
      else
        left += x2 - obj_left;
      y1 = for_obj.GV (Am_Y1);
      y2 = for_obj.GV (Am_Y2);
      obj_top = for_obj.Get (Am_TOP);
      if (y1 < y2)
        top += y1 - obj_top;
      else
        top += y2 - obj_top;
    }
    // cout << "<>For " << self << " calculating l,t (" << left << ","
    //	  << top << ") for_obj "
    //	  << for_obj << " owner " << owner
    //	  << " valid " << valid << endl << flush;
    *****  */
  }
  else {
    left = 0; top = 0; 
  }
  self.Set(Am_TOP, top);
  return(left);
}

// Set selection handles width, height.
// Put this in the Am_Width slot.
Am_Define_Formula (int, selections_handles_wh) {
  Am_Object for_obj, sel_group;
  int width, height;
  for_obj = self.Get(Am_ITEM);
  if (Am_Object_And_Owners_Valid_And_Visible( for_obj)) {
    /* ****
    if (for_obj.Is_Instance_Of (Am_Line)) {
      int x1, x2, y1, y2;
      x1 = for_obj.GV (Am_X1);
      x2 = for_obj.GV (Am_X2);
      if (x1 < x2)
        width = x2 - x1;
      else
        width = x1 - x2;
      y1 = for_obj.GV (Am_Y1);
      y2 = for_obj.GV (Am_Y2);
      if (y1 < y2)
        height = y2 - y1;
      else
        height = y1 - y2;
    }
    else    {
    **** */
    width = (int)for_obj.Get(Am_WIDTH);
    height = (int)for_obj.Get(Am_HEIGHT);
      /* } */
    width += HANDLE_SIZE - 1;
    height += HANDLE_SIZE - 1;
    // cout << "<>For " << self << " calculating w,h (" 
    // << width << "," << height << ") for_obj "
    //	  << for_obj << " owner " << owner
    //	  << " valid " << valid << endl << flush;
  }
  else {
    width = 0; height = 0;
  }
  self.Set(Am_HEIGHT, height);  
  return(width);
}

Am_Define_Method(Am_Draw_Method, void, selection_handles_draw,
		 (Am_Object self, Am_Drawonable* draw,
		  int x_offset, int y_offset)) {
  Am_Object for_obj;
  for_obj = self.Get(Am_ITEM);

  if (for_obj.Valid()) { //otherwise don't draw anything	   
    Am_Value value;
    value=for_obj.Peek(Am_AS_LINE);
    bool as_line = value.Valid();

    Am_Style style;
    style = self.Get(Am_FILL_STYLE);
    Am_Object sel_handles_widget = self.Get_Owner().Get_Owner();
    if (!style.Valid()) {
      //get from owner's owner so don't need a constraint
      //owner must be valid if I am being drawn
      style = sel_handles_widget.Get(Am_FILL_STYLE);
    }
    Am_Style special_style = Am_No_Style;
    Am_Move_Grow_Where_Attach special_style_where = Am_ATTACH_WHERE_HIT; //0
    value=sel_handles_widget.Peek(Am_SELECT_CLOSEST_POINT_STYLE);
    if (value.Valid()) {
      special_style = value;
      value=sel_handles_widget.Peek(Am_SELECT_CLOSEST_POINT_OBJ);
      if (value.Valid() && Am_Object(value) == for_obj) {
        special_style_where = (Am_Move_Grow_Where_Attach)
          sel_handles_widget.Get(Am_SELECT_CLOSEST_POINT_WHERE);
      }
    }
    //cout << "--Drawing " << self << " for obj " << for_obj << " as line "
    //	 << as_line << " l,t,w,h (" << left << ","
    //	 << top << "," << width << "," << height << ")\n" << flush;
    if (as_line) {
      int x1 = for_obj.Get(Am_X1);
      int y1 = for_obj.Get(Am_Y1);
      int x2 = for_obj.Get(Am_X2);
      int y2 = for_obj.Get(Am_Y2);
      int offx, offy;
      Am_Object win = self.Get(Am_WINDOW);
      Am_Translate_Coordinates(for_obj.Get_Owner(), 0, 0, win, offx, offy);
      draw_2_handles_for_line(x1 + offx,
			      y1 + offy,
			      x2 + offx,
			      y2 + offy, true, draw, style,
			      special_style, special_style_where);
    }
    else {
      int left = (int)self.Get (Am_LEFT) + x_offset;
      int top = (int)self.Get (Am_TOP) + y_offset;
      int width = self.Get (Am_WIDTH);
      int height = self.Get (Am_HEIGHT);		 
      draw_8_handles_for_rect(left, top, width, height, true, draw, style,
			      special_style, special_style_where);
    }
  }
}

///////////////////////////////////////////////////////////////////////////
// For top-level widget
///////////////////////////////////////////////////////////////////////////

Am_Define_Formula (int, width_from_owner_or_zero) {
  Am_Object owner = self.Get_Owner();
  if (owner.Valid()) {
    if (owner.Is_Instance_Of(Am_Scrolling_Group))
      return owner.Get(Am_INNER_WIDTH);
    else return owner.Get(Am_WIDTH);
  }
  else return 0;
}
Am_Define_Formula (int, height_from_owner_or_zero) {
  Am_Object owner = self.Get_Owner();
  if (owner.Valid()) {
    if (owner.Is_Instance_Of(Am_Scrolling_Group))
      return owner.Get(Am_INNER_HEIGHT);
    else return owner.Get(Am_HEIGHT);
  }
  else return 0;
}

Am_Define_Formula (int, set_command_and_move_old_owner) {
  Am_Object cmd;
  cmd = self.Get(Am_COMMAND);
  if (cmd.Valid()) 
    cmd.Set(Am_SAVED_OLD_OWNER, self, Am_OK_IF_NOT_THERE);
  cmd = self.Get(Am_MOVE_GROW_COMMAND);
  if (cmd.Valid()) 
    cmd.Set(Am_SAVED_OLD_OWNER, self, Am_OK_IF_NOT_THERE);
  return 1;
}

///////////////////////////////////////////////////////////////////////////
// For become selected
///////////////////////////////////////////////////////////////////////////

void set_commands_for_sel(Am_Object inter, Am_Object widget,
			  Am_Object clicked_obj,
			  Am_Value new_value, Am_Value old_value) {
  Am_Object inter_command, widget_command;
  inter_command = inter.Get_Object(Am_COMMAND);
  if (inter_command.Valid()) {
    inter_command.Set(Am_OLD_VALUE, old_value);
    inter_command.Set(Am_VALUE, new_value);
    inter_command.Set(Am_OBJECT_MODIFIED, clicked_obj);
  }
  widget_command = widget.Get_Object(Am_COMMAND);
  if (widget_command.Valid()) {
    widget_command.Set(Am_OLD_VALUE, old_value);
    widget_command.Set(Am_VALUE, new_value);
    widget_command.Set(Am_OBJECT_MODIFIED, clicked_obj);
  }
}
  
//widget just used for debugging output
void toggle_object_in_list(const Am_Object &widget, 
				  Am_Object new_object,
				  Am_Value_List &list) {
  list.Start();
  if(list.Member(new_object)) {
    Am_INTER_TRACE_PRINT(widget, "Selection handle removing " << new_object
			 << " from " << widget);
    list.Delete();
  }
  else { // not a member, add it
    Am_INTER_TRACE_PRINT(widget, "Selection handle adding " << new_object <<
			 " to " << widget);
    // Not allowed to have objects selected that are parts of
    // new_object, owners of new_object, or have owners different from
    // new_object.  This can happen when you have select into groups.
    for (list.Start (); !list.Last (); list.Next ()) {
      Am_Object item = list.Get ();
      if (item.Is_Part_Of (new_object) ||
	  new_object.Is_Part_Of (item) ||
	  item.Get_Owner () != new_object.Get_Owner ())
	list.Delete ();
    }
    list.Add(new_object);
  }
}

void clear_multi_selections(const Am_Object &widget) {
  Am_Value value;
  value=widget.Peek(Am_MULTI_SELECTIONS);
  if (value.Valid () && Am_Value_List::Test (value)) {
    Am_Value_List widget_list (value);
    for (widget_list.Start (); !widget_list.Last (); widget_list.Next ()) {
      Am_Object curr_widget = widget_list.Get ();
      if (curr_widget != widget)
	curr_widget.Set (Am_VALUE, Am_No_Value_List);
      // Clear selection of joined widgets.  NDY: Need to consider undo.
    }
  }
}
  
  

//called to select the object.
//object to be operated on is in interim
//depending on state of shift, add to select set or make this be the only
//object.
// **** Should use destructive modification for lists ***
Am_Define_Method(Am_Mouse_Event_Method, void, sel_object,
		 (Am_Object inter, int mouse_x, int mouse_y,
		  Am_Object ref_obj, Am_Input_Char ic)) {
  Am_Object new_object;
  if (inter.Valid()) {
    Am_Object widget = inter.Get_Owner();
    clear_multi_selections(widget);
    new_object = inter.Get(Am_START_OBJECT);
    if(new_object.Valid ()) {
      Am_Value new_value;
      bool toggle_in_set = ic.shift;
      if (new_object == widget) { // then clicked in the background
        if (toggle_in_set) {   //  don't do anything
          Am_Abort_Interactor(inter); //make sure not queued for undo
          return;
        }
        else {  // select nothing
          Am_INTER_TRACE_PRINT(widget, "Selection handle setting empty for "
			       << widget);
          new_object = NULL; //so object_modified will be null
          new_value = Am_Value_List();
        }
      }
      else { // over a specific object
	Am_Value value;
	value=widget.Peek(Am_SELECT_CLOSEST_POINT_STYLE);
	bool abort_ok = true;
	if (value.Valid()) {
	  Am_Where_Method method = widget.Get(Am_SELECT_CLOSEST_POINT_METHOD);
	  Am_Object diff_obj =
	    method.Call(widget, new_object, ref_obj, mouse_x, mouse_y);
	  if (diff_obj.Valid()) abort_ok = false;
	}
        Am_Value_List list;
        list = widget.Get(Am_VALUE);
        list.Start();
	if (toggle_in_set) 
	  toggle_object_in_list(widget, new_object, list);
	else { //if object is selected, do nothing, otherwise,
	  // make new_object be the only selection
	  if(list.Member(new_object)) { //nothing to do if already selected
	    if (abort_ok) {
	      Am_Abort_Interactor(inter); //make sure not queued for undo
	      return;
	    }
	  }
	  else {
	    Am_INTER_TRACE_PRINT(widget, "Selection handle setting " << widget
				 << " to contain only " << new_object);
	    list.Make_Empty();
	    list.Add(new_object);
	  }
	}
        new_value = list;
      }
      Am_Value old_value;
      old_value=widget.Peek(Am_VALUE);
      widget.Set(Am_VALUE, new_value);
      set_commands_for_sel(inter, widget, new_object, new_value, old_value);
    }
  }
}

// checks which point on the object x,y is closest to, and sets
// appropriate slots of widget.  Returns a valid object when the selection is
// different from the old value.
Am_Define_Method(Am_Where_Method, Am_Object, Am_Default_Closest_Select_Point,
		 (Am_Object widget, Am_Object object,
		  Am_Object ref_obj, int x, int y)) {
  Am_Move_Grow_Where_Attach which_point = Am_ATTACH_WHERE_HIT; //0
  Am_Value value;
  value=object.Peek(Am_AS_LINE);
  bool as_line = value.Valid();
  Am_Object owner = object.Get_Owner();
  Am_Translate_Coordinates(ref_obj, x, y, owner, x, y);
  if (as_line) {
    int x1 = object.Get(Am_X1);
    int y1 = object.Get(Am_Y1);
    int x2 = object.Get(Am_X2);
    int y2 = object.Get(Am_Y2);
    long d1 = (long(x1 - x)*long(x1 - x)) +
        (long(y1 - y)*long(y1 - y));
    long d2 = (long(x2 - x)*long(x2 - x)) +
        (long(y2 - y)*long(y2 - y));
    if (d1 < d2) which_point = Am_ATTACH_END_1;
    else which_point = Am_ATTACH_END_2;
  }
  else {
    int width = object.Get(Am_WIDTH);
    int height = object.Get(Am_HEIGHT);
    int left = object.Get(Am_LEFT);
    int top = object.Get(Am_TOP);
    int w3 = width / 3;
    int h3 = height / 3;
    Am_Move_Grow_Where_Attach_vals xcontrol;
    //  first do x direction
    if (x > left + w3 + w3)  // right third
      xcontrol = Am_ATTACH_E_val;
    else if (x < left + w3) // left third
      xcontrol = Am_ATTACH_W_val;
    else  // center third
      xcontrol = Am_ATTACH_CENTER_val;
    // now do y, and compute real attach
    if (y > top + h3 + h3) { //bottom third
      switch (xcontrol) {
      case Am_ATTACH_W_val: which_point = Am_ATTACH_SW; break;
      case Am_ATTACH_E_val: which_point = Am_ATTACH_SE; break;
      case Am_ATTACH_CENTER_val: which_point = Am_ATTACH_S; break;
      default: Am_ERRORO("Bad Am_WHERE_ATTACH "
			 << Am_Move_Grow_Where_Attach (xcontrol)
			 << " for selection widget " << widget,
			 widget, Am_WHERE_ATTACH);
        break;
      }
    }
    else if (y < top + h3) { // top third
      switch (xcontrol) {
      case Am_ATTACH_W_val: which_point = Am_ATTACH_NW; break;
      case Am_ATTACH_E_val: which_point = Am_ATTACH_NE; break;
      case Am_ATTACH_CENTER_val: which_point = Am_ATTACH_N; break;
      default: Am_ERRORO("Bad Am_WHERE_ATTACH "
			 << Am_Move_Grow_Where_Attach (xcontrol)
			 << " for selection widget " << widget,
			 widget, Am_WHERE_ATTACH);
        break;
      }
    }
    else { // middle third
      switch (xcontrol) {
      case Am_ATTACH_W_val: which_point = Am_ATTACH_W; break;
      case Am_ATTACH_E_val: which_point = Am_ATTACH_E; break;
      case Am_ATTACH_CENTER_val: //hack, for center, use NW
	which_point = Am_ATTACH_NW; break;
      default: Am_ERRORO("Bad Am_WHERE_ATTACH "
			 << Am_Move_Grow_Where_Attach (xcontrol)
			 << " for selection widget " << widget,
			 widget, Am_WHERE_ATTACH);
        break;
      }
    }
  }
  Am_Value old_point, old_object;
  old_object=widget.Peek(Am_SELECT_CLOSEST_POINT_OBJ);
  old_point=widget.Peek(Am_SELECT_CLOSEST_POINT_WHERE);
  widget.Set(Am_SELECT_CLOSEST_POINT_OBJ, object);
  widget.Set(Am_SELECT_CLOSEST_POINT_WHERE, which_point);
  if (old_object != object || old_point != which_point)
    return object;
  else return Am_No_Object;
}

///////////////////////////////////////////////////////////////////////////
// For Moving
///////////////////////////////////////////////////////////////////////////

//goes into the move-grow interactor in the selection widget
Am_Define_Object_Formula (Am_Compute_MG_Feedback_Object) {
  Am_Object feedback;
  Am_Object owner = self.Get_Owner ();
  bool as_line = self.Get (Am_AS_LINE);
  feedback = owner.Get (as_line ? Am_LINE_FEEDBACK_OBJECT :
		        Am_RECT_FEEDBACK_OBJECT);
  if (feedback.Valid () && feedback.Get_Object (Am_OPERATES_ON, Am_NO_DEPENDENCY) != owner) {
    feedback = feedback.Create ()
      .Set(Am_VISIBLE, false);
    feedback.Set (Am_OPERATES_ON, owner);
    owner.Set (as_line ? Am_LINE_FEEDBACK_OBJECT : Am_RECT_FEEDBACK_OBJECT,
	       feedback);
    owner.Add_Part (feedback, false);
  }
  return feedback;
}

void fix_feedback_for_inter(Am_Object &inter) {
  Am_Object widget = inter.Get_Owner();
  Am_Object feedback = inter.Get(Am_FEEDBACK_OBJECT);
  Am_Object feedback_owner = feedback.Get_Owner();
  if (feedback_owner != widget) {
    feedback.Remove_From_Owner();
    widget.Add_Part(feedback);
  }
}

Am_Define_Method(Am_Current_Location_Method, void, grow_start_do,
		 (Am_Object inter, Am_Object object_modified,
		  Am_Inter_Location data)) {
  fix_feedback_for_inter(inter);
  //now call the prototype's method, which we have stored as the REDO method
  Am_Current_Location_Method method;
  method = Am_Move_Grow_Interactor.Get(Am_START_DO_METHOD);
  method.Call(inter, object_modified, data);
}

Am_Define_Method(Am_Object_Method, void, new_points_start_do,
		 (Am_Object inter)) {
  fix_feedback_for_inter(inter);
  //now call the prototype's method, which we have stored as the REDO method
  Am_Object_Method method;
  method = Am_New_Points_Interactor.Get(Am_START_DO_METHOD);
  method.Call(inter);
}

static Am_Input_Char get_owner_start_and_convert_string(Am_Object &widget) {
  Am_Value value;
  Am_Input_Char parent_char;
  value = widget.Peek(Am_START_WHEN);
  if (value.type == Am_STRING) {
    Am_String sval;
    sval = value;
    parent_char = Am_Input_Char((char*)sval);
  }
  else if (Am_Input_Char::Test (value))
    parent_char = value;
  else Am_Error("Am_START_WHEN slot has wrong type");
  return parent_char;
}

//get owner's start_when, and remove the "any" from the front.  Used as the
//start_when of the growing (so shift doesn't cause moving)
Am_Define_Formula (Am_Value, compute_mg_start_when) {
  Am_Object widget = self.Get_Owner();
  if (widget.Valid()) {
    Am_Input_Char parent_char = get_owner_start_and_convert_string(widget);
    parent_char.any_modifier = false;
    parent_char.shift = false;
    if (parent_char.button_down == Am_ANY_DOWN_UP)
      parent_char.button_down = Am_BUTTON_DOWN;
    if (parent_char.click_count == Am_ANY_CLICK)
      parent_char.click_count = Am_SINGLE_CLICK;
    // cout << "computed start when " << parent_char << " from widget " <<
    //  widget << endl << flush;
    return parent_char;
  }
  else return Am_Input_Char("LEFT_DOWN");
}

//get owner's start_when, and convert to a CLICK
Am_Define_Formula (Am_Value, compute_click_start_when) {
  Am_Object widget = self.Get_Owner();
  if (widget.Valid()) {
    Am_Input_Char parent_char = get_owner_start_and_convert_string(widget);
    parent_char.button_down = Am_BUTTON_CLICK;
    return parent_char;
  }
  else return Am_Input_Char("LEFT_CLICK");
}

//get owner's start_when, and convert to a DRAG
Am_Define_Formula (Am_Value, compute_drag_start_when) {
  Am_Object widget = self.Get_Owner();
  if (widget.Valid()) {
    Am_Input_Char parent_char = get_owner_start_and_convert_string(widget);
    parent_char.button_down = Am_BUTTON_DRAG;
    return parent_char;
  }
  else return Am_Input_Char("LEFT_DRAG");
}

void calculate_group_size(Am_Value_List list, int &min_left, int &min_top, 
			  int &max_right, int &max_bottom,
			  Am_Object &owner) {
  min_left = 29999;
  min_top  = 29999;
  max_right = -29999;
  max_bottom = -29999;
  int cur_left, cur_top;
  Am_Object obj, cur_owner;
  for(list.Start(); !list.Last(); list.Next()) {
    obj = list.Get();
    cur_owner = obj.Get_Owner();
    if (cur_owner.Valid()) {
      if (!owner.Valid()) owner = cur_owner;
      else if (owner != cur_owner)
	Am_Error("Moving objects from different groups.");
    }
    if (obj.Is_Instance_Of (Am_Line)) {
      int x1, x2, y1, y2;
      x1 = obj.Get(Am_X1);
      x2 = obj.Get(Am_X2);
      y1 = obj.Get(Am_Y1);
      y2 = obj.Get(Am_Y2);
      min_left = IMIN (IMIN (min_left, x1), x2);
      min_top = IMIN (IMIN (min_top, y1), y2);
      max_right = IMAX (IMAX (max_right, x1), x2);
      max_bottom = IMAX (IMAX (max_bottom, y1), y2);
    }
    else {
      cur_left = obj.Get(Am_LEFT);
      cur_top = obj.Get(Am_TOP);
      min_left = IMIN(min_left, cur_left);
      min_top = IMIN(min_top, cur_top);
      max_right = IMAX(max_right, cur_left + (int)obj.Get(Am_WIDTH));
      max_bottom = IMAX(max_bottom, cur_top + (int)obj.Get(Am_HEIGHT));
    }
  }
}

//figure out the bounds of all the selected objects, and assign it into
//fake_group
void calculate_fake_group_size_and_set(Am_Object& fake_group,
				       Am_Value_List& list) {
  int min_left = 29999;
  int min_top  = 29999;
  int max_right = -29999;
  int max_bottom = -29999;
  int left_offset, top_offset;
  Am_Object owner;
  calculate_group_size(list, min_left, min_top, max_right, max_bottom, owner);
  Am_Translate_Coordinates(owner, 0, 0, fake_group.Get_Owner(),
			   left_offset, top_offset);
  fake_group.Set(Am_LEFT, min_left + left_offset);
  fake_group.Set(Am_TOP, min_top + top_offset);
  int wh = max_right - min_left;
  if (wh == 0) wh = 1;
  fake_group.Set(Am_WIDTH, wh);
  wh = max_bottom - min_top;
  if (wh == 0) wh = 1;
  fake_group.Set(Am_HEIGHT, wh);
  // cout << "Calculate fake group " << fake_group << " size = " << min_left
  // << "," << min_top << "," << max_right - min_left << ","
  // << max_bottom - min_top << endl << flush;
}

Am_Define_Method(Am_Object_Method, void, copy_values_for_grow,
		 (Am_Object cmd)) {
  //copy from me to widget's command
  Am_Object widget, widget_command;
  widget = cmd.Get_Owner().Get_Owner();
  Am_Value value;
  if (widget.Valid()) {
    widget_command = widget.Get_Object(Am_MOVE_GROW_COMMAND);
    if (widget_command.Valid()) {
      value=cmd.Peek(Am_OLD_VALUE);
      widget_command.Set(Am_OLD_VALUE, value);
      value=cmd.Peek(Am_VALUE);
      widget_command.Set(Am_VALUE, value);
      value=cmd.Peek(Am_OBJECT_MODIFIED);
      widget_command.Set(Am_OBJECT_MODIFIED, value);

      widget_command.Set(Am_GROWING, true);
    }
  }
}

static void set_commands_for_move (const Am_Object& widget,
	   const Am_Object& inter, const Am_Value& object_modified,
           const Am_Value& old_value, const Am_Value& new_value,
           int final_left, int final_top, const Am_Object& dest_widget) {
  Am_Object inter_command, widget_command;
  inter_command = inter.Get_Object(Am_COMMAND);
  if (inter_command.Valid()) {
    inter_command.Set(Am_OLD_VALUE, old_value);
    inter_command.Set(Am_VALUE, new_value);
    inter_command.Set(Am_OBJECT_MODIFIED, object_modified);
    inter_command.Set(Am_LEFT, final_left);
    inter_command.Set(Am_TOP, final_top);
    inter_command.Set(Am_SELECTION_WIDGET, dest_widget);
  }
  widget_command = widget.Get_Object(Am_MOVE_GROW_COMMAND);
  if (widget_command.Valid()) {
    widget_command.Set(Am_OLD_VALUE, old_value);
    widget_command.Set(Am_VALUE, new_value);
    widget_command.Set(Am_OBJECT_MODIFIED, object_modified);
    widget_command.Set(Am_GROWING, false);
  }
}

//move all the selected objects by the offsets.
static void adjust_all_objects_position (const Am_Object& widget,
      const Am_Object& inter, Am_Value_List& list,
      int left_offset, int top_offset, int final_left, int final_top,
      Am_Object& new_owner, const Am_Object& dest_widget) {
  // cout << "<>adjusting offsets of sel objects by " << left_offset << ","
  //   << top_offset << endl << flush;
  Am_Object obj;
  Am_Value_List new_values, old_values;
  Am_Object owner;
  for(list.Start(); !list.Last(); list.Next()) {
    obj = list.Get();
    if (new_owner.Is_Part_Of (obj)) {
      Am_Beep ();
      Am_Abort_Interactor (inter);
      return;
    }
  }
  int old_left, old_top, new_left, new_top, w, h;
  for(list.Start(); !list.Last(); list.Next()) {
    obj = list.Get();
    owner = obj.Get_Owner();
    old_left = obj.Get(Am_LEFT);
    old_top = obj.Get(Am_TOP);
    new_left = old_left + left_offset;
    new_top = old_top + top_offset;
    w = obj.Get(Am_WIDTH);
    h = obj.Get(Am_HEIGHT);
    old_values.Add(Am_Inter_Location(false, owner, old_left, old_top, w, h));
    new_values.Add(Am_Inter_Location(false, new_owner, new_left, new_top,w,h));
    Am_INTER_TRACE_PRINT(widget, "Selection handle widget " << widget
			 << " moving  " << obj << " to " <<
			 new_left << "," << new_top);
    //Am_Temporary_Turn_Off_Animator(obj);
    obj.Set(Am_LEFT, new_left, Am_NO_ANIMATION);
    obj.Set(Am_TOP, new_top, Am_NO_ANIMATION);
    //Am_Temporary_Restore_Animator(obj);
    if (owner != new_owner) {
      obj.Remove_From_Owner ();
      new_owner.Add_Part (obj);
    }
  }
  Am_Value objs_value, old_values_value, new_values_value;
  objs_value = list;
  old_values_value = old_values;
  new_values_value = new_values;
  set_commands_for_move(widget, inter, objs_value, old_values_value,
			new_values_value, final_left, final_top, dest_widget);
}

//call the where function on the operates on value
Am_Define_Method(Am_Where_Method, Am_Object, owner_start_where_operates_on,
		 (Am_Object inter, Am_Object object, Am_Object event_window,
		  int x, int y)) {
  Am_Object operates_on = object.Get(Am_OPERATES_ON);
  Am_Where_Method method;
  method = object.Get(Am_START_WHERE_TEST);
  return method.Call(inter, operates_on, event_window, x, y);
}
  

//Where function: For moving.  Same as owner_start_where_or_none but won't
//return object (if click in background).  Also, if multiple objects selected,
//returns the fake group, but first sets its size based on the current
//selection.
Am_Define_Method(Am_Where_Method, Am_Object, owner_start_where_or_fake,
		 (Am_Object inter, Am_Object object, Am_Object event_window,
		  int x, int y)) {
  Am_Object ret = owner_start_where_operates_on_proc(inter, object,
						     event_window, x, y);
  if (ret.Valid()) { //have a valid selection
    Am_Value_List list;
    list = object.Get(Am_VALUE);
    list.Start();
    if (!list.Member(ret))
      ret = Am_No_Object;  //click caused the object to now not be selected
    else if (list.Length() > 1) { // otherwise, ret is OK
      Am_Object fake_group = object.Get (Am_FAKE_GROUP);
      if (!fake_group.Get_Owner ().Valid ()) {
        fake_group = fake_group.Create ();
        object.Add_Part (fake_group);
        object.Set (Am_FAKE_GROUP, fake_group);
      }
      else if (fake_group.Get_Owner () != object) {
        fake_group.Remove_From_Owner ();
        object.Add_Part (fake_group);
      }
      calculate_fake_group_size_and_set(fake_group, list);
      ret = fake_group;
    }
  }
  return ret;
}

//returns false if any items in selection are marked as invalid for moving
//returns true if OK
bool check_all_objects_moveable(Am_Object &inter) {
  Am_Object widget = inter.Get_Owner();
  Am_Value_List selection = widget.Get(Am_VALUE);
  bool ret = Am_Check_All_Objects_For_Inactive(selection, Am_MOVE_INACTIVE);
  return ret;
}

Am_Define_Method(Am_Current_Location_Method, void, sel_move_start_do,
		 (Am_Object command_obj, Am_Object /* object_modified */,
		  Am_Inter_Location /*data*/)) {
  Am_Object inter;
  inter = command_obj.Get_Owner();
  if (!check_all_objects_moveable(inter)) {
    Am_Beep();
    Am_Abort_Interactor(inter);
    return;
  }
}

Am_Define_Method_Type_Impl (Am_In_Region_Method);
Am_Define_Method_Type_Impl (Am_Drop_Target_Interim_Do_Method);
Am_Define_Method_Type_Impl (Am_Drop_Target_Background_Interim_Do_Method);
Am_Define_Method_Type_Impl (Am_Drop_Target_Do_Method);
Am_Define_Method_Type_Impl (Am_Drop_Target_Background_Do_Method);

Am_Define_Method(Am_Current_Location_Method, void, sel_move_inter_interim_do,
		 (Am_Object inter, Am_Object object_modified,
		  Am_Inter_Location data)) {
  Am_Object feedback;
  feedback = inter.Get (Am_FEEDBACK_OBJECT); 
  // Test background to see if moving is allowed there
  Am_Object final_owner = Am_Find_Destination_Group (object_modified, inter);
  Am_Object prev_owner = inter.Get (Am_SAVED_OLD_OWNER);
  if (final_owner != prev_owner) {
    inter.Set (Am_SAVED_OLD_OWNER, final_owner, Am_OK_IF_NOT_THERE);
    if (final_owner.Valid ()) {
      Am_Value value;
      value=final_owner.Peek(Am_DROP_TARGET);
      if (value.Valid ()) {
        Am_Object target = value;
        value=target.Peek(Am_DROP_TARGET_TEST);
        if (value.Valid () &&
	    Am_Drop_Target_Background_Interim_Do_Method::Test (value)) {
          Am_Drop_Target_Background_Interim_Do_Method interim (value);
          Am_Value selected = inter.Get_Owner ().Get (Am_VALUE);
          Am_Background_Drop_Result result =
	        interim.Call (target, data, selected);
          if (result == Am_DROP_NOT_ALLOWED) {
            if (feedback.Valid ()) 
              feedback.Set (Am_VISIBLE, false);
            return;
	  }
        }
      }
      if (feedback.Valid ()) 
        feedback.Set (Am_VISIBLE, true);
    }
  }

  //if get here, then moved enough, so move the feedback 
  if (!feedback.Valid()) feedback = object_modified; //this shouldn't happen
  Am_Check_And_Fix_Feedback_Group (feedback, inter);
  Am_Modify_Object_Pos (feedback, data, false);

  Am_Value drop_test_value;
  drop_test_value = inter.Get_Owner ().Get (Am_DROP_TARGET_TEST);
  if (!drop_test_value.Valid ())
    return;

  Am_Object event_window = inter.Get (Am_WINDOW);
  int x = inter.Get(Am_INTERIM_X);
  int y = inter.Get(Am_INTERIM_Y);
  Am_Object leaf;
  if (Am_Where_Method::Test (drop_test_value)) {
    Am_Where_Method drop_test (drop_test_value);
    leaf = drop_test.Call (inter, event_window, event_window, x, y);
  }
  Am_Value target_value;
  Am_Object target, prev_target;
  while (leaf.Valid ()) {
    target_value=leaf.Peek(Am_DROP_TARGET);
    if (target_value.Valid ()) {
      if (target_value.type != Am_OBJECT)
        Am_ERROR ("Discovered an object with a non-command"
                  " Am_DROP_TARGET_SLOT");
      Am_Object target (target_value);
      prev_target = inter.Get (Am_DROP_TARGET);
      if (target != prev_target) {
        bool valid = false;
        target_value=target.Peek(Am_DROP_TARGET_TEST);
        if (target_value.Valid ()) {
          Am_Value selected = inter.Get_Owner ().Get (Am_VALUE);
          if (Am_Drop_Target_Interim_Do_Method::Test (target_value)) {
            Am_Drop_Target_Interim_Do_Method interim_test (target_value);
            valid = interim_test.Call (target, selected);
	  }
	  else if (Am_Drop_Target_Background_Interim_Do_Method::Test
		     (target_value)) {
            Am_Drop_Target_Background_Interim_Do_Method interim_test
	                                                (target_value);
            /* Am_Background_Drop_Result result = */
                interim_test.Call (target, data, selected);
            valid = false;
	  }
	}
        else
          valid = true;
        if (valid) {
          inter.Set (Am_DROP_TARGET, target);
          target.Set (Am_INTERIM_SELECTED, true);
	}
	else
          inter.Set (Am_DROP_TARGET, Am_No_Object);
        if (prev_target.Valid ())
          prev_target.Set (Am_INTERIM_SELECTED, false);
      }
      return;
    }
    leaf = leaf.Get_Owner ();
  }
  prev_target = inter.Get (Am_DROP_TARGET);
  if (prev_target.Valid ())
    prev_target.Set (Am_INTERIM_SELECTED, false);
  inter.Set (Am_DROP_TARGET, Am_No_Object);
}

static void complete_drop (const Am_Object& inter, Am_Object& target)
{
  Am_Object undo_handler = Am_Inter_Find_Undo_Handler (inter);
  if (undo_handler.Valid ()) {
    Am_Register_Command_Method reg_method =
              undo_handler.Get (Am_REGISTER_COMMAND);
    if (reg_method.Valid ())
      reg_method.Call (undo_handler, target);
    Am_Process_All_Parent_Commands (target, undo_handler, reg_method);
    Am_Abort_Interactor (inter); // drop target handles the rest
  }
}

Am_Define_Method(Am_Current_Location_Method, void, sel_move_do,
		 (Am_Object inter, Am_Object object_modified,
		  Am_Inter_Location data)) {
  // turn off feedback
  Am_Object feedback;
  feedback = inter.Get(Am_FEEDBACK_OBJECT);
  if (feedback.Valid ()) 
    feedback.Set(Am_VISIBLE, false);
  inter.Set (Am_SAVED_OLD_OWNER, Am_No_Object, Am_OK_IF_NOT_THERE);

  Am_Object widget = inter.Get_Owner();
  Am_Value_List list = widget.Get(Am_VALUE);

  //if moving fake-rect, then need to move the individual objects
  int orig_left = 0, orig_top = 0, final_left, final_top;
  Am_Object fake_group, orig_owner;
  if (list.Length() > 1) {
    fake_group = widget.Get (Am_FAKE_GROUP);
    orig_left = fake_group.Get(Am_LEFT);
    orig_top = fake_group.Get(Am_TOP);
    orig_owner = Am_Object (list.Get_Nth (0)).Get_Owner ();
  }
  else
    orig_owner = object_modified.Get_Owner ();

  Am_Object final_owner = Am_Find_Destination_Group (object_modified, inter);
  Am_Object target = inter.Get (Am_DROP_TARGET);

  if (!final_owner.Valid () && !target.Valid ()) {
    Am_Beep ();
    Am_Abort_Interactor (inter);
    return;
  }

  // if drop target is set then drop into target.
  if (!target.Valid ()) { // check background for a target
    Am_Value value;
    value=final_owner.Peek(Am_DROP_TARGET);
    if (value.Valid ()) {
      target = value;
      value=target.Peek(Am_DROP_TARGET_TEST);
      if (value.Valid () &&
	  Am_Drop_Target_Background_Interim_Do_Method::Test (value)) {
        Am_Drop_Target_Background_Interim_Do_Method interim (value);
        Am_Background_Drop_Result result =
	     interim.Call (target, data, Am_Value (list));
        if (result == Am_DROP_NORMALLY)
          target = Am_No_Object;
        else if (result == Am_DROP_NOT_ALLOWED) {
          Am_Beep ();
          Am_Abort_Interactor (inter);
          return;
	}
      }
    }
  }
  if (target.Valid ()) {
    inter.Set (Am_DROP_TARGET, Am_No_Object);
    target.Set (Am_INTERIM_SELECTED, false);
    Am_Value do_value;
    do_value=target.Peek(Am_DO_METHOD);
    if (do_value.Valid ()) {
      if (Am_Drop_Target_Do_Method::Test (do_value)) {
        Am_Drop_Target_Do_Method do_method (do_value);
        do_method.Call (target, Am_Value (list));
      }
      else if (Am_Drop_Target_Background_Do_Method::Test (do_value)) {
        Am_Drop_Target_Background_Do_Method do_method (do_value);
        do_method.Call (target, data, Am_Value (list));
      }
      complete_drop (inter, target);
      return;
    }
    else if (!final_owner.Valid ()) {
      Am_Beep ();
      Am_Abort_Interactor (inter);
      return;
    }
  }

  // move real object or fake rect
  if (final_owner != orig_owner) {
    object_modified.Remove_From_Owner ();
    final_owner.Add_Part (object_modified);
  }
  bool growing = inter.Get(Am_GROWING);
  Am_Modify_Object_Pos (object_modified, data, growing);

  Am_Value multi_owner_value;
  multi_owner_value=inter.Peek(Am_MULTI_OWNERS);
  if (!multi_owner_value.Valid ())
    final_owner = widget.Get (Am_OPERATES_ON);
  Am_Object dest_widget;
  if (orig_owner != final_owner) {
    // object has changed groups, must eliminate selection
    widget.Set (Am_VALUE, Am_No_Value_List);
    // find new widget and set its selection
    Am_Object operates_on;
    Am_Value_List widget_list = widget.Get (Am_MULTI_SELECTIONS);
    for (widget_list.Start (); !widget_list.Last (); widget_list.Next ()) {
      dest_widget = widget_list.Get ();
      operates_on = dest_widget.Get (Am_OPERATES_ON);
      if (operates_on == final_owner) {
        dest_widget.Set (Am_VALUE, list);
        break;
      }
    }
  }
  else
    dest_widget = widget;
  if (list.Length() > 1) {
    final_left = fake_group.Get(Am_LEFT);
    final_top = fake_group.Get(Am_TOP);
    adjust_all_objects_position(widget, inter, list, final_left - orig_left,
				final_top - orig_top, final_left, final_top,
				final_owner, dest_widget);
    if (fake_group.Get_Owner () != widget) {
      fake_group.Remove_From_Owner ();
      widget.Add_Part (fake_group);
    }
  }
  else { // have moved the object, but still have to set up the commands
    // may have to move owner still
    if (orig_owner != final_owner) {
      Am_Inter_Location location (object_modified);
      object_modified.Remove_From_Owner ();
      final_owner.Add_Part (object_modified);
      location.Install (object_modified, false);
      data.Translate_To (final_owner);
    }
    final_left = object_modified.Get (Am_LEFT);
    final_top = object_modified.Get (Am_TOP);
    set_commands_for_move (widget, inter, Am_Value (object_modified),
			   inter.Get (Am_OLD_VALUE), Am_Value (data),
			   final_left, final_top, dest_widget);
  }
  //Am_Temporary_Restore_Animator(object_modified);
}

static Am_Object in_target (Am_Object& top_level, Am_Object& in_leaf,
			    Am_Object& event_window, int x, int y)
{
  Am_Value target_value;
  target_value=in_leaf.Peek(Am_DROP_TARGET);
  if (target_value.Valid ())
    return in_leaf;
  if (in_leaf == top_level)
    return Am_No_Object;
  Am_Object owner = in_leaf.Get_Owner ();
  Am_Object leaf = in_leaf;
  while (owner.Is_Part_Of (top_level)) {
    Am_Value_List parts = owner.Get (Am_GRAPHICAL_PARTS);
    parts.Start ();
    parts.Member (leaf);
    parts.Prev ();
    while (!parts.First ()) {
      leaf = parts.Get ();
      if (Am_Point_In_Obj (leaf, x, y, event_window).Valid ()) {
        leaf = Am_Point_In_Leaf (leaf, x, y, event_window);
        leaf = in_target (owner, leaf, event_window, x, y);
        if (leaf.Valid ())
          return leaf;
      }
      parts.Prev ();
    }
    leaf = owner;
    target_value=leaf.Peek(Am_DROP_TARGET);
    if (target_value.Valid ())
      return leaf;
    owner = leaf.Get_Owner ();
  }
  return Am_No_Object;
}

Am_Define_Method(Am_Where_Method, Am_Object,
		 Am_In_Target,
                 // find the top-most object that has a drop-target slot
		 (Am_Object /* inter */, Am_Object object,
		  Am_Object event_window, int x, int y))
{
  if (!Am_Point_In_All_Owners (object, x, y, event_window) ||
      !Am_Point_In_Obj (object, x, y, event_window).Valid ())
    return Am_No_Object;
  Am_Object leaf = Am_Point_In_Leaf (object, x, y, event_window);
  return in_target (object, leaf, event_window, x, y);
}

///////////////////////////////////////////////////////////////////////////
// For Undo Move/Grow
///////////////////////////////////////////////////////////////////////////

// goes in the Am_IMPLEMENTATION_PARENT slot of the command in the
// interactor to get the Am_MOVE_GROW_COMMAND of the widget
Am_Define_Object_Formula (get_owners_move_grow_command) {
  Am_Object command = 0;
  Am_Object inter = self.Get_Owner(); // owner will be interactor
  if (inter.Valid ()) {
    Am_Object widget = inter.Get_Owner(); // widget the interactor is in
    if (widget.Valid ()) {
      command = widget.Get_Object(Am_MOVE_GROW_COMMAND);
      if (command.Valid ()) {
	if(!command.Is_Instance_Of(Am_Command))
	  command = 0;// then command slot just contains a regular object
      }
    }
  }
  return command;
}

// set new_data_value based on the position(s) of obj(s)
void update_data_from_objs(Am_Value objs_value, Am_Value &new_data_value) {
  Am_Object obj, owner;
  int left, top, w, h;
  if (Am_Value_List::Test(objs_value)) {
    Am_Value_List objs_list, new_data_list;
    objs_list = objs_value;
    for(objs_list.Start(); !objs_list.Last(); objs_list.Next()) {
      obj = objs_list.Get();
      owner = obj.Get_Owner();
      left = obj.Get(Am_LEFT);
      top = obj.Get(Am_TOP);
      w = obj.Get(Am_WIDTH);
      h = obj.Get(Am_HEIGHT);
      new_data_list.Add(Am_Inter_Location(false, owner, left, top, w, h));
    }
    new_data_value = new_data_list;
  }
  else if (Am_Object::Test(objs_value)) {
    obj = objs_value;
    owner = obj.Get_Owner();
    left = obj.Get(Am_LEFT);
    top = obj.Get(Am_TOP);
    w = obj.Get(Am_WIDTH);
    h = obj.Get(Am_HEIGHT);
    Am_Inter_Location new_data(false, owner, left, top, w, h);
    new_data_value = new_data;
  }
  else Am_Error("objs not a list or object");
}

static void remove_object_from_selection (const Am_Object& object,
					  const Am_Object& command)
{
  Am_Object widget = command.Get (Am_SELECTION_WIDGET);
  Am_Value_List select_list = widget.Get (Am_VALUE);
  select_list.Start ();
  if (select_list.Member (object)) {
    select_list.Delete ();
    widget.Set (Am_VALUE, select_list);
  }
}

void update_objs_from_value(Am_Value objs_value, Am_Value new_data_value,
			    bool growing, Am_Object command_obj) {
  int min_left = 29999;
  int min_top  = 29999;
  Am_Object obj;
  Am_Inter_Location new_data;
  if (Am_Value_List::Test(objs_value)) {
    Am_Value_List objs_list, new_data_list;
    if (!Am_Value_List::Test(new_data_value))
      Am_Error("list of objs but not list of values");
    objs_list = objs_value;
    new_data_list = new_data_value;
    if (objs_list.Length() != new_data_list.Length())
      Am_Error("Lists have different lengths");
    int cur_left, cur_top;
    for(objs_list.Start(), new_data_list.Start(); !objs_list.Last();
	objs_list.Next(), new_data_list.Next()) {
      obj = objs_list.Get();
      new_data = new_data_list.Get();
      if (Am_Check_And_Fix_Owner_For_Object (obj, new_data))
        remove_object_from_selection (obj, command_obj);
      Am_Modify_Object_Pos (obj, new_data, growing);
      cur_left = obj.Get(Am_LEFT);
      cur_top = obj.Get(Am_TOP);
      min_left = IMIN(min_left, cur_left);
      min_top = IMIN(min_top, cur_top);
    }
  }
  else if (Am_Object::Test(objs_value)) {
    obj = objs_value;
    new_data = new_data_value;
    if (Am_Check_And_Fix_Owner_For_Object (obj, new_data))
      remove_object_from_selection (obj, command_obj);
    Am_Modify_Object_Pos (obj, new_data, growing);
    min_left = obj.Get(Am_LEFT);
    min_top = obj.Get(Am_TOP);
  }
  else Am_Error("objs not a list or object");
  command_obj.Set(Am_LEFT, min_left);
  command_obj.Set(Am_TOP, min_top);
}

void adjust_all_objects_position_for_undo(Am_Value_List list, int left_offset,
					  int top_offset,
					  Am_Value_List &new_list) {
  Am_Object obj;
  Am_Object owner;
  new_list.Make_Empty();
  int old_left, old_top, new_left, new_top, w, h;
  for(list.Start(); !list.Last(); list.Next()) {
    obj = list.Get();
    owner = obj.Get_Owner();
    old_left = obj.Get(Am_LEFT);
    old_top = obj.Get(Am_TOP);
    w = obj.Get(Am_WIDTH);
    h = obj.Get(Am_HEIGHT);
    new_left = old_left + left_offset;
    new_top = old_top + top_offset;
    Am_INTER_TRACE_PRINT(Am_INTER_TRACE_SETTING, "++Setting " << obj <<
			 " left " << new_left << " top " << new_top);
    obj.Set(Am_LEFT, new_left, Am_NO_ANIMATION);
    obj.Set(Am_TOP, new_top, Am_NO_ANIMATION);
    new_list.Add(Am_Inter_Location(false, owner, new_left, new_top, w, h));
  }
}

void update_new_objs_from_pos(Am_Value objs_value, int left, int top,
			      Am_Value &new_data_value,
			      bool growing, Am_Object command_obj) {
  if (growing) // then must be a single object, so new_data_value will be OK
    update_objs_from_value(objs_value, new_data_value, growing, command_obj);
  else {
    Am_Object obj;
    if (Am_Value_List::Test(objs_value)) {
      Am_Value_List objs_list, new_data_value_list;
      objs_list = objs_value;
      // calculate group offset and move
      int min_left, min_top, max_right, max_bottom;
      Am_Object owner;
      calculate_group_size(objs_list, min_left, min_top, max_right, max_bottom,
			   owner);
      adjust_all_objects_position_for_undo(objs_list, left - min_left,
					   top - min_top, new_data_value_list);
      new_data_value = new_data_value_list;
      command_obj.Set(Am_LEFT, min_left);
      command_obj.Set(Am_TOP, min_top);
    }
    else if (objs_value.type == Am_OBJECT) {
      obj = objs_value;
      obj.Set(Am_LEFT, left, Am_NO_ANIMATION);
      obj.Set(Am_TOP, top, Am_NO_ANIMATION);
      command_obj.Set(Am_LEFT, left);
      command_obj.Set(Am_TOP, top);
    }
  }
}
    

void sel_move_grow_general_undo_redo(Am_Object command_obj, bool undo,
				     bool selective, bool reload_data,
				     Am_Value objs_value) {
  Am_Object inter;
  inter = command_obj.Get(Am_SAVED_OLD_OWNER);
  
  if (reload_data) {
    command_obj.Set(Am_OBJECT_MODIFIED, objs_value);
    Am_Object parent;
    parent = command_obj.Get(Am_IMPLEMENTATION_PARENT);
    if (parent.Valid())
      parent.Set(Am_OBJECT_MODIFIED, objs_value);
  }
  else objs_value=command_obj.Peek(Am_OBJECT_MODIFIED);
  
  #ifdef DEBUG
  if (inter.Valid () && Am_Inter_Tracing(inter)) {
    if (selective) cout << "Selective ";
    if (undo) cout << "Undo"; else cout << "repeat";
    cout << " command " << command_obj << " on " << objs_value <<endl << flush;
  }
  #endif
  if (objs_value.Valid ()) {
    Am_Value old_data_value, new_data_value;

    old_data_value=command_obj.Peek(Am_OLD_VALUE);
    if (!reload_data) new_data_value=command_obj.Peek(Am_VALUE);
    bool growing = command_obj.Get(Am_GROWING);
    if (selective) {
      if (undo) update_data_from_objs(objs_value, new_data_value);
      else      update_data_from_objs(objs_value, old_data_value);
    }
    if (undo) {
      update_objs_from_value(objs_value, old_data_value, growing, command_obj);
      // swap current and old values, in case undo or undo-the-undo again
      command_obj.Set(Am_OLD_VALUE, new_data_value);
      command_obj.Set(Am_VALUE, old_data_value);
    }
    else {
      if (reload_data) {
	int left, top;
	left = command_obj.Get(Am_LEFT);
	top = command_obj.Get(Am_TOP);
	update_new_objs_from_pos(objs_value, left, top, new_data_value,
				 growing, command_obj);
	command_obj.Set(Am_LEFT, left);
	command_obj.Set(Am_TOP, top);
      }
      else update_objs_from_value(objs_value, new_data_value,
				  growing, command_obj);
      if (selective) command_obj.Set(Am_OLD_VALUE, old_data_value);
      if (reload_data) command_obj.Set(Am_VALUE, new_data_value);
    }
  }
}

Am_Define_Method(Am_Object_Method, void, sel_move_undo,
		 (Am_Object command_obj)) {
  sel_move_grow_general_undo_redo(command_obj, true, false, false, 0);
}
Am_Define_Method(Am_Object_Method, void, sel_move_selective_undo,
		 (Am_Object command_obj)){
  sel_move_grow_general_undo_redo(command_obj, true, true, false, 0);
}
Am_Define_Method(Am_Object_Method, void, sel_move_selective_repeat,
		 (Am_Object command_obj)){
  sel_move_grow_general_undo_redo(command_obj, false, true, false, 0);
}
Am_Define_Method(Am_Selective_Repeat_New_Method, void,
		 sel_move_selective_repeat_new,
		 (Am_Object command_obj, Am_Value new_sel)){
  sel_move_grow_general_undo_redo(command_obj, false, true, true, new_sel);
}
 

///////////////////////////////////////////////////////////////////////////
// For Growing
///////////////////////////////////////////////////////////////////////////

//x,y is w.r.t me
bool check_in_8_handles(Am_Object sel_handles, int x, int y) {
  int width = sel_handles.Get(Am_WIDTH);
  int height = sel_handles.Get(Am_HEIGHT);
  
  int width_d2 = width / 2;
  int height_d2 = height / 2;
  
  if (x <= HANDLE_SIZE) {
    // left-top
    if (y <= HANDLE_SIZE) return true;
    // left-middle
    if (y >= height_d2 - HANDLE_SIZE_D2 && y <= height_d2 + HANDLE_SIZE_D2)
      return true;
    // left-bottom
    if (y > height - HANDLE_SIZE) return true;
  }
  else if (x >= width_d2 - HANDLE_SIZE_D2 && x <= width_d2 + HANDLE_SIZE_D2) {
    // middle-top
    if (y <= HANDLE_SIZE) return true;
    // middle-bottom
    if (y > height - HANDLE_SIZE) return true;
  }
  else if (x > width - HANDLE_SIZE) {
    // right-top
    if (y <= HANDLE_SIZE) return true;
    // right-middle
    if (y >= height_d2 - HANDLE_SIZE_D2 && y <= height_d2 + HANDLE_SIZE_D2)
      return true;
    // right-bottom
    if (y > height - HANDLE_SIZE) return true;
  }
  //if get here, then not over a handle
  return false;
}

//x,y is w.r.t sel_handles
bool check_in_line_handles(Am_Object sel_handles, Am_Object for_obj,
			   int x, int y) {
  int x1 = for_obj.Get(Am_X1);
  int y1 = for_obj.Get(Am_Y1);
  int x2 = for_obj.Get(Am_X2);
  int y2 = for_obj.Get(Am_Y2);
  Am_Translate_Coordinates(sel_handles, x, y, for_obj.Get_Owner(), x, y);
  if ((x >= x1-HANDLE_SIZE_D2 && x <= x1+HANDLE_SIZE_D2 &&
       y >= y1-HANDLE_SIZE_D2 && y <= y1+HANDLE_SIZE_D2) ||
      (x >= x2-HANDLE_SIZE_D2 && x <= x2+HANDLE_SIZE_D2 &&
       y >= y2-HANDLE_SIZE_D2 && y <= y2+HANDLE_SIZE_D2))
    return true;
  else return false;
}

// object will be the widget.  
//this is used for growing objects.  Don't need to identify which
//handle it is over, since use Am_WHERE_HIT for grow function of
//move_grow_interactor!
// If over a handle, returns its for_obj
Am_Define_Method(Am_Where_Method, Am_Object,
		 selection_handles_where_function,
		 (Am_Object /*inter*/, Am_Object object, Am_Object event_window,
		  int x, int y)) {
		 
  //object is the whole sections_handles_widget
  // cout  << "Testing widget " << object << endl << flush;
  if (!Am_Point_In_All_Owners(object, x, y, event_window))
    return Am_No_Object;
  //For each of the selections handles I created:
  Am_Value_List parts;
  //handles map is in the Am_FEEDBACK_OBJECT slot of the widget
  parts = object.Get_Object(Am_FEEDBACK_OBJECT).Get(Am_GRAPHICAL_PARTS);
  Am_Object one_handle, for_obj;
  Am_Value value;
  int x1, y1;
  for(parts.Start(); !parts.Last(); parts.Next()) {
    one_handle = parts.Get();
    // cout  << "   Testing handles " << one_handle << endl << flush;
    //translate to coordinate inside of one_handle, so can test against 0
    //instead of left
    if (Am_Translate_Coordinates(event_window, x, y, one_handle, x1, y1)) {
      //first see if inside me at all
      if (Am_Point_In_Obj (one_handle, x1, y1, one_handle).Valid()) {
	// now see if over a handle
	for_obj = one_handle.Get(Am_ITEM);
	if (for_obj.Valid()) { //otherwise no handles
	  value=for_obj.Peek(Am_AS_LINE);
	  bool as_line = value.Valid();
	  if (as_line) {
	    if (check_in_line_handles(one_handle, for_obj, x1, y1))
	      return for_obj;
	  }
	  else {
	    if (check_in_8_handles(one_handle, x1, y1))
	      return for_obj;
	  }
	}
      }
    }
  }
  //if get here, not over a handle
  return Am_No_Object;
}

Am_Define_Method(Am_Selective_Repeat_New_Method, void, sel_grow_repeat_new,
		 (Am_Object /*command_obj*/, Am_Value /*new_sel*/)){
  Am_Beep();
  cout << "** Sorry Do Again Grow not supported \n" << flush;
}

Am_Define_String_Formula(move_or_grow_label) {
  bool growing = self.Get(Am_GROWING);
  if (growing) return Am_String("Grow");
  else return Am_String("Move");
}
  
//////////////////////////////////////////////////////////////////////////
// Select All Command
//////////////////////////////////////////////////////////////////////////

Am_Define_Method(Am_Object_Method, void, select_all_do, (Am_Object cmd)) {
  Am_Object sel_handles, main_group;
  sel_handles = Am_Get_Selection_Widget_For_Command(cmd);
  if (sel_handles.Valid()) {
    main_group = sel_handles.Get(Am_OPERATES_ON);
    if (main_group.Valid()) {
      Am_Value old_value;
      Am_Value_List new_list;
      new_list = main_group.Get(Am_GRAPHICAL_PARTS); // ** may not be all **
      old_value=sel_handles.Peek(Am_VALUE);
      cmd.Set(Am_OLD_VALUE, old_value);
      cmd.Set(Am_VALUE, new_list);
      sel_handles.Set(Am_VALUE, new_list);
      cmd.Set(Am_OBJECT_MODIFIED, sel_handles);
    }
  }
}

Am_Define_Formula (Am_Value, get_sel_widgets_impl_parent) {
  Am_Value value;   //value is the return value;
  Am_Object sel_handles, sel_handles_command;
  value = Am_No_Value;
  sel_handles = self.Get(Am_SELECTION_WIDGET);
  if (sel_handles.Valid()) {
    sel_handles_command = sel_handles.Get_Object(Am_COMMAND);
    if (sel_handles_command.Valid()) {
      //value is the return value;
      value = sel_handles_command.Peek(Am_IMPLEMENTATION_PARENT);
    }
  }
  //cout << "impl parent " << value << " for cmd " << self << " widget "
  //     << sel_handles << " sel command " << sel_handles_command
  //     << endl << flush;
     return value;
}

Am_Define_Formula (bool, am_drag_drop_on)
{
  Am_Value test_value;
  test_value = self.Get_Owner ().Peek (Am_DROP_TARGET_TEST);
  return test_value.Valid ();
}

Am_Define_Value_List_Formula (am_operates_list)
{
  Am_Value value;
  value = self.Get_Owner ().Peek (Am_MULTI_SELECTIONS);
  if (!Am_Value_List::Test (value))
    return Am_No_Value_List;
  Am_Value_List operates_list;
  Am_Value_List selection_list (value);
  for (selection_list.Start (); !selection_list.Last ();
       selection_list.Next ()) {
    operates_list.Add (Am_Object (selection_list.Get ()).Get (Am_OPERATES_ON));
  }
  return operates_list;
}

//////////////////////////////////////////////////////////////////////////
// For selecting in the background
//////////////////////////////////////////////////////////////////////////

Am_Define_Value_List_Formula(return_move_inter) {
  Am_Object mover;
  mover = self.Get_Sibling(Am_MOVE_INTERACTOR);
  if (mover.Valid()) return mover;
  else return NULL;
}

//returns a list of the parts of group inside of region
Am_Define_Method(Am_In_Region_Method, Am_Value_List, Am_Group_Parts_Inside,
		 (Am_Object /* widget */, Am_Object group,
		  Am_Inter_Location region)) {
  Am_Value_List parts = group.Get (Am_GRAPHICAL_PARTS);
  Am_Value_List list;
  Am_Object part;
  Am_Inter_Location part_loc;
  for (parts.Start (); !parts.Last (); parts.Next ()) {
    part = parts.Get ();
    part_loc = Am_Inter_Location(part);
    if (region >= part)
      list.Add (part);
  }
  return list;
}

Am_Define_Method(Am_Current_Location_Method, void, background_sel_do,
		 (Am_Object inter, Am_Object /* object */,
		  Am_Inter_Location points)) {
  Am_Object widget = inter.Get_Owner();
  Am_Input_Char ic = inter.Get(Am_START_CHAR);
  bool toggle_in_set = ic.shift;
  Am_Object feedback = inter.Get(Am_FEEDBACK_OBJECT);
  if (feedback.Valid ()) 
    feedback.Set(Am_VISIBLE, false);

  clear_multi_selections(widget);
  Am_Value_List selection_list;
  if (toggle_in_set)  //else leave selection_list empty
    selection_list = widget.Get(Am_VALUE);
  Am_Object operates_on = widget.Get(Am_OPERATES_ON);
  Am_In_Region_Method method = widget.Get(Am_REGION_WHERE_TEST);
  Am_Value_List object_list = method.Call(widget, operates_on, points);
  Am_Object new_object;
  for (object_list.Start (); !object_list.Last (); object_list.Next ()) {
    new_object = object_list.Get();
    if (toggle_in_set)
      toggle_object_in_list(widget, new_object, selection_list);
    else selection_list.Add(new_object);
  }
  Am_Value old_value;
  old_value=widget.Peek(Am_VALUE);
  widget.Set(Am_VALUE, selection_list);

  // set up for undo, like set_commands_for_sel()
  Am_Object inter_command, widget_command;
  inter_command = inter.Get_Object(Am_COMMAND);
  if (inter_command.Valid()) {
    inter_command.Set(Am_OLD_VALUE, old_value);
    inter_command.Set(Am_VALUE, selection_list);
    inter_command.Set(Am_OBJECT_MODIFIED, selection_list);
  }
  widget_command = widget.Get_Object(Am_COMMAND);
  if (widget_command.Valid()) {
    widget_command.Set(Am_OLD_VALUE, old_value);
    widget_command.Set(Am_VALUE, selection_list);
    widget_command.Set(Am_OBJECT_MODIFIED, selection_list);
  }
}

//////////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////////

//exported objects
Am_Object Am_Selection_Widget;
Am_Object Am_Selection_Widget_Select_All_Command;
Am_Object Am_Drop_Target_Command;

//internal objects
Am_Object Am_One_Selection_Handle;

void Am_Selection_Widget_Initialize()
{
  Am_Object inter, inter2, inter3, inter4, inter5;
  
  Am_Object_Advanced obj_adv; // to get at advanced features

  Am_One_Selection_Handle =
    Am_Graphical_Object.Create ("Am_One_Selection_Handle")
     .Add (Am_ITEM, NULL) //will be set by Am_Selection_Widget map
     .Add (Am_FILL_STYLE, NULL) //if empty, then uses owner's
     //Am_Selections_Handles_LTWH sets all the widget's sizes.
    // .Set (Am_LEFT, selections_handles_ltwh)
     .Set (Am_LEFT, selections_handles_lt)
     .Set (Am_WIDTH, selections_handles_wh)
     .Set (Am_DRAW_METHOD, selection_handles_draw)
     ;

  Am_Selection_Widget = Am_Group.Create("Am_Selection_Widget")
    //parameters
    .Add (Am_START_WHEN, Am_Input_Char("ANY_LEFT_DOWN"))
    .Add (Am_FILL_STYLE, Am_Black)
    .Add (Am_VALUE, Am_Value_List()) // set by the interactors or externally
    .Add (Am_ACTIVE, true) // 
    .Add (Am_OPERATES_ON, NULL) //fill in with group holding parts to select
    .Add (Am_START_WHERE_TEST, Am_Inter_In_Part)
    .Add (Am_REGION_WHERE_TEST, Am_Group_Parts_Inside)
    .Add (Am_MULTI_SELECTIONS, false)
    .Add (Am_DROP_TARGET_TEST, false)
    .Add (Am_GRID_METHOD, false)
    .Add (Am_GRID_X, 0)
    .Add (Am_GRID_Y, 0)
    .Add (Am_SELECT_CLOSEST_POINT_STYLE, NULL) //if one point should be diff
    .Add (Am_SELECT_CLOSEST_POINT_METHOD, Am_Default_Closest_Select_Point) 
    .Add (Am_SELECT_CLOSEST_POINT_OBJ, NULL) //set by method
    .Add (Am_SELECT_CLOSEST_POINT_WHERE, NULL) //set by method
    
    //internal slots
    .Set (Am_LEFT, 0) //must be zero!
    .Set (Am_TOP, 0)
    .Set (Am_WIDTH, width_from_owner_or_zero)
    .Set (Am_HEIGHT, height_from_owner_or_zero)
    .Add (Am_SET_COMMAND_OLD_OWNER, set_command_and_move_old_owner)
    .Add (Am_LINE_FEEDBACK_OBJECT, Am_Line.Create("Sel_Line_Feedback")
      .Add (Am_OPERATES_ON, Am_No_Object)
      .Set (Am_VISIBLE, false)
      .Set (Am_LINE_STYLE, Am_Dashed_Line))
    .Add (Am_RECT_FEEDBACK_OBJECT, Am_Rectangle.Create("Sel_Rect_Feedback")
      .Add (Am_OPERATES_ON, Am_No_Object)
      .Set (Am_VISIBLE, false)
      .Set (Am_LINE_STYLE, Am_Dashed_Line)
      .Set (Am_FILL_STYLE, Am_No_Style))
    
    .Add (Am_FAKE_GROUP, Am_Rectangle.Create("Fake_group")
               .Set (Am_VISIBLE, false)
               )
    .Add_Part (Am_FEEDBACK_OBJECT, Am_Map.Create("Selection_Handles")
               .Set_Part (Am_ITEM_PROTOTYPE,
                          Am_One_Selection_Handle.Create ("Sel_Handle_Proto")
                          )
               //controls the item_prototypes
               .Set (Am_ITEMS, Am_From_Owner(Am_VALUE))
               // components lay out themselves.
               .Set (Am_LAYOUT, NULL) 
               )
    // components lay out themselves.  This just makes
    // checks for objects to go invalid or invisible
    .Set (Am_LAYOUT, remove_from_value_if_invalid)
    ;

  //Visual C++ can't handle really long statements
  Am_Selection_Widget
    .Add_Part (Am_INTERACTOR,
               inter = Am_One_Shot_Interactor.Create("inter_in_sel_widget")
               .Set (Am_PRIORITY, 3.0) //so higher than move
               .Set (Am_HOW_SET, Am_CHOICE_SET)
               .Set (Am_START_WHERE_TEST, owner_start_where_operates_on)
               .Set (Am_RUN_ALSO, return_move_inter)
               // .Set (Am_START_WHERE_TEST, owner_start_where_or_none)
               .Set (Am_START_WHEN, Am_From_Owner(Am_START_WHEN)) 
               .Set (Am_ACTIVE, Am_From_Owner (Am_ACTIVE))
               //use the standard choice_start_do
               .Set (Am_INTERIM_DO_METHOD, NULL)
               .Set (Am_ABORT_DO_METHOD, NULL)
               .Set (Am_DO_METHOD, sel_object)
               );

  //Visual C++ can't handle really long statements
  Am_Selection_Widget.Add_Part (Am_MOVE_INTERACTOR, inter2 = Am_Move_Grow_Interactor.Create("move_inter_in_handle"));
  inter2.Set (Am_PRIORITY, 2.0); //so higher than select_in_background
  inter2.Set (Am_START_WHERE_TEST, owner_start_where_or_fake);
  inter2.Set (Am_FEEDBACK_OBJECT, Am_Compute_MG_Feedback_Object);
  inter2.Set (Am_START_WHEN, compute_drag_start_when);
  inter2.Set (Am_ACTIVE, Am_From_Owner (Am_ACTIVE));

               //use standard start and abort methods
  inter2.Set (Am_INTERIM_DO_METHOD, sel_move_inter_interim_do);
  inter2.Set (Am_DO_METHOD, sel_move_do);
  inter2.Set (Am_ALL_WINDOWS, am_drag_drop_on);
  inter2.Set (Am_MULTI_OWNERS, am_operates_list);
  inter2.Set (Am_MULTI_FEEDBACK_OWNERS, Am_From_Owner (Am_MULTI_SELECTIONS));
  inter2.Add (Am_DROP_TARGET, Am_No_Object);
  inter2.Set (Am_GRID_X, Am_From_Owner (Am_GRID_X));
  inter2.Set (Am_GRID_Y, Am_From_Owner (Am_GRID_Y));
  inter2.Set (Am_GRID_METHOD, Am_From_Owner (Am_GRID_METHOD));
  inter2.Set (Am_CHECK_INACTIVE_COMMANDS, false); //done in interim_do
  inter2.Add (Am_SAVED_OLD_OWNER, Am_No_Object);
  inter2.Get_Object (Am_COMMAND)
                  .Add (Am_LEFT, 0)
                  .Add (Am_TOP, 0)
                  .Add (Am_SELECTION_WIDGET, 0)
                  .Get_Owner ()
               ;

  //the next two are lower priority than the previous select and
  //move, so they won't start if the previous ones take the down event
  //Visual C++ can't handle really long statements
  Am_Selection_Widget
    .Add_Part (Am_SELECT_OUTSIDE_INTERACTOR,
               inter5 = Am_One_Shot_Interactor.Create("sel_widget_background")
               .Set (Am_HOW_SET, Am_CHOICE_SET)
               .Set (Am_START_WHERE_TEST, Am_Inter_In)
               .Set (Am_START_WHEN, compute_click_start_when) 
               .Set (Am_ACTIVE, Am_From_Owner (Am_ACTIVE))
               //use the standard choice_start_do
               .Set(Am_INTERIM_DO_METHOD, NULL)
               .Set(Am_ABORT_DO_METHOD, NULL)
               .Set(Am_DO_METHOD, sel_object)
               );

  //Visual C++ can't handle really long statements
  Am_Selection_Widget
    .Add_Part (Am_BACKGROUND_INTERACTOR,
               inter4 = Am_New_Points_Interactor.Create("drag_in_background")
               .Set (Am_START_WHEN, compute_drag_start_when)
               .Set (Am_AS_LINE, false)
               .Set (Am_ACTIVE, Am_From_Owner (Am_ACTIVE))
               .Set (Am_FEEDBACK_OBJECT, Am_Compute_MG_Feedback_Object)
               .Set (Am_START_WHERE_TEST, Am_Inter_In) //not on part
               .Set (Am_START_DO_METHOD, new_points_start_do)
               .Set (Am_DO_METHOD, background_sel_do)
               );

  //Visual C++ can't handle really long statements
  Am_Selection_Widget
    .Add_Part (Am_GROW_INTERACTOR,
               inter3 = Am_Move_Grow_Interactor.Create("grow_inter_in_handle")
               .Set (Am_START_WHERE_TEST, selection_handles_where_function)
               .Set (Am_GROWING, true)
               .Set (Am_START_WHEN, compute_mg_start_when)
               .Set (Am_FEEDBACK_OBJECT, Am_Compute_MG_Feedback_Object)
               .Set (Am_PRIORITY, 40.0) //so higher than the others
               .Set (Am_START_DO_METHOD, grow_start_do)
               .Set (Am_GRID_X, Am_From_Owner (Am_GRID_X))
               .Set (Am_GRID_Y, Am_From_Owner (Am_GRID_Y))
               .Set (Am_GRID_METHOD, Am_From_Owner (Am_GRID_METHOD))
               )
    .Add_Part (Am_COMMAND, Am_Command.Create("Selection_Command")
               .Set(Am_IMPLEMENTATION_PARENT, Am_NOT_USUALLY_UNDONE)
               .Set(Am_LABEL, "Select")
               )
    .Add_Part (Am_MOVE_GROW_COMMAND, Am_Command.Create("Move_Grow_Command")
               .Add(Am_GROWING, false)
               .Set(Am_LABEL, move_or_grow_label)
               )
    ;

  obj_adv = (Am_Object_Advanced&)Am_Selection_Widget;
  obj_adv.Get_Slot (Am_FILL_STYLE)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_SELECT_CLOSEST_POINT_WHERE)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  
  //selection inter; all the work done by the command
  inter.Set(Am_IMPLEMENTATION_COMMAND, 0);
  /* Get_Object(Am_IMPLEMENTATION_COMMAND)
    .Set_Name("internal_command_in_sel_widget")
    .Set (Am_UNDO_METHOD, NULL)
    .Set (Am_REDO_METHOD, NULL)
    .Set (Am_SELECTIVE_UNDO_METHOD, NULL)
    .Set (Am_SELECTIVE_REPEAT_SAME_METHOD, NULL)
    .Set (Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL);
    */
  
  inter.Get_Object(Am_COMMAND)
    .Set_Name("selection_command_in_sel_widget")
    .Set(Am_IMPLEMENTATION_PARENT, Am_Get_Owners_Command)
    .Set(Am_UNDO_METHOD, Am_Widget_Inter_Command_Undo)
    .Set(Am_REDO_METHOD, Am_Widget_Inter_Command_Undo)
    .Set(Am_SELECTIVE_UNDO_METHOD, Am_Widget_Inter_Command_Selective_Undo)
    .Set(Am_SELECTIVE_REPEAT_SAME_METHOD,
         Am_Widget_Inter_Command_Selective_Repeat)
    .Set(Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL)
    ;

  inter5.Set(Am_IMPLEMENTATION_COMMAND, 0);
/*  inter5.Add_Part(Am_IMPLEMENTATION_COMMAND,
          inter.Get_Object(Am_IMPLEMENTATION_COMMAND).Create("sel2_impl"));
*/
  inter5.Set_Part(Am_COMMAND,
                  inter.Get_Object(Am_COMMAND).Create("sel2_command"));
  
  inter2.Set(Am_IMPLEMENTATION_COMMAND, 0);
/*  .Get_Object(Am_IMPLEMENTATION_COMMAND)
    .Set (Am_UNDO_METHOD, NULL)
    .Set (Am_REDO_METHOD, NULL)
    .Set (Am_SELECTIVE_UNDO_METHOD, NULL)
    .Set (Am_SELECTIVE_REPEAT_SAME_METHOD, NULL)
    .Set (Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL);
    ;
*/

  inter2.Get_Object(Am_COMMAND)
    .Set(Am_IMPLEMENTATION_PARENT, get_owners_move_grow_command)
    .Set(Am_START_DO_METHOD, sel_move_start_do)
    .Set(Am_LABEL, "Move")
    .Add(Am_GROWING, false)
    .Set(Am_UNDO_METHOD, sel_move_undo)
    .Set(Am_REDO_METHOD, sel_move_undo)
    .Set(Am_SELECTIVE_UNDO_METHOD, sel_move_selective_undo)
    .Set(Am_SELECTIVE_REPEAT_SAME_METHOD, sel_move_selective_repeat)
    .Set(Am_SELECTIVE_REPEAT_ON_NEW_METHOD, sel_move_selective_repeat_new)
    .Set_Name("move_command_in_sel_widget")
    ;
  
  inter3.Get_Object(Am_IMPLEMENTATION_COMMAND)
    .Set (Am_SELECTIVE_REPEAT_NEW_ALLOWED,
          Am_Selective_New_Allowed_Return_False) //**NIY**
    .Set (Am_SELECTIVE_REPEAT_ON_NEW_METHOD, sel_grow_repeat_new);
  
  inter3.Get_Object(Am_COMMAND)
    .Set(Am_LABEL, "Grow")
    .Set(Am_IMPLEMENTATION_PARENT, get_owners_move_grow_command)
    .Set(Am_DO_METHOD, copy_values_for_grow)
    .Set_Name("grow_command_in_sel_widget")
    ;

  //even though is a new_point, undo like a one_shot inter
  inter4.Set_Part(Am_COMMAND, inter.Get_Object(Am_COMMAND).Create()
                  .Set_Name("internal_command_for_background"));
  // don't do the implementation command, since it is for new_points
  inter4.Set(Am_IMPLEMENTATION_COMMAND, 0);
                  
  Am_Selection_Widget_Select_All_Command =
    Am_Command.Create("Select_All_Command")
    .Set (Am_LABEL, "Select All")
    .Add (Am_SELECTION_WIDGET, NULL) //set this to associated sel..han..widget
    .Set (Am_DO_METHOD, select_all_do)
    .Set(Am_UNDO_METHOD, Am_Widget_Inter_Command_Undo)
    .Set(Am_REDO_METHOD, Am_Widget_Inter_Command_Undo)
    .Set(Am_SELECTIVE_UNDO_METHOD, Am_Widget_Inter_Command_Selective_Undo)
    .Set(Am_SELECTIVE_REPEAT_SAME_METHOD,
         Am_Widget_Inter_Command_Selective_Repeat)
    .Set(Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL)
    .Set(Am_IMPLEMENTATION_PARENT, get_sel_widgets_impl_parent)
    .Set(Am_ACCELERATOR, Am_Input_Char("CONTROL_a"))
    ;

  Am_Drop_Target_Command = Am_Command.Create ("Drop_Target_Command")
    .Add (Am_INTERIM_SELECTED, false)
  ;
}

