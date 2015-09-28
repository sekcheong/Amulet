/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code has been placed in the public   			          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* Originally written as AmEdit by S.Nebel (Linkworks Ltd Wellington,NZ) 1997.
   Modified and updated by Brad A. Myers of the Amulet project at CMU.
*/

#include <amulet.h>

// strstream is needed for Am_POP_UP_ERROR_WINDOW
#include STR_STREAM__H

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>

#ifdef GCC
#include <string.h>
#else
extern "C" {
#if defined(_MSC_VER) || defined(NEED_STRING)
#include <string.h>
#endif
}
#endif

#include INTER_ADVANCED__H  //for Am_Copy_Values_To_Command for polygon


Am_Define_Method_Type(Am_Customize_Object_Method, void,
		      (Am_Object &command_obj, Am_Object &object))
Am_Define_Method_Type_Impl(Am_Customize_Object_Method)
     

#include "panel.h"
#include "externs.h"
#include "savecpp.h"

Am_Slot_Key CUSTOMIZE_METHOD = Am_Register_Slot_Name ("CUSTOMIZE_METHOD");
Am_Slot_Key SUB_LABEL = Am_Register_Slot_Name ("SUB_LABEL");
Am_Slot_Key Lw_NAME = Am_Register_Slot_Name ("Lw_NAME");
Am_Slot_Key FONT_KEY = Am_Register_Slot_Name ("FONT_KEY");
Am_Slot_Key LAYOUT_KEY = Am_Register_Slot_Name ("LAYOUT_KEY");
Am_Slot_Key FILL_STYLE_KEY = Am_Register_Slot_Name ("FILL_STYLE_KEY");
Am_Slot_Key LINE_STYLE_KEY = Am_Register_Slot_Name ("LINE_STYLE_KEY");
Am_Slot_Key TYPE_NAME = Am_Register_Slot_Name ("TYPE_NAME");
Am_Slot_Key SAVE_COMMAND_ITEMS = Am_Register_Slot_Name ("SAVE_COMMAND_ITEMS");

Am_Object workgroup, menu_bar, colorpanel, sel_widget, win, button_panel,
  main_group, main_group_rect, main_group_rect_proto;

paneldialog paneldlg;

Am_Object arrow_bitmap,
  arc_proto, rectangle_proto, line_proto, polygon_proto,
  text_proto, border_rectangle_proto, button_proto, buttons_proto, ok_proto,
  checkbox_proto, radio_proto, text_input_proto, number_input_proto,
  option_proto, scroll_group_proto;

Am_Object tool_panel;

Am_Object global_undo_handler;

Am_Object rfeedback, lfeedback;

#define ARROW_ID 1
#define RUN_ID 2
#define POLYGON_ID 3

Am_Object grid_command;
int grid_amt = 20;

Am_Object open_command;

void fix_lw_name(Am_String &name) {
  char * s = name;
  while (*s != 0) {
    if (!((*s >= 'a' && *s <= 'z') ||
	  (*s >= 'A' && *s <= 'Z')))
      *s = '_';
    s++;
  }
}

Am_Define_No_Self_Formula (int, grid_if_should) {
  Am_Value grid_on;
  grid_on = grid_command.Peek(Am_VALUE);
  if (grid_on.Valid()) return grid_amt;
  else return 0;
}
Am_Define_No_Self_Formula (bool, selection_tool) {
  Am_Value v = tool_panel.Peek(Am_VALUE);
  if (v.type == Am_INT && v == ARROW_ID) return true;
  else return false;
}
Am_Define_No_Self_Formula (bool, run_tool)
{
  Am_Value v = tool_panel.Peek(Am_VALUE);
  if (v.type == Am_INT && v == RUN_ID) return true;
  else return false;
}
Am_Define_No_Self_Formula (bool, line_tool)
{
  Am_Value v = tool_panel.Peek(Am_VALUE);
  if (v.type == Am_OBJECT && v == line_proto) return true;
  else return false;
}
Am_Define_Object_Formula (compute_feedback_obj) {
  if ((bool)self.Get(Am_AS_LINE)) return lfeedback;
  else return rfeedback;
}
Am_Define_No_Self_Formula (bool, rubber_bandable_tool_is_selected) {
  Am_Value v = tool_panel.Peek(Am_VALUE);
  if (v.type == Am_INT) return false;
  else return true;
}
Am_Define_No_Self_Formula (int, points_from_tool) {
  Am_Value v = tool_panel.Peek(Am_VALUE);
  if (v.type == Am_OBJECT) {
    Am_Object proto = v;
    v = proto.Get(Am_HOW_MANY_POINTS, Am_NO_DEPENDENCY);
    if (v.Valid() && v.type == Am_INT) return v;
  }
  return 2;
}
Am_Define_No_Self_Formula(int, minw_from_tool) {
  Am_Value v = tool_panel.Peek(Am_VALUE);
  if (v.type == Am_OBJECT) {
    Am_Object proto = v;
    v = proto.Get(Am_HOW_MANY_POINTS, Am_NO_DEPENDENCY);
    if (v.Valid() && v.type == Am_INT && v == 1) 
      return proto.Get(Am_WIDTH, Am_NO_DEPENDENCY);
    else
      return proto.Get(Am_MINIMUM_WIDTH);
  }
  return 10;
}
Am_Define_No_Self_Formula(int, minh_from_tool) {
  Am_Value v = tool_panel.Peek(Am_VALUE);
  if (v.type == Am_OBJECT) {
    Am_Object proto = v;
    v = proto.Get(Am_HOW_MANY_POINTS, Am_NO_DEPENDENCY);
    if (v.Valid() && v.type == Am_INT && v == 1)
      return proto.Get(Am_HEIGHT, Am_NO_DEPENDENCY);
    else
      return proto.Get(Am_MINIMUM_HEIGHT);
  }
  return 10;
}
Am_Define_No_Self_Formula (bool, polygon_tool_is_selected) {
  Am_Value v = tool_panel.Peek(Am_VALUE);
  if (v.type == Am_INT && v == POLYGON_ID) return true;
  else return false;
}

Am_Define_No_Self_Formula(Am_Wrapper*, undo_handler_if_not_running) {
  //disable undoing if running
  Am_Value v = tool_panel.Peek(Am_VALUE);
  if (v.type == Am_INT && v == RUN_ID) return Am_No_Object;
  else return global_undo_handler;
}

char* fontnames[] = { "Sm", "B", "I", "U",
              "Me", "B", "I", "U" ,
	      "La", "B", "I", "U",
	      };


Am_String Add_Extension(Am_String in_filename, const char * new_ext) {
  if (!in_filename.Valid()) return in_filename;
  char * in_filestr = in_filename;
  int len = strlen(in_filestr);
  if (len == 0) return in_filename;
  char * ext_part = strrchr(in_filestr, '.');
  int ext_len = 0;
  if (ext_part) {
    ext_len = strlen(ext_part);
    if (ext_len > 5) //probably not a real extension
      ext_len = 0;
  }
  int new_ext_len = strlen(new_ext);
  int new_len = len - ext_len + new_ext_len;
  char *new_str = new char[new_len+1]; //terminating \0
  new_str = strncpy(new_str, in_filestr, len - ext_len);
  new_str[len - ext_len] = '\0';
  new_str = strcat(new_str, new_ext);
  return Am_String(new_str, false);
}

/////////////////////// save load stuff /////////////////////////

// This method should take the supplied contents list and add it to
// the window, after removing what is already there
Am_Define_Method (Am_Handle_Loaded_Items_Method, void, use_file_contents,
		  (Am_Object /* command */, Am_Value_List &contents)) {
  Am_Value_List current = workgroup.Get (Am_GRAPHICAL_PARTS);
  Am_Object item;
  //first delete all of the current contents of the window
  for (current.Start (); !current.Last (); current.Next ()) {
    item = current.Get ();
    item.Destroy ();
  }
  //now add the new objects, fixing up the slots
  Am_Value value;
  for (contents.Start (); !contents.Last (); contents.Next ()) {
    item = contents.Get ();

    if (item.Is_Instance_Of(main_group_rect_proto)) {
      main_group_rect.Set(MAIN_NAME, item.Get(MAIN_NAME));
      main_group_rect.Set(C_FILENAME, item.Get(C_FILENAME));
      main_group_rect.Set(CREATE_HEADER, item.Get(CREATE_HEADER));
      main_group_rect.Set(H_FILENAME, item.Get(H_FILENAME));
      main_group_rect.Set(WINDOW_OR_GROUP, item.Get(WINDOW_OR_GROUP));
      main_group_rect.Set(WIN_TITLE, item.Get(WIN_TITLE));
      main_group_rect.Set(FIXED_SIZE_OBJ, item.Get(FIXED_SIZE_OBJ));
      main_group_rect.Set(EXPLICIT_SIZE_OBJ, item.Get(EXPLICIT_SIZE_OBJ));
      main_group_rect.Set(WIDTH_OBJ, item.Get(WIDTH_OBJ));
      main_group_rect.Set(HEIGHT_OBJ, item.Get(HEIGHT_OBJ));
      main_group_rect.Set(FILL_STYLE_KEY, item.Get(FILL_STYLE_KEY));
      main_group_rect.Set(Am_FILL_STYLE, n2s[(int)item.Get(FILL_STYLE_KEY)]);
    }
    else {
      value = item.Peek(FONT_KEY);
      if (value.Exists()) {
	item.Set(Am_FONT, fontarray[(int)value]);
      }
      value = item.Peek(LAYOUT_KEY);
      if (value.Exists()) {
	if ((int)value == 1)
	  item.Set(Am_LAYOUT, Am_Horizontal_Layout);
	else 
	  item.Set(Am_LAYOUT, Am_Vertical_Layout);
      }
      value = item.Peek(FILL_STYLE_KEY);
      if (value.Exists()) {
	item.Set(Am_FILL_STYLE, n2s[(int)value]);
      }
      value = item.Peek(LINE_STYLE_KEY);
      if (value.Exists()) {
	item.Set(Am_LINE_STYLE, n2l[(int)value]);
      }
      value = item.Peek(SUB_LABEL);
      if (value.Exists()) {
	item.Get_Object(Am_COMMAND).Set(Am_LABEL, value);
      }
      
      workgroup.Add_Part (item);
    }
  }
}

//This method should return the list of objects to save
Am_Define_Method (Am_Items_To_Save_Method, Am_Value_List, contents_for_save,
		  (Am_Object /* command */)) {
  Am_Value_List obs_to_save = workgroup.Get (Am_GRAPHICAL_PARTS);
  obs_to_save.Add(main_group_rect);
  return obs_to_save;
}

////////////////////////////////////////////////////////////////////////

Am_Define_Style_Formula(get_fill_style)
{
  self.Set(FILL_STYLE_KEY, (int)self.Get(Am_RANK), Am_OK_IF_NOT_THERE);
  if (self.Get(Am_RANK) == -1) return (Am_No_Style);
  else return n2s[(int)self.Get(Am_RANK)];

}


Am_Define_Style_Formula(get_line_style)
{
  self.Set(LINE_STYLE_KEY, (int)self.Get(Am_RANK), Am_OK_IF_NOT_THERE);
  if (self.Get(Am_RANK) == -1) return (Am_No_Style);
  else return n2l[(int)self.Get(Am_RANK)];

}


Am_Define_String_Formula(fontgen)
{
  if (self.Get(Am_RANK) == -1) return (Am_String("0"));
  else {
    return(Am_String(fontnames[(int) self.Get(Am_RANK)]));
  }  

}

Am_Define_Method(Am_Object_Method, void, savecppcmd, (Am_Object /*cmd*/)) {
  Am_String save_name = sel_widget.Get(Am_DEFAULT_LOAD_SAVE_FILENAME,
				       Am_RETURN_ZERO_ON_ERROR);
  if (!save_name.Valid()) save_name = "";
  Am_String main_obj_name = main_group_rect.Get(MAIN_NAME);
  if (!main_obj_name.Valid() || main_obj_name == "") {
    main_obj_name = Add_Extension(save_name, "");
  }
  if (!main_obj_name.Valid() || main_obj_name == "") main_obj_name = "object";
  fix_lw_name(main_obj_name);

  Am_String c_file_name = main_group_rect.Get(C_FILENAME);
  if (!c_file_name.Valid() || c_file_name == "") {
    c_file_name = Add_Extension(save_name, ".cc");
  }

  bool create_header = main_group_rect.Get(CREATE_HEADER);
  Am_String h_file_name = main_group_rect.Get(H_FILENAME);
  if (!h_file_name.Valid() || h_file_name == "") {
    h_file_name = Add_Extension(save_name, ".h");
  }
  bool create_window = main_group_rect.Get(WINDOW_OR_GROUP);
  Am_String window_title = main_group_rect.Get(WIN_TITLE);
  bool win_fixed_size = main_group_rect.Get(FIXED_SIZE_OBJ);
  bool explicit_size = main_group_rect.Get(EXPLICIT_SIZE_OBJ);
  int width = main_group_rect.Get(Am_WIDTH);
  int height = main_group_rect.Get(Am_HEIGHT);
  
  savecpp_window.Get_Object(MAIN_NAME).Set(Am_VALUE, main_obj_name);
  savecpp_window.Get_Object(C_FILENAME).Set(Am_VALUE, c_file_name);

  if (create_header) savecpp_window.Get_Object(CREATE_HEADER)
    .Set(Am_VALUE, Am_Value_List().Add(1));
  else savecpp_window.Get_Object(CREATE_HEADER).Set(Am_VALUE, Am_Value_List());

  savecpp_window.Get_Object(H_FILENAME).Set(Am_VALUE, h_file_name);

  if (create_window)
    savecpp_window.Get_Object(WINDOW_OR_GROUP).Set(Am_VALUE, 1);
  else savecpp_window.Get_Object(WINDOW_OR_GROUP).Set(Am_VALUE, 2);

  savecpp_window.Get_Object(WIN_TITLE).Set(Am_VALUE, window_title);

  if (win_fixed_size)
    savecpp_window.Get_Object(FIXED_SIZE_OBJ)
      .Set(Am_VALUE, Am_Value_List().Add(1));
  else savecpp_window.Get_Object(FIXED_SIZE_OBJ)
    .Set(Am_VALUE, Am_Value_List());

  if (explicit_size)
    savecpp_window.Get_Object(EXPLICIT_SIZE_OBJ) .Set(Am_VALUE, 1);
  else savecpp_window.Get_Object(EXPLICIT_SIZE_OBJ).Set(Am_VALUE, 2);

  savecpp_window.Get_Object(WIDTH_OBJ).Set(Am_VALUE, width);
  savecpp_window.Get_Object(HEIGHT_OBJ).Set(Am_VALUE, height);
    
  Am_Value ok;
  Am_Pop_Up_Window_And_Wait(savecpp_window, ok, true);
  if (ok.Valid()) {
    main_obj_name = savecpp_window.Get_Object(MAIN_NAME).Get(Am_VALUE);
    fix_lw_name(main_obj_name);

    c_file_name = savecpp_window.Get_Object(C_FILENAME).Get(Am_VALUE);
    Am_Value_List l = savecpp_window.Get_Object(CREATE_HEADER).Get(Am_VALUE);
    l.Start();
    create_header = l.Member(1);
    h_file_name = savecpp_window.Get_Object(H_FILENAME).Get(Am_VALUE);
    int val = savecpp_window.Get_Object(WINDOW_OR_GROUP) .Get(Am_VALUE);
    create_window = val == 1;
    window_title = savecpp_window.Get_Object(WIN_TITLE).Get(Am_VALUE);
    l = savecpp_window.Get_Object(FIXED_SIZE_OBJ) .Get(Am_VALUE);
    l.Start();
    win_fixed_size = l.Member(1);
    val = savecpp_window.Get_Object(EXPLICIT_SIZE_OBJ) .Get(Am_VALUE); 
    explicit_size = val == 1;
    width = savecpp_window.Get_Object(WIDTH_OBJ).Get(Am_VALUE);
    height = savecpp_window.Get_Object(HEIGHT_OBJ).Get(Am_VALUE);

    main_group_rect.Set(MAIN_NAME, main_obj_name);
    main_group_rect.Set(C_FILENAME, c_file_name);
    main_group_rect.Set(CREATE_HEADER, create_header);
    main_group_rect.Set(H_FILENAME, h_file_name);
    main_group_rect.Set(WINDOW_OR_GROUP, create_window);
    main_group_rect.Set(WIN_TITLE, window_title);
    main_group_rect.Set(FIXED_SIZE_OBJ, win_fixed_size);
    main_group_rect.Set(EXPLICIT_SIZE_OBJ, explicit_size);
    main_group_rect.Set(WIDTH_OBJ, width);
    main_group_rect.Set(HEIGHT_OBJ, height);

    int fill_key = main_group_rect.Get(FILL_STYLE_KEY);
    Am_Value_List top_level_objs_list = workgroup.Get(Am_GRAPHICAL_PARTS);
    if (create_header) {
      output_cc_with_header(h_out_file, cc_out_file, create_window,
			    top_level_objs_list, main_obj_name, fill_key,
			    window_title, explicit_size, width, height,
			    win_fixed_size);
      h_out_file.close();
      cout << "Wrote " << h_file_name << " and " << c_file_name
	   << endl << flush;
    }
    else {
      output_cc_no_header(cc_out_file, create_window,
			  top_level_objs_list, main_obj_name, fill_key,
			  window_title, explicit_size, width, height,
			  win_fixed_size);
      cout << "Wrote " << c_file_name << endl << flush;

    }
    cc_out_file.close();
  }
}

Am_Define_Method(Am_Object_Method, void, fillstyler, (Am_Object cmd))
{
  Am_Value_List l = sel_widget.Get(Am_VALUE);
  Am_Object vobj = cmd.Get(Am_VALUE);
  Am_Style s = vobj.Get(Am_FILL_STYLE);
  int key =  vobj.Get(FILL_STYLE_KEY);
  if (l.Empty()) { //set the background
    main_group_rect.Set(Am_FILL_STYLE, s).Set(FILL_STYLE_KEY,key);
  }
  else {
    for(l.Start(); !l.Last(); l.Next()) {
      Am_Object am = l.Get();
      Am_Value old_key = am.Peek(FILL_STYLE_KEY);
      if (old_key.Exists()) {
	// supports setting fill_style
	am.Set(Am_FILL_STYLE, s).Set(FILL_STYLE_KEY,key);
      }
    }
  }
}

Am_Define_Method(Am_Object_Method, void, linestyler, (Am_Object cmd))
{
  Am_Value_List l = sel_widget.Get(Am_VALUE);
  Am_Object vobj = cmd.Get(Am_VALUE);
  Am_Style s = vobj.Get(Am_LINE_STYLE);
  int key =  vobj.Get(LINE_STYLE_KEY);
  for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    Am_Value old_key = am.Peek(LINE_STYLE_KEY);
    if (old_key.Exists()) {
      // supports setting line_style
      am.Set(Am_LINE_STYLE, s).Set(LINE_STYLE_KEY,key);
    }
  }
}

Am_Define_Method(Am_Object_Method, void, fontstyler, (Am_Object cmd))
{
  Am_Value_List l = sel_widget.Get(Am_VALUE);
  Am_Object vobj = cmd.Get(Am_VALUE);
  Am_Font s = fontarray[(int)vobj.Get(Am_RANK)];

  Am_Value old_f_val;
  Am_Object am;
  for(l.Start(); !l.Last(); l.Next()) {
    am = l.Get();
    old_f_val = am.Peek(FONT_KEY);
    if (old_f_val.Exists()) { //then supports setting the font
      am.Set(Am_FONT, s);
      am.Set(FONT_KEY, (int)vobj.Get(Am_RANK));
    }
    //** do something with old_f_val for UNDO
  }
}

Am_Define_Method(Am_Object_Method, void, aboutcmd, (Am_Object /* cmd */)) {
  Am_Value ok;
  Am_Pop_Up_Window_And_Wait(About_Gilt_Window, ok, true);
}

Am_Define_Method(Am_Object_Method, void, align_up, (Am_Object /* cmd */)) {
  int minval = 1000000; 
  Am_Value_List l = sel_widget.Get(Am_VALUE);

  for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    int val = am.Get(Am_TOP) ;
    if (minval > val) minval = val;
  }
  for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    am.Set(Am_TOP, minval) ;
  }  
}

Am_Define_Method(Am_Object_Method, void, align_down, (Am_Object /* cmd */)) {
  int minval = -10000; 
  Am_Value_List l = sel_widget.Get(Am_VALUE);

  Am_Object am;
  int val;
  for(l.Start(); !l.Last(); l.Next()) {
    am = l.Get();
    val = (int)am.Get(Am_TOP) + (int)am.Get(Am_HEIGHT) ;
    if (minval < val) minval = val;
  }
  for(l.Start(); !l.Last(); l.Next()) {
    am = l.Get();
    am.Set(Am_TOP, minval - (int)am.Get(Am_HEIGHT)) ;
  }  
}

Am_Define_Method(Am_Object_Method, void, align_right, (Am_Object /* cmd */)) {
 int minval = -1000000; 
 Am_Value_List l = sel_widget.Get(Am_VALUE);

  for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    int val = (int)am.Get(Am_LEFT) + (int) am.Get(Am_WIDTH);
    if (minval < val) minval = val;
  }
    for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    am.Set(Am_LEFT, minval - (int)am.Get(Am_WIDTH)) ;
  }  
}

Am_Define_Method(Am_Object_Method, void, align_left, (Am_Object /* cmd */))
{
 int minval = 1000000; 
 Am_Value_List l = sel_widget.Get(Am_VALUE);

  for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    int val = am.Get(Am_LEFT) ;
    if (minval > val) minval = val;
  }
    for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    am.Set(Am_LEFT, minval) ;
  }  
}

Am_Define_Method(Am_Object_Method, void, same_widths, (Am_Object /* cmd */)) {
 int max_width = -1000000; 
 Am_Value_List l = sel_widget.Get(Am_VALUE);

 Am_Object am;
 for(l.Start(); !l.Last(); l.Next()) {
   am = l.Get();
   int val = am.Get(Am_WIDTH);
   if (val > max_width) max_width = val;
  }
 for(l.Start(); !l.Last(); l.Next()) {
   am = l.Get();
   am.Set(Am_WIDTH, max_width);
  }  
}
Am_Define_Method(Am_Object_Method, void, same_heights, (Am_Object /* cmd */)) {
 int max_height = -1000000; 
 Am_Value_List l = sel_widget.Get(Am_VALUE);

 Am_Object am;
 for(l.Start(); !l.Last(); l.Next()) {
   am = l.Get();
   int val = am.Get(Am_HEIGHT);
   if (val > max_height) max_height = val;
  }
 for(l.Start(); !l.Last(); l.Next()) {
   am = l.Get();
   am.Set(Am_HEIGHT, max_height);
  }
}

Am_Define_Method(Am_Object_Method, void, go_up, (Am_Object /* cmd */))
{
 Am_Value_List l = sel_widget.Get(Am_VALUE);

  for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    am.Set(Am_TOP, (int)am.Get(Am_TOP) -1 );
  }

}
Am_Define_Method(Am_Object_Method, void, go_down, (Am_Object /* cmd */))
{
 Am_Value_List l = sel_widget.Get(Am_VALUE);


  for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    am.Set(Am_TOP, (int)am.Get(Am_TOP)+ 1);
  }

}


Am_Define_Method(Am_Object_Method, void, go_left, (Am_Object /* cmd */))
{
 Am_Value_List l = sel_widget.Get(Am_VALUE);


  for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    am.Set(Am_LEFT, (int)am.Get(Am_LEFT) - 1);
  }

}



Am_Define_Method(Am_Object_Method, void, go_right, (Am_Object /* cmd */))
{
  Am_Value_List l = sel_widget.Get(Am_VALUE);
  for(l.Start(); !l.Last(); l.Next()) {
    Am_Object am = l.Get();
    am.Set(Am_LEFT, (int)am.Get(Am_LEFT) +1 );
  }
}
Am_Define_Method(Am_Object_Method, void, go_into_run_mode,
		 (Am_Object /* cmd */)) {
  sel_widget.Set(Am_VALUE, Am_Value_List()); //rest down by constraints
}

Am_Define_Method(Am_Object_Method, void, fake_do_cancel, (Am_Object cmd)) {
  Am_String which_cmd = cmd.Get(Am_LABEL);
  Am_Beep();
  Am_Show_Alert_Dialog (Am_Value_List()
			.Add(which_cmd)
			.Add("button pressed in Run mode.")
			.Add("Leaving Run Mode."));
  tool_panel.Set(Am_VALUE, ARROW_ID); //return to selection after each create
}

void do_customize(Am_Object &cmd, Am_Object &obj) {
  Am_Value v = obj.Peek(CUSTOMIZE_METHOD);
  if (v.Valid()) {
    Am_Customize_Object_Method method = v;
    method.Call(cmd, obj);
  }
  //this shouldn't happen:
  else Am_POP_UP_ERROR_WINDOW("Object " << obj << " cannot be customized");
}

Am_Define_Method(Am_Object_Method, void, customize, (Am_Object cmd))
{
  Am_Object owner = cmd.Get(Am_VALUE);
  if (owner.Valid())
    do_customize(cmd, owner);
}

Am_Define_Method(Am_Object_Method, void, customize_selected, (Am_Object cmd)) {
  Am_Value_List selected = sel_widget.Get(Am_VALUE);
  if (selected.Empty()) Am_Error("Nothing selected"); //not possible
  else if (selected.Length() > 1) {
    Am_Pop_Up_Error_Window("Can only get properties on one object at a time.");
    Am_Abort_Widget(cmd); //keep it out of the undo history
  }
  else {
    selected.Start();
    Am_Object obj = selected.Get();
    do_customize(cmd, obj);
  }
}

Am_Define_Method(Am_Customize_Object_Method, void, customize_name_only,
		 (Am_Object &cmd, Am_Object &owner)) {
  Am_String vs = owner.Get(Lw_NAME);
  Name_Only_Window.Get_Object(NAME_OBJ).Set(Am_VALUE, vs);
  Am_Value ok;
  Am_Pop_Up_Window_And_Wait(Name_Only_Window, ok, true);
  if (ok.Valid()) {
    Am_String new_vs = Name_Only_Window.Get_Object(NAME_OBJ).Get(Am_VALUE);
    fix_lw_name(new_vs);
    owner.Set(Lw_NAME, new_vs);
    cmd.Set(Am_OBJECT_MODIFIED, owner);
    cmd.Set(Am_SLOTS_TO_SAVE, Am_Value_List().Add(Lw_NAME),
	    Am_OK_IF_NOT_THERE);
    cmd.Set(Am_OLD_VALUE, Am_Value_List().Add(vs));
    cmd.Set(Am_VALUE, Am_Value_List().Add(new_vs));
  }
  else Am_Abort_Widget(cmd); //keep out of undo history
}

Am_Define_Method(Am_Customize_Object_Method, void, customize_text,
		 (Am_Object &cmd, Am_Object &owner)) {
  Am_String ls = owner.Get(Am_TEXT);
  Am_String vs = owner.Get(Lw_NAME);
  Name_And_Label_Window.Get_Object(NAME_OBJ).Set(Am_VALUE, vs);
  Name_And_Label_Window.Get_Object(LABEL_OBJ).Set(Am_VALUE, ls);
  Am_Value ok;

  Am_Pop_Up_Window_And_Wait(Name_And_Label_Window, ok, true);
  if (ok.Valid()) {
    Am_String new_vs =Name_And_Label_Window.Get_Object(NAME_OBJ).Get(Am_VALUE);
    fix_lw_name(new_vs);
    owner.Set(Lw_NAME, new_vs);
    Am_String new_ls=Name_And_Label_Window.Get_Object(LABEL_OBJ).Get(Am_VALUE);
    owner.Set(Am_TEXT, new_ls);
    cmd.Set(Am_OBJECT_MODIFIED, owner);
    cmd.Set(Am_SLOTS_TO_SAVE, Am_Value_List().Add(Lw_NAME).Add(Am_TEXT),
	    Am_OK_IF_NOT_THERE);
    cmd.Set(Am_OLD_VALUE, Am_Value_List().Add(vs).Add(ls));
    cmd.Set(Am_VALUE, Am_Value_List().Add(new_vs).Add(new_ls));
  }
  else Am_Abort_Widget(cmd); //keep out of undo history
}

Am_Define_Method(Am_Customize_Object_Method, void, customize_name_label,
		 (Am_Object &cmd, Am_Object &owner)) {
  Am_String ls = owner.Get_Object(Am_COMMAND).Get(Am_LABEL);
  Am_String vs = owner.Get(Lw_NAME);
  Name_And_Label_Window.Get_Object(NAME_OBJ).Set(Am_VALUE, vs);
  Name_And_Label_Window.Get_Object(LABEL_OBJ).Set(Am_VALUE, ls);
  Am_Value ok;

  Am_Pop_Up_Window_And_Wait(Name_And_Label_Window, ok, true);
  if (ok.Valid()) {
    Am_String new_vs= Name_And_Label_Window.Get_Object(NAME_OBJ).Get(Am_VALUE);
    fix_lw_name(new_vs);
    owner.Set(Lw_NAME, new_vs);
    Am_String new_ls=Name_And_Label_Window.Get_Object(LABEL_OBJ).Get(Am_VALUE);
    owner.Get_Object(Am_COMMAND).Set(Am_LABEL, new_ls);
    owner.Set(SUB_LABEL, new_ls);
    cmd.Set(Am_OBJECT_MODIFIED, owner);
    cmd.Set(Am_SLOTS_TO_SAVE, Am_Value_List().Add(Lw_NAME).Add(SUB_LABEL),
	    Am_OK_IF_NOT_THERE);
    cmd.Set(Am_OLD_VALUE, Am_Value_List().Add(vs).Add(ls));
    cmd.Set(Am_VALUE, Am_Value_List().Add(new_vs).Add(new_ls));
  }
  else Am_Abort_Widget(cmd); //keep out of undo history
}

Am_Define_Method(Am_Customize_Object_Method, void, customize_number_input,
		 (Am_Object &cmd, Am_Object &owner)) {
  Am_String ls = owner.Get_Object(Am_COMMAND).Get(Am_LABEL);
  Am_String vs = owner.Get(Lw_NAME);
  Number_Input_Window.Get_Object(NAME_OBJ).Set(Am_VALUE, vs);
  Number_Input_Window.Get_Object(LABEL_OBJ).Set(Am_VALUE, ls);
  Am_Value old_val1 = owner.Get(Am_VALUE_1);
  Am_Value old_val2 = owner.Get(Am_VALUE_2);
  bool restricted = old_val1.Exists();
  int value1 = 0;
  int value2 = 0;
  if (restricted) {
    Number_Input_Window.Get_Object(RESTRICTED).Set(Am_VALUE, Am_Value_List().Add(1));
    value1 = old_val1;
    value2 = owner.Get(Am_VALUE_2);
  }
  else Number_Input_Window.Get_Object(RESTRICTED)
    .Set(Am_VALUE, Am_Value_List());
  Number_Input_Window.Get_Object(MIN_VALUE).Set(Am_VALUE, value1);
  Number_Input_Window.Get_Object(MAX_VALUE).Set(Am_VALUE, value2);

  Am_Value ok;
  Am_Pop_Up_Window_And_Wait(Number_Input_Window, ok, true);
  if (ok.Valid()) {
    Am_String new_vs = Number_Input_Window.Get_Object(NAME_OBJ).Get(Am_VALUE);
    fix_lw_name(new_vs);
    owner.Set(Lw_NAME, new_vs);
    Am_String new_ls = Number_Input_Window.Get_Object(LABEL_OBJ).Get(Am_VALUE);
    owner.Get_Object(Am_COMMAND).Set(Am_LABEL, new_ls);
    owner.Set(SUB_LABEL, new_ls);
    restricted = Number_Input_Window.Get_Object(MAX_VALUE).Get(Am_ACTIVE);
    Am_Value val1 = Am_No_Value;
    Am_Value val2 = Am_No_Value;
    if (restricted) {
      val1 = Number_Input_Window.Get_Object(MIN_VALUE).Get(Am_VALUE);
      val2 = Number_Input_Window.Get_Object(MAX_VALUE).Get(Am_VALUE);
    }
    owner.Set(Am_VALUE_1, val1);
    owner.Set(Am_VALUE_2, val2);

    cmd.Set(Am_OBJECT_MODIFIED, owner);
    cmd.Set(Am_SLOTS_TO_SAVE, Am_Value_List().Add(Lw_NAME).Add(SUB_LABEL)
	    .Add(Am_VALUE_1).Add(Am_VALUE_2), Am_OK_IF_NOT_THERE);
    cmd.Set(Am_OLD_VALUE, Am_Value_List().Add(vs).Add(ls).Add(old_val1)
	    .Add(old_val2));
    cmd.Set(Am_VALUE, Am_Value_List().Add(new_vs).Add(new_ls).Add(val1)
	    .Add(val2));
  }
  else Am_Abort_Widget(cmd); //keep out of undo history
}

Am_Define_Method(Am_Customize_Object_Method, void, customize_border_rect,
		 (Am_Object &cmd, Am_Object &owner)) {
  Am_String vs = owner.Get(Lw_NAME);
  Border_Rect_Window.Get_Object(NAME_OBJ).Set(Am_VALUE, vs);
  bool selected = owner.Get(Am_SELECTED);
  if (selected) {
    Border_Rect_Window.Get_Object(LOOKS_SELECTED_OBJ)
      .Set(Am_VALUE, Am_Value_List().Add(1));
  }
  else Border_Rect_Window.Get_Object(LOOKS_SELECTED_OBJ)
    .Set(Am_VALUE, Am_Value_List());
  
  Am_Value ok;
  Am_Pop_Up_Window_And_Wait(Border_Rect_Window, ok, true);
  if (ok.Valid()) {
    Am_String new_vs = Border_Rect_Window.Get_Object(NAME_OBJ).Get(Am_VALUE);
    fix_lw_name(new_vs);
    owner.Set(Lw_NAME, new_vs);
    Am_Value_List l = Border_Rect_Window.Get_Object(LOOKS_SELECTED_OBJ)
      .Get(Am_VALUE);
    l.Start();
    bool new_selected = false;
    if (l.Member(1)) new_selected = true;
    owner.Set(Am_SELECTED, new_selected);

    cmd.Set(Am_OBJECT_MODIFIED, owner);
    cmd.Set(Am_SLOTS_TO_SAVE, Am_Value_List().Add(Lw_NAME).Add(Am_SELECTED),
	    Am_OK_IF_NOT_THERE);
    cmd.Set(Am_OLD_VALUE, Am_Value_List().Add(vs).Add(selected));
    cmd.Set(Am_VALUE, Am_Value_List().Add(new_vs).Add(new_selected));
  }
  else Am_Abort_Widget(cmd); //keep out of undo history
}

Am_Define_Method(Am_Customize_Object_Method, void, customize_scroll_group,
		 (Am_Object &cmd, Am_Object &owner)) {
  Am_String vs = owner.Get(Lw_NAME);
  int iw = owner.Get(Am_INNER_WIDTH);
  int ih = owner.Get(Am_INNER_HEIGHT);
  bool vsb =  owner.Get(Am_V_SCROLL_BAR);
  bool hsb =  owner.Get(Am_H_SCROLL_BAR);
  bool sbt =  owner.Get(Am_H_SCROLL_BAR_ON_TOP);
  bool sbl =  owner.Get(Am_V_SCROLL_BAR_ON_LEFT);

  Scroll_Group_Window.Get_Object(NAME_OBJ).Set(Am_VALUE, vs);
  Scroll_Group_Window.Get_Object(Am_INNER_WIDTH).Set(Am_VALUE, iw);
  Scroll_Group_Window.Get_Object(Am_INNER_HEIGHT).Set(Am_VALUE, ih);

  Am_Value_List l;
  if (vsb) l.Add(1);
  if (hsb) l.Add(2);
  if (sbt) l.Add(3);
  if (sbl) l.Add(4);
  Scroll_Group_Window.Get_Object(SCROLL_PROPS_OBJ).Set(Am_VALUE, l);

  Am_Value ok;
  Am_Pop_Up_Window_And_Wait(Scroll_Group_Window, ok, true);
  if (ok.Valid()) {
    Am_String new_vs = Scroll_Group_Window.Get_Object(NAME_OBJ).Get(Am_VALUE);
    int new_iw = Scroll_Group_Window.Get_Object(Am_INNER_WIDTH).Get(Am_VALUE);
    int new_ih = Scroll_Group_Window.Get_Object(Am_INNER_HEIGHT).Get(Am_VALUE);
    l = Scroll_Group_Window.Get_Object(SCROLL_PROPS_OBJ).Get(Am_VALUE);
    l.Start();
    bool new_vsb = l.Member(1);
    l.Start();
    bool new_hsb = l.Member(2);
    l.Start();
    bool new_sbt = l.Member(3);
    l.Start();
    bool new_sbl = l.Member(4);

    fix_lw_name(new_vs);
    owner.Set(Lw_NAME, new_vs);
    owner.Set(Am_INNER_WIDTH, new_iw);
    owner.Set(Am_INNER_HEIGHT, new_ih);
    owner.Set(Am_V_SCROLL_BAR, new_vsb);
    owner.Set(Am_H_SCROLL_BAR, new_hsb);
    owner.Set(Am_H_SCROLL_BAR_ON_TOP, new_sbt);
    owner.Set(Am_V_SCROLL_BAR_ON_LEFT, new_sbl);

    cmd.Set(Am_OBJECT_MODIFIED, owner);
    cmd.Set(Am_SLOTS_TO_SAVE, Am_Value_List().Add(Lw_NAME)
	    .Add(Am_INNER_WIDTH).Add(Am_INNER_HEIGHT).Add(Am_V_SCROLL_BAR)
	    .Add(Am_H_SCROLL_BAR).Add(Am_H_SCROLL_BAR_ON_TOP)
	    .Add(Am_V_SCROLL_BAR_ON_LEFT), Am_OK_IF_NOT_THERE);
    cmd.Set(Am_OLD_VALUE, Am_Value_List().Add(vs).Add(iw).Add(ih).Add(vsb)
	    .Add(hsb).Add(sbt).Add(sbl));
    cmd.Set(Am_VALUE, Am_Value_List().Add(new_vs).Add(new_iw).Add(new_ih)
	    .Add(new_vsb).Add(new_hsb).Add(new_sbt).Add(new_sbl));
  }
  else Am_Abort_Widget(cmd); //keep out of undo history
}

Am_Define_Method(Am_Customize_Object_Method, void, customize_panels,
		 (Am_Object &cmd, Am_Object &owner)) {
  owner.Make_Unique(Am_ITEMS);
  Am_String name = owner.Peek(Lw_NAME);
  Am_Value_List il = ((Am_Value_List)owner.Get(Am_ITEMS));
  Am_Value layout_key = owner.Peek(LAYOUT_KEY);
  Am_Value box_on_left = owner.Peek(Am_BOX_ON_LEFT);
  Am_Value fw =  owner.Peek(Am_FIXED_WIDTH);
  Am_Value hspace, vspace, maxrank;
  Am_Value_List slots_to_save = owner.Get(Am_SLOTS_TO_SAVE);
  slots_to_save.Start();
  if (slots_to_save.Member(Am_H_SPACING))
    hspace = owner.Get(Am_H_SPACING);
  slots_to_save.Start();
  if (slots_to_save.Member(Am_V_SPACING))
    vspace = owner.Get(Am_V_SPACING);
  slots_to_save.Start();
  if (slots_to_save.Member(Am_MAX_RANK))
    maxrank = owner.Get(Am_MAX_RANK);
  slots_to_save.Start();
  bool can_edit_items = slots_to_save.Member(Am_ITEMS);

  Am_String old_name = name;
  Am_Value_List old_il = il;
  Am_Value old_layout_key = layout_key;
  Am_Value old_box_on_left = box_on_left;
  Am_Value old_fw = fw;
  Am_Value old_hspace = hspace;
  Am_Value old_vspace = vspace;
  Am_Value old_maxrank = maxrank;
  if(paneldlg.Execute(name, il, layout_key, box_on_left, fw, hspace, vspace,
		      maxrank, can_edit_items)) {
    fix_lw_name(name);
    owner.Set(Lw_NAME, name)
      .Set(Am_FIXED_WIDTH, fw)
      ;
    if (can_edit_items) {
      owner.Set(Am_ITEMS,il);
      owner.Note_Changed(Am_ITEMS);
    }
    if (layout_key.Exists()) {
      if ((int)layout_key == 1)
	owner.Set(LAYOUT_KEY, 1).Set(Am_LAYOUT, Am_Horizontal_Layout);
      else 
	owner.Set(LAYOUT_KEY, 0).Set(Am_LAYOUT, Am_Vertical_Layout);
    }
    if (hspace.Exists()) owner.Set(Am_H_SPACING, hspace);
    if (vspace.Exists()) owner.Set(Am_V_SPACING, vspace);
    if (maxrank.Exists()) owner.Set(Am_MAX_RANK, maxrank);

    cmd.Set(Am_OBJECT_MODIFIED, owner);
    Am_Value_List sl = Am_Value_List().Add(Lw_NAME)
      .Add(Am_FIXED_WIDTH).Add(LAYOUT_KEY)
      .Add(Am_H_SPACING).Add(Am_V_SPACING).Add(Am_MAX_RANK);
    Am_Value_List oldl = Am_Value_List().Add(old_name)
      .Add(old_fw).Add(old_layout_key)
      .Add(old_hspace).Add(old_vspace).Add(old_maxrank);
    Am_Value_List newl = Am_Value_List().Add(name)
      .Add(fw).Add(layout_key).Add(hspace).Add(vspace).Add(maxrank);
    if (can_edit_items) {
      sl.Add(Am_ITEMS);
      oldl.Add(old_il);
      newl.Add(il);
    }
    cmd.Set(Am_SLOTS_TO_SAVE, sl, Am_OK_IF_NOT_THERE);
    cmd.Set(Am_OLD_VALUE, oldl);
    cmd.Set(Am_VALUE, newl);
  }
  else Am_Abort_Widget(cmd); //keep out of undo history
}

Am_Define_Method(Am_Object_Method, void, undo_set_properties,
		 (Am_Object cmd)) {
  Am_Value_List old_values = cmd.Get(Am_OLD_VALUE);
  Am_Value_List new_values = cmd.Get(Am_VALUE);
  Am_Value_List slots = cmd.Get(Am_SLOTS_TO_SAVE);
  Am_Object object = cmd.Get(Am_OBJECT_MODIFIED);
  
  Am_Slot_Key slot;
  Am_Value old_val;
  for (slots.Start (), old_values.Start() ; !slots.Last ();
       slots.Next(), old_values.Next()) {
    slot = (int)slots.Get();
    old_val = old_values.Get();
    
    object.Set(slot, old_val);    
    // some slots need special handling as well
    if (slot == Am_ITEMS) { 
      object.Note_Changed(Am_ITEMS);
    }
    else if (slot == LAYOUT_KEY) {
      if ((int)old_val == 1)
	object.Set(Am_LAYOUT, Am_Horizontal_Layout);
      else 
	object.Set(Am_LAYOUT, Am_Vertical_Layout);
    }
    else if (slot == SUB_LABEL) {
      object.Get_Object(Am_COMMAND).Set(Am_LABEL, old_val);
    }
  }
  //now swap old and new in case redo
  cmd.Set(Am_VALUE, old_values);
  cmd.Set(Am_OLD_VALUE, new_values);
}

Am_Object create_new_obj_internal (Am_Object &prototype,
				   Am_Inter_Location & data) {
  bool set_width_height = (int)prototype.Get(Am_HOW_MANY_POINTS) == 2;

  Am_Object ref_obj;
  int a, b, c, d;
  bool create_line;
  data.Get_Location(create_line, ref_obj, a, b, c, d);
  if (ref_obj != workgroup)
  {
    Am_Translate_Coordinates(ref_obj, a, b, workgroup, a, b);
    if (create_line)
      Am_Translate_Coordinates(ref_obj, c, d, workgroup, c, d);
  }

  /*
  Am_Object fill_color_obj = fill_color_panel.Get(Am_VALUE);
  Am_Style fill_color = fill_color_obj.Get(Am_FILL_STYLE);
  
  Am_Object line_color_obj = line_color_panel.Get(Am_VALUE);
  Am_Style line_color = line_color_obj.Get(Am_FILL_STYLE);
  */

  Am_Object new_obj;
  if (create_line)
    new_obj = prototype.Create ()
      // .Set (Am_LINE_STYLE, line_color)
      .Set (Am_X1, a)
      .Set (Am_Y1, b)
      .Set (Am_X2, c)
      .Set (Am_Y2, d)
      .Set (Lw_NAME,"")	// line_proto has this slot
      ;
  else if (prototype.Valid()) {
    new_obj = prototype.Create ()
      // .Set (Am_LINE_STYLE, line_color)
      // .Set (Am_FILL_STYLE, fill_color)
      .Set (Am_LEFT, a)
      .Set (Am_TOP, b)
      .Set (Lw_NAME,"")
      ;
    if (set_width_height) {
      new_obj
	.Set (Am_WIDTH, c)
	.Set (Am_HEIGHT, d)
	;
    }
  }
  else Am_Error("No Prototype");

  workgroup.Add_Part (new_obj);
  tool_panel.Set(Am_VALUE, ARROW_ID); //return to selection after each create
  return new_obj;
}

// Am_Create_New_Object_Proc for new object command
Am_Define_Method(Am_Create_New_Object_Method, Am_Object, create_new_object,
                 (Am_Object /* inter */, Am_Inter_Location data,
                  Am_Object old_object)) {
  Am_Object new_obj;
  if (old_object.Valid()) {
    Am_Error("Should not be an old object since no selective redo");
    /* 
    new_obj = old_object.Copy();
    workgroup.Add_Part (new_obj);
    */
    }
  else {
    Am_Object proto = tool_panel.Get(Am_VALUE);
    new_obj = create_new_obj_internal (proto, data);
  }
  return new_obj;
}

void Create_Tool_Panel() {
  //don't want changing the mode to go onto the undo history
  Am_Object undoable_cmd = Am_Command.Create()
    .Set(Am_IMPLEMENTATION_PARENT, true);
  Am_Value_List l;
  l.Add(undoable_cmd.Create()
	.Set(Am_LABEL, arrow_bitmap)
	.Set(Am_ID, ARROW_ID))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Run")
	 .Set(Am_DO_METHOD, go_into_run_mode)
	 .Set(Am_ID, RUN_ID))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, Am_Rectangle.Create()
	      .Set(Am_FILL_STYLE, Am_Red)
	      .Set(Am_LINE_STYLE, Am_Black)
	      .Set(Am_WIDTH, 30)
	      .Set(Am_HEIGHT, 15))
	 .Set(Am_ID, rectangle_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, Am_Line.Create()
	      .Set(Am_LINE_STYLE, Am_Red)
	      .Set(Am_X1, 0)
	      .Set(Am_Y1, 0)
	      .Set(Am_X2, 30)
	      .Set(Am_Y2, 15))
	 .Set(Am_ID, line_proto));
  l.Add(undoable_cmd.Create()
	.Set(Am_LABEL, Am_Arc.Create()
	     .Set(Am_FILL_STYLE, Am_Yellow)
	     .Set(Am_LINE_STYLE, Am_Black)
	     .Set(Am_WIDTH, 20)
	     .Set(Am_HEIGHT, 10))
	.Set(Am_ID, arc_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, Am_Polygon.Create()
	      .Set (Am_POINT_LIST, Am_Point_List()
		    // an irregular closed polygon
		    .Add(15.0f,0.0f).Add(0.0f,20.0f).Add(30.0f,40.0f)
		    .Add(30.0f,20.0f).Add(15.0f,20.0f).Add(10.0f,0.0f))
	      .Set (Am_WIDTH, 16)
	      .Set (Am_HEIGHT, 10)
	      .Set (Am_LINE_STYLE, Am_Black)
	      .Set (Am_FILL_STYLE, Am_Green))
	 .Set(Am_ID, POLYGON_ID)
	 )
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Text")
	 .Set(Am_ID, text_proto));
  l.Add(undoable_cmd.Create()
	.Set(Am_LABEL, "Border")
	.Set(Am_ID, border_rectangle_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Button")
	 .Set(Am_ID, button_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Buttons")
	 .Set(Am_ID, buttons_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "OK-Cancel")
	 .Set(Am_ID, ok_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Option")
	 .Set(Am_ID, option_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Checkboxes")
	 .Set(Am_ID, checkbox_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Radios")
	 .Set(Am_ID, radio_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Text Input")
	 .Set(Am_ID, text_input_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Number Input")
	 .Set(Am_ID, number_input_proto))
    .Add(undoable_cmd.Create()
	 .Set(Am_LABEL, "Scroll Group")
	 .Set(Am_ID, scroll_group_proto))
    ;

  tool_panel = Am_Button_Panel.Create ("tool panel")
     .Set (Am_LEFT, 10)
     .Set (Am_TOP, 40)
     .Set (Am_FIXED_HEIGHT, false)
     .Set (Am_FIXED_WIDTH, false)
     .Set (Am_ITEM_OFFSET, -2)
     .Set (Am_FINAL_FEEDBACK_WANTED, true)
     .Set (Am_ITEMS, l)
     .Set (Am_H_SPACING, 2)
     .Set (Am_LAYOUT, Am_Horizontal_Layout)
     .Set (Am_MAX_SIZE, 100)
     .Set (Am_VALUE, ARROW_ID);
  Am_Value_List parts = tool_panel.Get(Am_GRAPHICAL_PARTS);
  parts.Start();
  Am_Object obj, label_obj;
  for (int i = 0; i < 6; i++) {
    obj = parts.Get();
    obj.Set(Am_WIDTH, 31);
    obj.Set(Am_HEIGHT, 20);
    parts.Next();
  }
}

#if defined(_WINDOWS) || defined(_MACINTOSH)
#define ARROW_BITMAP "lib/images/arrow.gif"
#else
#define ARROW_BITMAP "lib/images/arrow.xbm"
#endif

void Create_Prototypes() {
  char* pathname = Am_Merge_Pathname(ARROW_BITMAP);
  Am_Image_Array picture = Am_Image_Array(pathname);
  delete [] pathname;
  if (!picture.Valid()) Am_Error ("Arrow bitmap image not found");
  arrow_bitmap = Am_Bitmap.Create("Arrow bitmap")
    .Set (Am_IMAGE, picture)
    .Set (Am_LINE_STYLE, Am_Black);
  arc_proto = Am_Arc.Create("Circle_Proto")
    .Add (CUSTOMIZE_METHOD, customize_name_only)
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Arc")
    .Add (Am_HOW_MANY_POINTS, 2)
    .Add (Am_MINIMUM_WIDTH, 10)
    .Add (Am_MINIMUM_HEIGHT, 10)
    .Set (Am_FILL_STYLE, Am_No_Style).Add (FILL_STYLE_KEY, 0)
    .Set (Am_LINE_STYLE, Am_Black).Add (LINE_STYLE_KEY,8)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Lw_NAME).Add(Am_LEFT).Add(Am_TOP).Add(Am_WIDTH).Add(Am_HEIGHT)
	  .Add(LINE_STYLE_KEY) .Add(FILL_STYLE_KEY)
	  )
    ;
  rectangle_proto = Am_Rectangle.Create("Rectangle_Proto")
    .Add (CUSTOMIZE_METHOD, customize_name_only)
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Rectangle")
    .Add (Am_HOW_MANY_POINTS, 2)
    .Add (Am_MINIMUM_WIDTH, 10)
    .Add (Am_MINIMUM_HEIGHT, 10)
    .Set (Am_FILL_STYLE, Am_No_Style).Add (FILL_STYLE_KEY, 0)
    .Set (Am_LINE_STYLE, Am_Black).Add (LINE_STYLE_KEY,8)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Lw_NAME).Add(Am_LEFT).Add(Am_TOP).Add(Am_WIDTH).Add(Am_HEIGHT)
	  .Add(LINE_STYLE_KEY) .Add(FILL_STYLE_KEY)
	  )
    ;
  polygon_proto = Am_Polygon.Create("Polygon_Proto")
    .Add (CUSTOMIZE_METHOD, customize_name_only)
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Polygon")
    .Set (Am_FILL_STYLE, Am_No_Style).Add (FILL_STYLE_KEY, 0)
    .Set (Am_LINE_STYLE, Am_Black).Add (LINE_STYLE_KEY,8)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Lw_NAME).Add(Am_POINT_LIST)
	  .Add(LINE_STYLE_KEY) .Add(FILL_STYLE_KEY)
	  )
    ;
  line_proto = Am_Line.Create("Line_Proto")
    .Add (CUSTOMIZE_METHOD, customize_name_only)
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Line")
    .Set (Am_LINE_STYLE, Am_Black).Add (LINE_STYLE_KEY,8)
    .Add (Am_MINIMUM_LENGTH, 10)
    .Add (Am_HOW_MANY_POINTS, 2)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Lw_NAME).Add(Am_X1).Add(Am_Y1).Add(Am_X2).Add(Am_Y2)
	  .Add(LINE_STYLE_KEY)
	  )
    ;
  text_proto = Am_Text.Create("Text_Proto")
    .Add (CUSTOMIZE_METHOD, customize_text)
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Text")
    .Set (Am_TEXT, "Text")
    .Add (FONT_KEY, 4)
    .Set (Am_FILL_STYLE, Am_No_Style).Add (FILL_STYLE_KEY, 0)
    .Set (Am_LINE_STYLE, Am_Black).Add (LINE_STYLE_KEY,8)
    .Add (Am_HOW_MANY_POINTS, 1)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Lw_NAME).Add(Am_LEFT).Add(Am_TOP).Add(Am_WIDTH).Add(Am_HEIGHT)
	  .Add(Am_TEXT).Add(FONT_KEY)
	  .Add(LINE_STYLE_KEY) .Add(FILL_STYLE_KEY)
	  )
    ;
  border_rectangle_proto = Am_Border_Rectangle.Create("Border_Rectangle_Proto")
    .Add (CUSTOMIZE_METHOD, customize_border_rect)
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Border_Rectangle")
    .Add (Am_HOW_MANY_POINTS, 2)
    .Add (Am_MINIMUM_WIDTH, 10)
    .Add (Am_MINIMUM_HEIGHT, 10)
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Lw_NAME).Add(Am_LEFT).Add(Am_TOP).Add(Am_WIDTH).Add(Am_HEIGHT)
	  .Add(Am_SELECTED) .Add(FILL_STYLE_KEY)
	  )
    ;
  button_proto = Am_Button.Create("Button_Proto")
    .Set (Am_ACTIVE_2, run_tool)
    .Add (CUSTOMIZE_METHOD, customize_name_label)
    .Add (Am_HOW_MANY_POINTS, 2)
    .Add (FONT_KEY, 4)
    .Add (SUB_LABEL, Am_Button.Get_Object(Am_COMMAND).Get(Am_LABEL))
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Button")
    .Add (Am_MINIMUM_WIDTH, 66)
    .Add (Am_MINIMUM_HEIGHT, 40)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP).Add(Am_WIDTH).Add(Am_HEIGHT)
	  .Add(FONT_KEY).Add(FILL_STYLE_KEY)
	  .Add(SUB_LABEL).Add(Lw_NAME))
    ;
  buttons_proto = Am_Button_Panel.Create("Buttons_Proto")
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Button_Panel")
    .Add (LAYOUT_KEY, 0) .Set(Am_LAYOUT, Am_Vertical_Layout)
    .Add (FONT_KEY, 4)
    .Add (CUSTOMIZE_METHOD, customize_panels)
    .Set (Am_ACTIVE_2, run_tool)
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Set (Am_ITEMS, Am_Value_List()
	  .Add("Button 1").Add("Button 2"))
    .Add (Am_HOW_MANY_POINTS, 1)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP)
	  .Add(FONT_KEY).Add(FILL_STYLE_KEY).Add(Am_ITEMS)
	  .Add(LAYOUT_KEY).Add(Lw_NAME)
	  .Add(Am_H_SPACING) .Add(Am_V_SPACING) .Add(Am_MAX_RANK)
	  )
    ;
  ok_proto = Am_Button_Panel.Create("OK_Proto")
    .Add (Lw_NAME, "")
    .Add (CUSTOMIZE_METHOD, customize_panels)
    .Add (TYPE_NAME, "Am_Button_Panel")
    .Add (LAYOUT_KEY, 1) .Set(Am_LAYOUT, Am_Horizontal_Layout)
    .Add (FONT_KEY, 4)
    .Set (Am_ACTIVE_2, run_tool)
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Set (Am_ITEMS, Am_Value_List()
	  .Add(Am_Standard_OK_Command.Create()
	       .Set(Am_DO_METHOD, fake_do_cancel)
	       .Set(Am_ACCELERATOR, NULL)) //get rid of the accelerator here
	  .Add(Am_Standard_Cancel_Command.Create()
	       .Set(Am_DO_METHOD, fake_do_cancel)
	       )
	  )
    .Add (SAVE_COMMAND_ITEMS, true)
    .Add (Am_HOW_MANY_POINTS, 1)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP)
	  .Add(FONT_KEY).Add(FILL_STYLE_KEY)
	  .Add(LAYOUT_KEY).Add(Lw_NAME)
	  .Add(Am_H_SPACING) .Add(Am_V_SPACING) .Add(Am_MAX_RANK)
	  .Add(SAVE_COMMAND_ITEMS)
	  );
  checkbox_proto = Am_Checkbox_Panel.Create("Checkbox_Proto")
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Checkbox_Panel")
    .Add (LAYOUT_KEY, 0) .Set(Am_LAYOUT, Am_Vertical_Layout)
    .Add (FONT_KEY, 4)
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Add (CUSTOMIZE_METHOD, customize_panels)
    .Set (Am_ACTIVE_2, run_tool)
    .Set (Am_ITEMS, Am_Value_List()
	  .Add("Checkbox 1").Add("Checkbox 2"))
    .Add (Am_HOW_MANY_POINTS, 1)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP)
	  .Add(FONT_KEY).Add(FILL_STYLE_KEY).Add(Am_ITEMS)
	  .Add(LAYOUT_KEY).Add(Lw_NAME)
	  .Add(Am_H_SPACING) .Add(Am_V_SPACING) .Add(Am_MAX_RANK)
	  );    
  radio_proto = Am_Radio_Button_Panel.Create("Radio_Proto")
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Radio_Button_Panel")
    .Add (LAYOUT_KEY, 0) .Set(Am_LAYOUT, Am_Vertical_Layout)
    .Add (FONT_KEY, 4)
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Add (CUSTOMIZE_METHOD, customize_panels)
    .Set (Am_ACTIVE_2, run_tool)
    .Set (Am_ITEMS, Am_Value_List()
	   .Add("Radio 1").Add("Radio 2"))
    .Add (Am_HOW_MANY_POINTS, 1)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP)
	  .Add(FONT_KEY).Add(FILL_STYLE_KEY).Add(Am_ITEMS)
	  .Add(LAYOUT_KEY).Add(Lw_NAME)
	  .Add(Am_H_SPACING) .Add(Am_V_SPACING) .Add(Am_MAX_RANK)
	  );    
  option_proto = Am_Option_Button.Create("Option_Proto")
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Option_Button")
    .Add (FONT_KEY, 4)
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Set (Am_ACTIVE_2, run_tool)
    .Add (CUSTOMIZE_METHOD, customize_panels)
    .Set (Am_ITEMS, Am_Value_List()
	   .Add("Option 1").Add("Option 2"))
    .Add (Am_HOW_MANY_POINTS, 1)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP)
	  .Add(FONT_KEY).Add(FILL_STYLE_KEY).Add(Am_ITEMS)
	  .Add(Lw_NAME))
    ;
  text_input_proto = Am_Text_Input_Widget.Create("Text_Input_Proto")
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Text_Input_Widget")
    .Add (CUSTOMIZE_METHOD, customize_name_label)
    .Add (FONT_KEY, 4)
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Set (Am_ACTIVE_2, run_tool)
    .Add (Am_HOW_MANY_POINTS, 2)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_MINIMUM_WIDTH, 115)
    .Add (Am_MINIMUM_HEIGHT, 25)
    .Add (SUB_LABEL, Am_Text_Input_Widget.Get_Object(Am_COMMAND).Get(Am_LABEL))
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP).Add(Am_WIDTH).Add(Am_HEIGHT)
	  .Add(SUB_LABEL).Add(FONT_KEY).Add(FILL_STYLE_KEY)
	  .Add(Lw_NAME))
    ;
  number_input_proto = Am_Number_Input_Widget.Create("Number_Input_Proto")
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Number_Input_Widget")
    .Add (CUSTOMIZE_METHOD, customize_number_input)
    .Add (FONT_KEY, 4)
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Add (Am_MINIMUM_WIDTH, 115)
    .Add (Am_MINIMUM_HEIGHT, 25)
    .Add (SUB_LABEL, Am_Number_Input_Widget.Get_Object(Am_COMMAND)
	  .Get(Am_LABEL))
    .Set (Am_ACTIVE_2, run_tool)
    .Add (Am_HOW_MANY_POINTS, 2)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP).Add(Am_WIDTH).Add(Am_HEIGHT)
	  .Add(FONT_KEY).Add(FILL_STYLE_KEY)
	  .Add(SUB_LABEL).Add(Lw_NAME) .Add(Am_VALUE_1).Add(Am_VALUE_2))
    ;
  scroll_group_proto = Am_Scrolling_Group.Create("Scroll_Group_Proto")
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Scrolling_Group")
    .Set (Am_ACTIVE_2, run_tool)
    .Add (CUSTOMIZE_METHOD, customize_scroll_group)
    .Add (Am_MINIMUM_WIDTH, 75)
    .Add (Am_MINIMUM_HEIGHT, 75)
    .Add (Am_HOW_MANY_POINTS, 2)
    .Add (FILL_STYLE_KEY, 10) .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP).Add(Am_WIDTH).Add(Am_HEIGHT)
	  .Add(FILL_STYLE_KEY).Add(Am_INNER_WIDTH).Add(Am_INNER_HEIGHT)
	  .Add(Am_H_SCROLL_BAR).Add(Am_V_SCROLL_BAR)
	  .Add(Am_H_SCROLL_BAR_ON_TOP).Add(Am_V_SCROLL_BAR_ON_LEFT)
	  .Add(Lw_NAME))
    ;

  main_group_rect_proto = Am_Rectangle.Create("main_group_rect_proto")
    .Add(FILL_STYLE_KEY, 9)
    .Add(MAIN_NAME, "")
    .Add(C_FILENAME, "")
    .Add(CREATE_HEADER, true)
    .Add(H_FILENAME, "")
    .Add(WINDOW_OR_GROUP, true)
    .Add(WIN_TITLE, "Amulet")
    .Add(FIXED_SIZE_OBJ, false)
    .Add(EXPLICIT_SIZE_OBJ, false)
    .Add(WIDTH_OBJ, 0)
    .Add(HEIGHT_OBJ, 0)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(MAIN_NAME) .Add(C_FILENAME) .Add(CREATE_HEADER)
	  .Add(H_FILENAME) .Add(WINDOW_OR_GROUP) .Add(WIN_TITLE)
	  .Add(FIXED_SIZE_OBJ) .Add(EXPLICIT_SIZE_OBJ) .Add(WIDTH_OBJ)
	  .Add(HEIGHT_OBJ) .Add(FILL_STYLE_KEY)
	  );

  // .Add(Lw_NAME) to list of slots to save and load.  Unlike other
  // objects, Am_Resize_Parts_Group already has a Am_SLOTS_TO_SAVE list.
  Am_Value_List slots_to_save = Am_Resize_Parts_Group.Get(Am_SLOTS_TO_SAVE);
  slots_to_save.Add(Lw_NAME);
  Am_Resize_Parts_Group
    .Add (CUSTOMIZE_METHOD, customize_name_only)
    .Add (Lw_NAME, "")
    .Add (TYPE_NAME, "Am_Group")
    .Set (Am_SLOTS_TO_SAVE, slots_to_save)
    ;
  
  //tell the default loader what to call all of the prototypes in the file
  Am_Default_Load_Save_Context.Register_Prototype("ARC", arc_proto);
  Am_Default_Load_Save_Context.Register_Prototype("RECT", rectangle_proto);
  Am_Default_Load_Save_Context.Register_Prototype("LIN", line_proto);
  Am_Default_Load_Save_Context.Register_Prototype("POLY", polygon_proto);

  Am_Default_Load_Save_Context.Register_Prototype("TEXT", text_proto);
  Am_Default_Load_Save_Context.Register_Prototype("BORDER",
						  border_rectangle_proto);
  Am_Default_Load_Save_Context.Register_Prototype("BUTTON", button_proto);
  Am_Default_Load_Save_Context.Register_Prototype("BUTTONS", buttons_proto);
  Am_Default_Load_Save_Context.Register_Prototype("OK", ok_proto);
  Am_Default_Load_Save_Context.Register_Prototype("CHECKBOX", checkbox_proto);
  Am_Default_Load_Save_Context.Register_Prototype("RADIO", radio_proto);
  Am_Default_Load_Save_Context.Register_Prototype("TEXT_INPUT",
						  text_input_proto);
  Am_Default_Load_Save_Context.Register_Prototype("NUMBER_INPUT",
						  number_input_proto);
  Am_Default_Load_Save_Context.Register_Prototype("OPTION", option_proto);
  Am_Default_Load_Save_Context.Register_Prototype("SCROLL_GROUP",
						  scroll_group_proto);
  Am_Default_Load_Save_Context.Register_Prototype("MAIN_GROUP",
						  main_group_rect_proto);
}

Am_Define_Formula (bool, look_is_me) {
  Am_Value my_look = self.Get(Am_WIDGET_LOOK);
  Am_Value current_look = Am_Screen.Get(Am_WIDGET_LOOK);
  return my_look == current_look;
}

Am_Define_Method( Am_Object_Method, void, set_look, (Am_Object cmd) ) {
  Am_Value my_look = cmd.Get(Am_WIDGET_LOOK);
  cout << "Setting look to " << my_look << endl << flush;
  Am_Set_Default_Look(my_look);
}

////////////////////////////////////////////////////////////////////////////
// Polygon stuff
////////////////////////////////////////////////////////////////////////////

Am_Object partial_poly; // polygon under construction

Am_Define_Method(Am_Mouse_Event_Method, void, polygon_start,
                 (Am_Object inter, int /* x */, int /* y */,
                  Am_Object /* event_window */, Am_Input_Char /* ic */)) {
  // start the polygon
  partial_poly = polygon_proto.Create ()
    .Set (Am_POINT_LIST, Am_Point_List());

  workgroup.Add_Part (partial_poly);

  // show feedback
  Am_Object feedback(inter.Get(Am_FEEDBACK_OBJECT));
  if (feedback.Valid ()) {
    feedback.Set(Am_VISIBLE, true);
    Am_To_Top(feedback);
    // feedback position will be set by polygon_interim_do
  }
}

Am_Define_Method(Am_Mouse_Event_Method, void, polygon_interim_do,
                 (Am_Object inter, int x, int y,
                  Am_Object event_window, Am_Input_Char ic)) {
  if (event_window != workgroup)
    Am_Translate_Coordinates (event_window, x, y, workgroup, x, y);

  // move endpoint of feedback line
  Am_Object feedback = inter.Get(Am_FEEDBACK_OBJECT);
  if (feedback.Valid ()) {
    bool feedback_vis = feedback.Get(Am_VISIBLE);
    if (feedback_vis == false) {
      feedback.Set(Am_VISIBLE, true);
    }
    feedback.Set(Am_X2, x);
    feedback.Set(Am_Y2, y);
  }

  static Am_Input_Char vertex_event("any_left_down");
  if (ic == vertex_event) {
    // user has left-clicked!
    if (!Am_Point_In_All_Owners (partial_poly, x, y, workgroup)) {
      // clicked outside of drawing window -- throw away this point and stop
      Am_Stop_Interactor (inter, Am_No_Object, Am_No_Input_Char,
			  Am_No_Object, 0, 0);
    }
    else {
      Am_Point_List pl = partial_poly.Get (Am_POINT_LIST);
      if (pl.Empty()) {
        // the click that started it all -- first point of the polygon
        partial_poly.Set (Am_POINT_LIST, pl.Add((float)x, (float)y));
        feedback.Set(Am_X1, x);
        feedback.Set(Am_Y1, y);
      }
      else {
        int first_x;
        int first_y;
        pl.Start(); pl.Get(first_x, first_y);

        int delta_x = x - first_x;
        int delta_y = y - first_y;

        if (delta_x < 5 && delta_x > -5 && delta_y < 5 && delta_y > -5) {
          // clicked on (er, near) the initial point again -- close
          // the polygon and stop
          pl.Add ((float)first_x, (float)first_y, Am_TAIL, false);
          Am_Stop_Interactor (inter, Am_No_Object, Am_No_Input_Char,
			      Am_No_Object, 0, 0);
        }
        else {
          // add new point to polygon, reset feedback origin to it, and
          // keep running
          pl.Add ((float)x, (float)y, Am_TAIL, false);
          feedback.Set(Am_X1, x);
          feedback.Set(Am_Y1, y);
        }
        partial_poly.Note_Changed (Am_POINT_LIST);
      }
    }
  }
}

Am_Define_Method(Am_Mouse_Event_Method, void, polygon_abort,
                 (Am_Object inter, int x, int y,
                  Am_Object event_window, Am_Input_Char /* ic */))
{
  if( event_window != workgroup )
    Am_Translate_Coordinates( event_window, x, y, workgroup, x, y );

  // hide feedback
  Am_Object feedback;
  feedback = inter.Get( Am_FEEDBACK_OBJECT );
  if( feedback.Valid() )
    feedback.Set(Am_VISIBLE, false);

  // destroy polygon under construction
  partial_poly.Destroy ();
}

Am_Define_Method(Am_Mouse_Event_Method, void, polygon_do,
                 (Am_Object inter, int /*x*/, int /*y*/,
                  Am_Object /*event_window*/, Am_Input_Char /* ic */))
{
  // hide feedback
  Am_Object feedback;
  feedback = inter.Get(Am_FEEDBACK_OBJECT);
  if( feedback.Valid() )
    feedback.Set(Am_VISIBLE, false);

  Am_Object new_object = partial_poly;
  
  /*
  Am_Style color = color_panel.Get_Object(Am_VALUE).Get(Am_FILL_STYLE);
  new_object.Set (Am_FILL_STYLE, color);
  */

  // new_object has already been added to workgroup

  // take care of undo/redo
  inter.Set (Am_VALUE, new_object);
  inter.Set(Am_OBJECT_MODIFIED, new_object);
  Am_Copy_Values_To_Command(inter);

  tool_panel.Set(Am_VALUE, ARROW_ID); //return to selection after each create
}

#define PANEL_WIDTH 105 //width of toolpanel area

int main (int argc, char *argv[])
{
  Am_Initialize(); 
  init_styles();
  Create_Prototypes();
  Create_Tool_Panel();
  Name_Only_Window_Initialize();
  Number_Input_Window_Initialize();
  Border_Rect_Window_Initialize ();
  savecpp_window_Initialize ();
  About_Gilt_Window_Initialize();
  Scroll_Group_Window_Initialize ();
  Name_And_Label_Window_Initialize ();

  rfeedback = Am_Rectangle.Create ("rfeedback")
    .Set (Am_FILL_STYLE, 0)
    .Set (Am_LINE_STYLE, Am_Dotted_Line)
    .Set (Am_VISIBLE, 0)
    ;
  lfeedback = Am_Line.Create ("lfeedback")
    .Set (Am_LINE_STYLE, Am_Dotted_Line)
    .Set (Am_VISIBLE, 0)
    ;
  grid_command = Am_Cycle_Value_Command.Create("grid")
    .Set(Am_LABEL_LIST, Am_Value_List()
	 .Add("Turn Grid On")
	 .Add("Turn Grid Off"));
  
  workgroup = Am_Group.Create("workgroup")
    . Set(Am_LEFT, 0)
    . Set(Am_TOP, 0)
    . Set(Am_WIDTH, 700)
    . Set(Am_HEIGHT, 670)
    .Add_Part(Am_Choice_Interactor.Create()
	      .Set(Am_HOW_SET, Am_CHOICE_SET)
	      //has to be higher than 3 or the sel_widget steals the click
	      .Set(Am_PRIORITY, 5)
	      .Set(Am_START_WHEN, Am_Value_List() //either middle, doubleclick
		   .Add(Am_Input_Char("MIDDLE_DOWN"))
		   .Add(Am_Input_Char("DOUBLE_LEFT_DOWN"))
		   )
	      .Set(Am_SET_SELECTED, false)
	      .Get_Object(Am_COMMAND)
	      .Set(Am_DO_METHOD, customize)
	      .Set(Am_UNDO_METHOD, undo_set_properties)
	      .Set(Am_REDO_METHOD, undo_set_properties)
	      .Get_Owner())
    .Add_Part(Am_New_Points_Interactor.Create("create_inter")
	      .Set(Am_AS_LINE, line_tool)
	      .Set(Am_FEEDBACK_OBJECT, compute_feedback_obj)
	      .Set(Am_CREATE_NEW_OBJECT_METHOD, create_new_object)
	      .Set(Am_ACTIVE, rubber_bandable_tool_is_selected)
	      .Set(Am_GRID_X, grid_if_should)
	      .Set(Am_GRID_Y, grid_if_should)
	      .Set(Am_MINIMUM_WIDTH, minw_from_tool)
	      .Set(Am_MINIMUM_HEIGHT, minh_from_tool)
	      .Set(Am_HOW_MANY_POINTS, points_from_tool)
	      )
    .Add_Part(Am_New_Points_Interactor.Create("create_polygons")
	      .Set(Am_START_WHEN, "ANY_SINGLE_LEFT_DOWN")
	      .Set(Am_STOP_WHEN, "ANY_DOUBLE_LEFT_DOWN")
	      .Set(Am_AS_LINE, true)
	      .Set(Am_FEEDBACK_OBJECT, lfeedback)
	      .Set(Am_START_DO_METHOD, polygon_start)
	      .Set(Am_INTERIM_DO_METHOD, polygon_interim_do)
	      .Set(Am_DO_METHOD, polygon_do)
	      .Set(Am_ABORT_DO_METHOD, polygon_abort)
	      .Set(Am_CREATE_NEW_OBJECT_METHOD, create_new_object)
	      .Set(Am_GRID_X, grid_if_should)
	      .Set(Am_GRID_Y, grid_if_should)
	      .Set(Am_ACTIVE, polygon_tool_is_selected)
	      )
    ;

  win = Am_Window.Create("Gilt Main Window")
    .Set(Am_TOP, 50)
    .Set(Am_LEFT, 50)
    .Set(Am_HEIGHT, 700)
    .Set(Am_WIDTH, 800)
    .Set(Am_TITLE, "Gilt: Amulet's Interface Builder")
    .Set(Am_ICON_TITLE, "Gilt")
    .Set(Am_DESTROY_WINDOW_METHOD, Am_Window_Destroy_And_Exit_Method)
    .Add_Part(Am_Group.Create()
	      .Set(Am_LAYOUT, Am_Horizontal_Layout)
	      .Set(Am_TOP, 30)
	      .Set(Am_WIDTH, Am_From_Owner(Am_WIDTH))
	      .Set(Am_HEIGHT, Am_From_Owner(Am_HEIGHT, -30))
	      .Add_Part(Am_Group.Create()
			.Set (Am_HEIGHT, Am_From_Owner(Am_HEIGHT))
			.Set(Am_WIDTH, PANEL_WIDTH)
			.Add_Part(tool_panel
				  .Set(Am_TOP, 3)
				  .Set(Am_LEFT, 3))
			.Add_Part(colorpanel = Am_Map.Create("fill_styles")
				  .Set(Am_TOP, 275)
				  .Set(Am_LEFT, 3)
				  .Set(Am_MAX_RANK, 4)
				  .Set(Am_LAYOUT, Am_Horizontal_Layout)
				  .Set(Am_ITEM_PROTOTYPE, 
				       Am_Rectangle.Create()
				       .Set(Am_WIDTH, 20)
				       .Set(Am_HEIGHT,15)
				       .Set(Am_FILL_STYLE, get_fill_style))
				  .Set(Am_ITEMS, FILL_STYLE_CNT)
				  .Add_Part(Am_Choice_Interactor.Create()
					    .Set(Am_HOW_SET, Am_CHOICE_SET)
					    .Get_Object(Am_COMMAND) 
					    .Set(Am_DO_METHOD, fillstyler)
					    .Get_Owner()))
			.Add_Part(Am_Map.Create("line_styles")
				   .Set(Am_TOP, 370)
				   .Set(Am_LEFT, 3)
				   .Set(Am_V_SPACING, 3)
				   .Set(Am_H_SPACING, 3)
				   .Set(Am_MAX_RANK, 4)
				   .Set(Am_LAYOUT, Am_Horizontal_Layout)
				   .Set(Am_ITEM_PROTOTYPE,
					Am_Line.Create()
					.Set(Am_WIDTH, 20)
					.Set(Am_HEIGHT,20)
					.Set(Am_LINE_STYLE, get_line_style))
				   .Set(Am_ITEMS, LINE_STYLE_CNT)
				   .Add_Part(Am_Choice_Interactor.Create()
					     .Set(Am_HOW_SET, Am_CHOICE_SET)
					     .Get_Object(Am_COMMAND) 
					     .Set(Am_DO_METHOD, linestyler)
					     .Get_Owner()))
			.Add_Part( Am_Map.Create()
				   .Set(Am_TOP,580)
				   .Set(Am_LEFT, 3)
				   .Set(Am_MAX_RANK, 4)
				   .Set(Am_LAYOUT, Am_Horizontal_Layout)
				   .Set(Am_ITEM_PROTOTYPE,
					Am_Text.Create()
					.Set(Am_WIDTH, 20)
					.Set(Am_HEIGHT,25)
					.Set(Am_TEXT,fontgen)
					.Set(Am_FONT, Am_Default_Font))
				   .Set(Am_ITEMS, FONT_ITEMS)
				   .Add_Part(Am_Choice_Interactor.Create()
					     .Set(Am_HOW_SET, Am_CHOICE_SET)
					     .Get_Object(Am_COMMAND) 
					     .Set(Am_DO_METHOD, fontstyler)
					     .Get_Owner()))
			)
	      .Add_Part(main_group = Am_Group.Create("main_group")
		          .Set(Am_HEIGHT, Am_From_Owner(Am_HEIGHT))
		          .Set(Am_WIDTH, Am_From_Owner(Am_WIDTH, -PANEL_WIDTH))
			)
	      );
  
  main_group_rect = main_group_rect_proto.Create()
	      .Set(Am_LINE_STYLE, Am_Line_1) .Set(FILL_STYLE_KEY, 9)
	      .Set(Am_FILL_STYLE, Am_White)
	      .Set(Am_LEFT, 0).Set(Am_TOP, 0)
	      .Set(Am_HEIGHT, Am_From_Owner(Am_HEIGHT))
	      .Set(Am_WIDTH, Am_From_Owner(Am_WIDTH))
	      ;


  main_group
    .Add_Part(main_group_rect)
    .Add_Part(workgroup)
    .Add_Part(lfeedback)
    .Add_Part(rfeedback)
    .Add_Part(sel_widget = Am_Selection_Widget.Create()
	      .Set(Am_ACTIVE, selection_tool)
	      .Set(Am_GRID_X, grid_if_should)
	      .Set(Am_GRID_Y, grid_if_should)
	      .Set(Am_OPERATES_ON, workgroup));
  
  menu_bar = Am_Menu_Bar.Create("menu_bar")
    .Set(Am_SELECTION_WIDGET, sel_widget)
    .Set(Am_ITEMS, Am_Value_List ()
         .Add (Am_Command.Create("File_Command")
               .Set(Am_IMPLEMENTATION_PARENT, true) //not undoable
               .Set(Am_LABEL, "File")
               .Set(Am_ITEMS, Am_Value_List ()
		    .Add (open_command = Am_Open_Command.Create()
			  .Set(Am_HANDLE_OPEN_SAVE_METHOD, use_file_contents))
		    .Add (Am_Save_As_Command.Create()
			  .Set(Am_HANDLE_OPEN_SAVE_METHOD, contents_for_save))
		    .Add (Am_Save_Command.Create()
			  .Set(Am_HANDLE_OPEN_SAVE_METHOD, contents_for_save))
		    .Add (Am_Command.Create("Save ")
                           .Set(Am_LABEL, "Generate C++ ...")
                           .Set(Am_ACTIVE, true)
			   .Set(Am_DO_METHOD, savecppcmd)
			   )
                     .Add (Am_Quit_No_Ask_Command.Create())
                     )
               )
           .Add (Am_Command.Create("Edit_Command")
                 .Set(Am_LABEL, "Edit")
                 .Set(Am_IMPLEMENTATION_PARENT, true) //not undoable
                 .Set(Am_ITEMS, Am_Value_List ()
                      .Add (Am_Undo_Command.Create())
                      .Add (Am_Redo_Command.Create())
                      .Add (Am_Menu_Line_Command.Create())
                      .Add (Am_Graphics_Cut_Command.Create())
                      .Add (Am_Graphics_Copy_Command.Create())
                      .Add (Am_Graphics_Paste_Command.Create())
                      .Add (Am_Graphics_Clear_Command.Create())
                      .Add (Am_Graphics_Clear_All_Command.Create())
		      .Add (Am_Menu_Line_Command.Create())
                      .Add (Am_Graphics_Duplicate_Command.Create())
                      .Add (Am_Selection_Widget_Select_All_Command.Create())
		      .Add (Am_Menu_Line_Command.Create())
		      .Add (Am_Command.Create()
			    .Set(Am_LABEL, "Properties...")
			    .Set(Am_ACTIVE, Am_Active_If_Selection)
			    .Set(Am_DO_METHOD, customize_selected)
			    .Set(Am_UNDO_METHOD, undo_set_properties)
			    .Set(Am_REDO_METHOD, undo_set_properties)
			    )
		      )
                 )
           .Add (Am_Command.Create("Arrange_Command")
                 .Set(Am_LABEL, "Arrange")
                 .Set(Am_IMPLEMENTATION_PARENT, true) //not undoable
                 .Set(Am_ITEMS, Am_Value_List ()
                      .Add (Am_Graphics_To_Top_Command.Create())
                      .Add (Am_Graphics_To_Bottom_Command.Create())
                      .Add (Am_Menu_Line_Command.Create())
                      .Add (Am_Graphics_Group_Command.Create())
                      .Add (Am_Graphics_Ungroup_Command.Create())
                      .Add (Am_Menu_Line_Command.Create())
		      .Add (grid_command)
                      .Add (Am_Menu_Line_Command.Create())
		      .Add (Am_Command.Create("Motif_Command")
                           .Set(Am_LABEL, "Motif Look")
			   .Add(Am_WIDGET_LOOK, Am_MOTIF_LOOK)
			   .Add(Am_CHECKED_ITEM, look_is_me)
                           .Set(Am_DO_METHOD, set_look)
			   .Set(Am_IMPLEMENTATION_PARENT, true)) //not undo
		      .Add (Am_Command.Create("Win_Command")
                           .Set(Am_LABEL, "Windows Look")
			   .Add(Am_WIDGET_LOOK, Am_WINDOWS_LOOK)
			   .Add(Am_CHECKED_ITEM, look_is_me)
			   .Set(Am_IMPLEMENTATION_PARENT, true) //not undo
                           .Set(Am_DO_METHOD, set_look))
		      .Add (Am_Command.Create("Mac_Command")
                           .Set(Am_LABEL, "Macintosh Look")
			   .Add(Am_WIDGET_LOOK, Am_MACINTOSH_LOOK)
			   .Add(Am_CHECKED_ITEM, look_is_me)
			   .Set(Am_IMPLEMENTATION_PARENT, true) //not undo
                           .Set(Am_DO_METHOD, set_look))
                      .Add (Am_Menu_Line_Command.Create())
		      .Add(Am_Command.Create()
			   .Set(Am_LABEL, "Nudge Left")
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_ACCELERATOR, "LEFT_ARROW")
			   .Set(Am_DO_METHOD, go_left)
			   )
		      .Add(Am_Command.Create()
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_LABEL, "Nudge Right")
			   .Set(Am_ACCELERATOR, "RIGHT_ARROW")
			   .Set(Am_DO_METHOD, go_right)
			   )
		      .Add(Am_Command.Create()
			   .Set(Am_LABEL, "Nudge Down")
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_ACCELERATOR, "DOWN_ARROW")
			   .Set(Am_DO_METHOD, go_down)
			   )
		      .Add(Am_Command.Create()
			   .Set(Am_LABEL, "Nudge Up")
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_ACCELERATOR, "UP_ARROW")
			   .Set(Am_DO_METHOD, go_up)
			   )
                      .Add (Am_Menu_Line_Command.Create())
		      .Add(Am_Command.Create()
			   .Set(Am_LABEL, "Align Lefts")
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_ACCELERATOR, "CONTROL_LEFT_ARROW")
			   .Set(Am_DO_METHOD, align_left)
			   )
		      .Add(Am_Command.Create()
			   .Set(Am_LABEL, "Align Rights")
			   .Set(Am_ACCELERATOR, "CONTROL_RIGHT_ARROW")
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_DO_METHOD, align_right)
			   )
		      .Add(Am_Command.Create()
			   .Set(Am_LABEL, "Align Tops")
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_ACCELERATOR, "CONTROL_UP_ARROW")
			   .Set(Am_DO_METHOD, align_up)
			   )
		      .Add(Am_Command.Create()
			   .Set(Am_LABEL, "Align Bottoms")
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_ACCELERATOR, "CONTROL_DOWN_ARROW")
			   .Set(Am_DO_METHOD, align_down)
			   )
                      .Add (Am_Menu_Line_Command.Create())
		      .Add(Am_Command.Create()
			   .Set(Am_LABEL, "Same Widths")
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_ACCELERATOR, "META_RIGHT_ARROW")
			   .Set(Am_DO_METHOD, same_widths)
			   )
		      .Add(Am_Command.Create()
			   .Set(Am_LABEL, "Same Heights")
			   .Set(Am_ACTIVE, Am_Active_If_Selection)
			   .Set(Am_ACCELERATOR, "META_DOWN_ARROW")
			   .Set(Am_DO_METHOD, same_heights)
			   )
                      )
                 )
	 .Add (Am_Command.Create("About_Command")
               .Set(Am_IMPLEMENTATION_PARENT, true) //not undoable
               .Set(Am_LABEL, "About")
               .Set(Am_ITEMS, Am_Value_List ()
                     .Add (Am_About_Amulet_Command.Create())
                     .Add (Am_Command.Create("About")
                           .Set(Am_LABEL, "About Gilt")
                           .Set(Am_ACTIVE, true)
			   .Set(Am_DO_METHOD, aboutcmd)
			 )
		    ))

           )
  ;
  win.Add_Part(menu_bar);
  global_undo_handler = Am_Multiple_Undo_Object.Create();
  win.Set(Am_UNDO_HANDLER, undo_handler_if_not_running);
  if (argc > 1) {
    Am_String s = (char *)argv[1];
    Am_Standard_Open_From_Filename(open_command, s);
  }
  Am_Screen.Add_Part(win);
  Am_Main_Event_Loop ();
  Am_Cleanup ();
  return 0;
} 

/* ***********************************************************************

TO DO:

----

From: MATHOG@seqaxp.bio.caltech.edu

Gilt is a very good start, but it needs some extra tools.  At the very
least, it needs align by center (vertically/horizontally). It also needs a
way of laying out user supplied widgets. For instance, if somebody writes a
graph widget, it would have to be coded in (or represented as just a box,
and the C++ code modified from there.) There is also one bug, ^down_arrow
doesn't align to bottom, but all the other directions work as adverstised. 

One problem I see, once I've exported the C++ from Gilt, if I modify the
code it generated I can't easily redo the interface without patching the 
code back in.  How do you folks work around that?  I'd like to think that 
the first interface design will be "it", but that isn't really very likely
in any real programming project.

I don't recall if Amulet has "icon buttons", but if not, it needs them, and
they should be added to Gilt.

-> maybe paste or drag-and-drop into a button to get picture to be the
label?  Or something with the pop-up dialog box?


----

* make undoable:
    nudges
    setting fillstyle, linestyle, font

* better font setting
* better linestyle setting
   - make size and color independent
* better organization for fill colors
   - allow custom colors also (any RGB)
* fix panel dialog so easier to enter multiple names

* drag and drop so can put things into scrolling windows?
* drag and drop so can reorder the names in menus

* ability to include standard commands?
* ability to set DO_methods?

* Menubars -- how set sub-menus?

* support multiple windows??

* double click on mode stay in that mode?

* add loading of gif pictures and bitmaps: radiobutton for whether
filename is relative to the Amulet root or not.

* menubar objects
* support for the built-in command objects
* ability to have arbitrary pictures in widgets

* Support for layout of groups
* All the rest of the properties of command objects & widgets
- default, enabled

* Use the hierarchy widget for the commands and items
- ability to drag-and-drop elements in the hierarchy list to move items
- also to promote and demote items

* Support for demonstrating and/or typing constraints to control
enabling/disabling, visibility.

* Support for adding interactors to the scene
* Support for custom methods and constraints in some kind of
interpreted language

* Move the various parts to be separate windows, and then support
multiple "frames" or windows for the main region.

* Connection with TCL/Python/JavaScript/interpreted C++

* Support for a gestural interface like Gilt

* Scripting (Brad's research project)

* From "Robert M. Muench" <100606.2653@CompuServe.COM>

Some questions for GILT: Is it possible to read in the description and
build the dialog on the fly in the client program? This would enable
something like a device-independet way to save a layout and create it
on the fly.

How to synchronize interactive GUI building and custom code which was
added afterwards? I can imagine that I can use GILT to build a
dialog-box etc. and let GILT generate the source-code. Is it possible
to hold all custom-code in an other file so that the GILT file can be
regenerated if the dialog-box changed?

*/
