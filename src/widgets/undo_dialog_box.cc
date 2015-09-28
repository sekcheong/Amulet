/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the button-like widgets
   
   Designed and implemented by Brad Myers
*/

#include <am_inc.h>

#include AM_IO__H

#include STANDARD_SLOTS__H
#include VALUE_LIST__H
#include UNDO_DIALOG__H
#include WIDGETS_ADVANCED__H  //for accelerator registering
#include INTER_ADVANCED__H  //for Am_Modify_Command_Label_Name
#include STR_STREAM__H
#include WIDGETS__H
#include INTER__H
#include OPAL__H
#include DEBUGGER__H

Am_Object Am_Undo_Dialog_Box;
Am_Object Am_Repeat_Other_Command;
Am_Value test;

Am_Slot_Key Am_UNDO_HANDLER_TO_DISPLAY =
  	Am_Register_Slot_Name ("~UNDO_HANDLER_TO_DISPLAY~");
Am_Slot_Key Am_UNDO_LABEL = Am_Register_Slot_Name ("~UNDO_LABEL~");
Am_Slot_Key Am_UNDO_SCROLL_GROUP =
	Am_Register_Slot_Name ("~UNDO_SCROLL_GROUP~");
Am_Slot_Key Am_UNDO_BUTTON_PANEL =
	Am_Register_Slot_Name ("~UNDO_BUTTON_PANEL~");
Am_Slot_Key Am_UNDO_MENU_OF_COMMANDS =
		Am_Register_Slot_Name ("~UNDO_MENU_OF_COMMANDS~");
Am_Slot_Key Am_SCROLLING_GROUP_SLOT = Am_Register_Slot_Name ("~SCROLLING_GROUP~");
Am_Slot_Key Am_UNDO_OPTIONS = Am_Register_Slot_Name ("~UNDO_OPTIONS~");
Am_Slot_Key Am_MARK_ACCELERATOR = Am_Register_Slot_Name ("~MARK_ACCELERATOR~");
Am_Slot_Key Am_MARK_COMMAND_DIALOG_BOX =
		Am_Register_Slot_Name ("~MARK_COMMAND_DIALOG_BOX~");
Am_Slot_Key Am_MARK_REPEAT_COMMAND =
		Am_Register_Slot_Name ("~MARK_REPEAT_COMMAND~");
Am_Slot_Key Am_REPEAT_COMMAND_ON_NEW =
		Am_Register_Slot_Name ("~REPEAT_COMMAND_ON_NEW~");
Am_Slot_Key Am_BORDER = Am_Register_Slot_Name ("~BORDER~");
Am_Slot_Key Am_UNDO_DIALOG_BOX_SLOT = Am_Register_Slot_Name ("UNDO_DIALOG_BOX");

const int ID_ON_THIS = 2;
const int ID_ON_SELECTION = 3;

Am_Object get_undo_handler(Am_Object cmd) {
  Am_Object panel, undo_handler;
  //Am_SAVED_OLD_OWNER of cmd is button_panel, owner is db
  panel = cmd.Get(Am_SAVED_OLD_OWNER);
  undo_handler = panel.Get_Owner().Get(Am_UNDO_HANDLER_TO_DISPLAY);
  return undo_handler;
}

Am_Define_Formula(int, scroll_group_top) {
  Am_Object label = self.Get_Sibling(Am_UNDO_LABEL);
  if (label.Valid()) // sometimes fails in cleanup
    return (int)label.Get(Am_TOP) + (int)label.Get(Am_HEIGHT) + 5;
  else return 10;
}

Am_Object if_list_get_obj(Am_Value v, bool only_if_one) {
  if (Am_Value_List::Test(v)) {
    Am_Value_List v_list;
    v_list = v;
    if (!only_if_one || (v_list.Length() == 1)) {
      v_list.Start();
      Am_Object o = v_list.Get();
      return o;
    }
  }
  return Am_No_Object;
}

Am_Define_Formula(int, scroll_group_height) {
  Am_Object buttons = self.Get_Sibling(Am_UNDO_BUTTON_PANEL);
  if (!buttons.Valid()) // sometimes fails in cleanup
    return 10;
  Am_Object window;
  window = self.Get(Am_WINDOW);
  if (window.Valid())
    return (int)window.Get(Am_HEIGHT) - (int)self.Get(Am_TOP) -
      (int)buttons.Get(Am_HEIGHT) - 15;
  else return 100;
}

Am_Define_Formula(int, scroll_group_width) {
  Am_Object radio_buttons = self.Get_Sibling(Am_UNDO_OPTIONS);
  return (int)radio_buttons.Get(Am_LEFT) - 10;
}

Am_Object output_obj_or_list(Am_Value &object_modified_value, OSTRSTREAM &oss){
  Am_Object object_modified = if_list_get_obj(object_modified_value, true);
  if (object_modified.Valid())
    oss << object_modified;
  else { //not a list
    if (object_modified_value.type == Am_OBJECT)
      object_modified = object_modified_value;
    oss << object_modified_value; 
  }
  return object_modified;
}

void write_command_name(Am_Object cmd, int cnt, OSTRSTREAM &oss,
			Am_Value mark_accel) { 
  Am_Value v, object_modified_value, new_value;
  Am_Object object_modified;
  v=cmd.Peek(Am_SHORT_LABEL, Am_NO_DEPENDENCY);
  if (!v.Valid()) v=cmd.Peek(Am_LABEL, Am_NO_DEPENDENCY);
  object_modified_value=cmd.Peek(Am_OBJECT_MODIFIED, Am_NO_DEPENDENCY);
  new_value=cmd.Peek(Am_VALUE, Am_NO_DEPENDENCY);
  oss << cnt;
  if (mark_accel.Valid()) {
    char s[Am_LONGEST_CHAR_STRING];
    Am_Input_Char i = mark_accel;
    i.As_Short_String(s);
    oss << "*(" << s << ")";
  }
  oss << ". " << v << " ";
  if (object_modified_value.Valid()) 
    object_modified = output_obj_or_list(object_modified_value, oss);
  if (!object_modified.Valid() ||
      new_value.type != Am_OBJECT ||
      Am_Object(new_value) != object_modified) {
    //don't output new_value if same as object_modified (as in create commands)
    oss << " = " << new_value; //don't test valid since 0 is a good value
  }
  oss << ends;
}

//generates a list of command objects whose labels are the right commands and 
Am_Define_Value_List_Formula(generate_undo_menu_items) {
  //don't do anything if my window is invisible
  Am_Object window = self.Get(Am_WINDOW);
  if (!window.Valid() || !(bool)window.Get(Am_VISIBLE))
    return NULL;
  Am_Object db, undo_handler;
  //self is menu, owner is Am_UNDO_SCROLL_GROUP, owner is db
  db = self.Get_Owner().Get_Owner();
  if (db.Valid())
    undo_handler = db.Get(Am_UNDO_HANDLER_TO_DISPLAY);
  
  if (!undo_handler.Valid()) return NULL;
  Am_Value_List cur_commands, menu_commands;
  cur_commands = undo_handler.Get(Am_COMMAND);
  Am_Object cmd;
  char line[200];
  int cnt = cur_commands.Length();

  Am_Value mark_accel;
  for (cur_commands.Start(); !cur_commands.Last();
       cur_commands.Next(), cnt--) {
    OSTRSTREAM_CONSTR (oss,line,200,ios::out);
    reset_ostrstream(oss);
    cmd = cur_commands.Get();
    mark_accel = cmd.Peek(Am_MARK_ACCELERATOR);
    write_command_name(cmd, cnt, oss, mark_accel);
    cmd.Set(Am_RANK, cnt, Am_OK_IF_NOT_THERE);
    OSTRSTREAM_COPY(oss,line,200);
    menu_commands.Add(Am_Command.Create()
		      .Set(Am_LABEL, line)
		      .Set(Am_ID, cmd)
		      );
  } //end loop
  
  //call Note_Changed on the currently selected command so the
  //constraints will re-check whether the operations are still legal
  self.Note_Changed(Am_VALUE);
  return menu_commands;
}

//returns the command to undo from the scrolling menu, or NULL if none.
Am_Object get_command_to_undo(Am_Object cmd) {
  Am_Value value = cmd.Get_Object(Am_SAVED_OLD_OWNER)
    .Get_Sibling(Am_UNDO_SCROLL_GROUP)
    .Get_Object(Am_UNDO_MENU_OF_COMMANDS).Peek(Am_VALUE);
  if (value.Valid()) {
    // because id is the cmd to undo, it will be the value of the menu
    Am_Object undo_cmd = value;
    return (undo_cmd);
  }
  return Am_No_Object;
}

//call notechanged on the currently selected command so the
//constraints will re-check whether the operations are still legal
void mark_changed_command_to_undo(Am_Object cmd) {
  Am_Object menu, panel;
  panel = cmd.Get(Am_SAVED_OLD_OWNER);
  menu = panel.Get_Sibling(Am_UNDO_SCROLL_GROUP)
    .Get_Object(Am_UNDO_MENU_OF_COMMANDS);
  menu.Note_Changed(Am_VALUE);
}

Am_Define_Formula(bool, last_undoable) {
  Am_Object last_command;
  Am_Object undo_handler = get_undo_handler (self);
  if (undo_handler.Valid())
    last_command = undo_handler.Get(Am_UNDO_ALLOWED);
  return last_command.Valid();
}

Am_Define_Method(Am_Object_Method, void, do_undo_last, (Am_Object cmd)) {
  Am_Object undo_handler = get_undo_handler(cmd);
  Am_Object_Method undoit;
  undoit = undo_handler.Get(Am_PERFORM_UNDO);
  undoit.Call (undo_handler);
  mark_changed_command_to_undo(cmd);
}

Am_Define_Formula(bool, redoable) {
  Am_Object last_command;
  Am_Object undo_handler = get_undo_handler(self);
  if (undo_handler.Valid())
    last_command = undo_handler.Get(Am_REDO_ALLOWED);
  return last_command.Valid();
}

Am_Define_Method(Am_Object_Method, void, am_do_redo, (Am_Object cmd)) {
  Am_Object undo_handler = get_undo_handler(cmd);
  Am_Object_Method redoit;
  redoit = undo_handler.Get(Am_PERFORM_REDO);
  redoit.Call (undo_handler);
  mark_changed_command_to_undo(cmd);
}

Am_Define_Method(Am_Object_Method, void, do_undo_this, (Am_Object cmd)) {
  Am_Object command_to_undo = get_command_to_undo(cmd);
  Am_Object undo_handler = get_undo_handler(cmd);
  if (command_to_undo.Valid()) {
    Am_Handler_Selective_Undo_Method method;
    method = undo_handler.Get(Am_SELECTIVE_UNDO_METHOD);
    method.Call(undo_handler, command_to_undo);
  }
  mark_changed_command_to_undo(cmd);
}

Am_Define_Formula(bool, exists_selection) {
  Am_Object cmd = get_command_to_undo(self);
  return cmd.Valid();
}

Am_Define_Formula(bool, selection_undoable) {
  Am_Object cmd = get_command_to_undo(self);
  bool allowed = false;
  if (cmd.Valid()) {
    // because id is the cmd to undo, it will be the value of the menu
    Am_Selective_Allowed_Method allowed_method;
    Am_Object undo_handler = get_undo_handler(self);
    if (undo_handler.Valid()) {
      allowed_method = undo_handler.Get(Am_SELECTIVE_UNDO_ALLOWED, Am_NO_DEPENDENCY);
      if (allowed_method.Valid())
	allowed = allowed_method.Call(cmd);
    }
  }
  return allowed;
}

Am_Define_Method(Am_Object_Method, void, do_repeat_this, (Am_Object cmd)) {
  Am_Object command_to_undo = get_command_to_undo(cmd);
  Am_Object undo_handler = get_undo_handler(cmd);
  if (command_to_undo.Valid()) {
    Am_Handler_Selective_Undo_Method method;
    method = undo_handler.Get(Am_SELECTIVE_REPEAT_SAME_METHOD);
    if (method.Valid()) method.Call(undo_handler, command_to_undo);
  }
  mark_changed_command_to_undo(cmd);
}

Am_Define_Formula(bool, selection_repeatable) {
  Am_Object cmd = get_command_to_undo(self);
  bool allowed = false;
  if (cmd.Valid()) {
    // because id is the cmd to undo, it will be the value of the menu
    Am_Object undo_handler = get_undo_handler(self);
    if (undo_handler.Valid()) {
      Am_Selective_Allowed_Method allowed_method;
      allowed_method = undo_handler.Get(Am_SELECTIVE_REPEAT_SAME_ALLOWED, Am_NO_DEPENDENCY);
      if (allowed_method.Valid())
	allowed = allowed_method.Call(cmd);
    }
  }
  return allowed;
}

void get_current_sel(Am_Object cmd, Am_Value &cur_sel) {
  //saved_old_owner is panel, owner of panel is db, which has the
  //selection handles widget, and want its value
  Am_Object sel_widget = cmd.Get_Object(Am_SAVED_OLD_OWNER).Get_Owner()
    .Get(Am_SELECTION_WIDGET);
  if (sel_widget.Valid())
    cur_sel = sel_widget.Peek(Am_VALUE);
  else cur_sel = Am_No_Value;
}

Am_Define_Method(Am_Object_Method, void, do_repeat_this_on_new,
		 (Am_Object cmd)){
  Am_Object command_to_undo = get_command_to_undo(cmd);
  Am_Object undo_handler = get_undo_handler(cmd);
  if (command_to_undo.Valid()) {
    Am_Value cur_sel;
    get_current_sel(cmd, cur_sel);
    Am_Handler_Selective_Repeat_New_Method method;
    method = undo_handler.Get(Am_SELECTIVE_REPEAT_ON_NEW_METHOD);
    method.Call(undo_handler, command_to_undo, cur_sel);
  }
  mark_changed_command_to_undo(cmd);
}

Am_Define_Formula(bool, selection_repeatable_on_new) {
  Am_Object cmd = get_command_to_undo(self);
  bool allowed = false;
  if (cmd.Valid()) {
    Am_Object undo_handler = get_undo_handler(self);
    if (undo_handler.Valid()) {
      // because id is the cmd to undo, it will be the value of the menu
      Am_Value cur_sel;
      get_current_sel(self, cur_sel);
      Am_Selective_New_Allowed_Method allowed_method;
      allowed_method = undo_handler.Get(Am_SELECTIVE_REPEAT_NEW_ALLOWED, Am_NO_DEPENDENCY);
      if (allowed_method.Valid())
	allowed = allowed_method.Call(cmd, cur_sel);
    }
  }
  return allowed;
}

Am_Define_Method(Am_Object_Method, void, do_flash, (Am_Object cmd)){
  Am_Object command_to_flash = get_command_to_undo(cmd);
  Am_Object object_modified;
  if (command_to_flash.Valid()) {
    Am_Value object_modified_value;
    object_modified_value=command_to_flash.Peek(Am_OBJECT_MODIFIED);
    if (object_modified_value.type == Am_OBJECT) {
      object_modified = object_modified_value;
      Am_Flash(object_modified, cout);
    }
    else {
      // ** TEMP, handle longer lists also **
      object_modified = if_list_get_obj(object_modified_value, true);
      if (object_modified.Valid())
	    Am_Flash(object_modified, cout);
    }
  }
}

Am_Define_Formula(bool, selection_flashable) {
  Am_Object command_to_flash = get_command_to_undo(self);
  if (command_to_flash.Valid()) {
    Am_Value object_modified_value;
    object_modified_value=command_to_flash.Peek(Am_OBJECT_MODIFIED, Am_NO_DEPENDENCY);
    // ** TEMP, handle lists also *******
    if (object_modified_value.type == Am_OBJECT)
      return true;
    else {
      Am_Object object_modified = if_list_get_obj(object_modified_value, true);
      if (object_modified.Valid()) return true;
    }
  }
  return false;
}

Am_Define_Formula(bool, selection_is_multiple) {
  Am_Object command = get_command_to_undo(self);
  if (command.Valid()) {
    Am_Value object_modified_value;
    object_modified_value=command.Peek(Am_OBJECT_MODIFIED, Am_NO_DEPENDENCY);
    if (Am_Value_List::Test(object_modified_value)) {
      Am_Value_List obj_list;
      obj_list = object_modified_value;
      if (obj_list.Length() > 1) return true;
    }
  }
  return false;
}

Am_Define_Method(Am_Object_Method, void, do_dismiss, (Am_Object cmd)){
  Am_Object panel, window;
  panel = cmd.Get(Am_SAVED_OLD_OWNER);
  window = panel.Get(Am_WINDOW);
  if (window.Valid())
    window.Set(Am_VISIBLE, false);
}

Am_Font bold_bigger_font = Am_Font(Am_FONT_SERIF, true, false, false,
				   Am_FONT_LARGE);

Am_Define_Formula(int, button_panel_top_formula) {
  int height = self.Get_Object(Am_WINDOW).Get(Am_HEIGHT);
  int my_height = self.Get(Am_HEIGHT);
  return height - my_height - 5;
}

/* ******
  Am_Define_String_Formula(toggle_record_selections_label) {
  Am_Value recording;
  recording = self.PV(Am_VALUE);
  if (recording.Valid()) return Am_String("Don't Record Selections");
  else return Am_String("Record Selections");
}
Am_Define_String_Formula(toggle_record_scrolling_label) {
  Am_Value recording;
  recording = self.PV(Am_VALUE);
  if (recording.Valid()) return Am_String("Don't Record Scrolling");
  else return Am_String("Record Scrolling");
}
***** */

Am_Define_Formula(int, check_panel_left) {
  return (int)self.Get_Object(Am_WINDOW).Get(Am_WIDTH) -
    (int)self.Get(Am_WIDTH) - 2;
}
Am_Define_Formula(int, check_panel_top) {
  Am_Object scroll_group = self.Get_Sibling(Am_UNDO_SCROLL_GROUP);
  if (scroll_group.Valid())
    return (int)scroll_group.Get(Am_TOP) + (int)scroll_group.Get(Am_HEIGHT) -
      (int)self.Get(Am_HEIGHT);
  else return 100;
}

Am_Define_Method(Am_Object_Method, void, toggle_record_scrolling,
		 (Am_Object cmd)) {
  Am_Value recording_value;
  recording_value=cmd.Peek(Am_VALUE);
  int recording;
  if (recording_value.Valid()) recording = 0;
  else recording = Am_NOT_USUALLY_UNDONE;
  Am_Object panel, scroll_group;
  panel = cmd.Get(Am_SAVED_OLD_OWNER);
  scroll_group = panel.Get_Owner().Get(Am_SCROLLING_GROUP_SLOT);
  if (scroll_group.Valid()) {
    scroll_group.Set(Am_COMMAND, recording);
  }
}

Am_Define_Method(Am_Object_Method, void, toggle_record_selections,
		 (Am_Object cmd)) {
  Am_Value recording_value;
  recording_value=cmd.Peek(Am_VALUE);
  int recording;
  if (recording_value.Valid()) recording = 0;
  else recording = Am_NOT_USUALLY_UNDONE;
  Am_Object panel, sel_widget, sel_command;
  panel = cmd.Get(Am_SAVED_OLD_OWNER);
  sel_widget = panel.Get_Owner().Get(Am_SELECTION_WIDGET);
  if (sel_widget.Valid()) {
    sel_command = sel_widget.Get_Object(Am_COMMAND);
    if (sel_command.Valid())
      sel_command.Set(Am_IMPLEMENTATION_PARENT, recording);
  }
}

///////////////////////////////////////////////////////////////////////////
// Mark command
///////////////////////////////////////////////////////////////////////////

void destroy_repeat_command_for(Am_Object cmd) {
  Am_Value v;
  v=cmd.Peek(Am_MARK_REPEAT_COMMAND);
  if (v.Valid()) {
    Am_Object repeat_command = v;
    if (repeat_command.Valid()) {
      Am_Object window = repeat_command.Get(Am_WINDOW);
      Am_Remove_Accelerator_Command_From_Window(repeat_command, window);
      cmd.Set(Am_MARK_REPEAT_COMMAND, NULL, Am_OK_IF_NOT_THERE);
      repeat_command.Destroy();
    }
  }
  cmd.Set(Am_MARK_ACCELERATOR, NULL, Am_OK_IF_NOT_THERE);
}
 
void create_repeat_command_for(Am_Object &cmd, Am_Input_Char accel,
			       Am_Object &window, bool on_new,
			       Am_Object &selection_widget,
			       Am_Object &repeat_command) {
  //first, see if one is already there
  if (repeat_command.Valid()) {
    repeat_command
      .Set(Am_ACCELERATOR, accel)
      .Set(Am_REPEAT_COMMAND_ON_NEW, on_new)
      ;
  }
  else { //create a repeat command and put it in the window
    Am_Value v;
    repeat_command = Am_Repeat_Other_Command.Create()
      .Add(Am_MARK_REPEAT_COMMAND, cmd, Am_OK_IF_NOT_THERE)
      .Set(Am_ACCELERATOR, accel)
      .Add(Am_WINDOW, window)
      .Add(Am_REPEAT_COMMAND_ON_NEW, on_new)
      .Add(Am_SELECTION_WIDGET, selection_widget)
      ;
    
    v=cmd.Peek(Am_SHORT_LABEL);
    repeat_command.Set(Am_SHORT_LABEL, v);
    v=cmd.Peek(Am_LABEL);
    repeat_command.Set(Am_LABEL, v);
    Am_Modify_Command_Label_Name(repeat_command,
				 Am_SELECTIVE_REPEAT_SAME_METHOD);
    cmd.Set(Am_MARK_REPEAT_COMMAND, repeat_command, Am_OK_IF_NOT_THERE)
      .Set_Inherit_Rule(Am_MARK_REPEAT_COMMAND, Am_LOCAL)
      ;
    Am_Add_Accelerator_Command_To_Window(repeat_command, window);
  }
}

Am_Define_Method(Am_Object_Method, void, repeat_other_do,
		 (Am_Object repeat_cmd)) {
  Am_Object cmd = repeat_cmd.Get(Am_MARK_REPEAT_COMMAND);
  Am_Object window = repeat_cmd.Get(Am_WINDOW);
  bool on_new = repeat_cmd.Get(Am_REPEAT_COMMAND_ON_NEW);
  bool allowed = false;
  Am_Object undo_handler = window.Get(Am_UNDO_HANDLER);
  if (undo_handler.Valid()) {
    if (on_new) {
      Am_Value new_selection;
      Am_Object selection_widget = repeat_cmd.Get(Am_SELECTION_WIDGET);
      if (selection_widget.Valid())
	new_selection=selection_widget.Peek(Am_VALUE);
      //first check if legal
      Am_Selective_New_Allowed_Method allowed_method;
      allowed_method = undo_handler.Get(Am_SELECTIVE_REPEAT_NEW_ALLOWED);
      if (allowed_method.Valid())
	allowed = allowed_method.Call(cmd, new_selection);
      if (allowed) {
	Am_Handler_Selective_Repeat_New_Method method;
	method = undo_handler.Get(Am_SELECTIVE_REPEAT_ON_NEW_METHOD);
	if (method.Valid()) method.Call(undo_handler, cmd, new_selection);
      }
      else { // not allowed
	Am_Beep();
      }
    }
    else { //not on_new
      //first check if legal
      Am_Selective_Allowed_Method allowed_method;
      allowed_method = undo_handler.Get(Am_SELECTIVE_REPEAT_SAME_ALLOWED);
      if (allowed_method.Valid())
	allowed = allowed_method.Call(cmd);
      if (allowed) {
	Am_Handler_Selective_Undo_Method method;
	method = undo_handler.Get(Am_SELECTIVE_REPEAT_SAME_METHOD);
	if (method.Valid()) method.Call(undo_handler, cmd);
      }
      else { //not allowed
	Am_Beep();
      }
    }
  }
}

Am_Define_Method(Am_Object_Method, void, mark_remove, (Am_Object cmd)) {
  Am_Object panel, mark_window, command_to_mark;
  panel = cmd.Get(Am_SAVED_OLD_OWNER);
  mark_window = panel.Get_Owner();
  command_to_mark = mark_window.Get(Am_COMMAND);
  destroy_repeat_command_for(command_to_mark);
  Am_Finish_Pop_Up_Waiting(mark_window, Am_No_Value);
}

Am_Define_Method(Am_Object_Method, void, mark_ok, (Am_Object cmd)) {
  Am_Object panel, mark_window, text_field, command_to_mark, main_window;
  panel = cmd.Get(Am_SAVED_OLD_OWNER);
  mark_window = panel.Get_Owner();
  main_window = mark_window.Get_Owner();
  command_to_mark = mark_window.Get(Am_COMMAND);
  Am_String text;
  text_field = panel.Get_Sibling(Am_ACCELERATOR);
  text = text_field.Get(Am_VALUE);
  if (text == "") {
    destroy_repeat_command_for(command_to_mark);
  }
  else {
    Am_Input_Char i = Am_Input_Char(text, false);
    if (i.Valid()) {
      Am_Object sel_widget = main_window.Get(Am_SELECTION_WIDGET);
      Am_Object window = sel_widget.Get(Am_WINDOW);

      Am_Value v;
      v=command_to_mark.Peek(Am_MARK_REPEAT_COMMAND);
      Am_Object old_repeat_command;
      if (v.Valid()) old_repeat_command = v;
      Am_Object other_cmd = Am_Check_Accelerator_Char_For_Window(i, window);
      if (other_cmd.Valid() && other_cmd != old_repeat_command) {
	Am_POP_UP_ERROR_WINDOW("Accelerator " << i << " already in use by "
			       << other_cmd);
	return; //don't make the window invisible if invalid
      }
      command_to_mark.Set(Am_MARK_ACCELERATOR, i, Am_OK_IF_NOT_THERE)
	.Set_Inherit_Rule(Am_MARK_ACCELERATOR, Am_LOCAL);
      Am_Object on_new_buttons = mark_window.Get_Object(Am_UNDO_OPTIONS);
      bool on_new = (int)on_new_buttons.Get(Am_VALUE) == ID_ON_SELECTION;
      create_repeat_command_for(command_to_mark, i, window, on_new,
				sel_widget, old_repeat_command);
    }
    else {
      Am_Beep();
      return; //don't make the window invisible if invalid
    }
  }
  Am_Finish_Pop_Up_Waiting(mark_window, Am_No_Value);
}

Am_Define_Method(Am_Object_Method, void, mark_cancel, (Am_Object cmd)) {
  Am_Object panel, mark_window;
  panel = cmd.Get(Am_SAVED_OLD_OWNER);
  mark_window = panel.Get_Owner();
  Am_Finish_Pop_Up_Waiting(mark_window, Am_No_Value);
}

void set_on_new(Am_Object command_to_mark, Am_Object db) {
  bool on_new = false;
  int int_val;
  //first, see if one is already there
  Am_Value v;
  v=command_to_mark.Peek(Am_MARK_REPEAT_COMMAND);
  if (v.Valid()) {
    Am_Object repeat_command = v;
    on_new = repeat_command.Get(Am_REPEAT_COMMAND_ON_NEW);
  }
  Am_Object on_new_buttons = db.Get_Object(Am_UNDO_OPTIONS);
  if (on_new) int_val = ID_ON_SELECTION;
  else int_val = ID_ON_THIS;
  on_new_buttons.Set(Am_VALUE, int_val);
}

Am_Define_Method(Am_Object_Method, void, display_mark_window,
		 (Am_Object cmd)) {
  Am_Object command_to_mark = get_command_to_undo(cmd);
  Am_Object panel, db, text, text_widget;
  if (command_to_mark.Valid()) {
    panel = cmd.Get(Am_SAVED_OLD_OWNER);
    db = panel.Get_Owner().Get_Object(Am_MARK_COMMAND_DIALOG_BOX);
    db.Set(Am_COMMAND, command_to_mark);
    //next, set the label so like "Repeat on Rectangle_345"
    Am_Value items_value, object_modified_value;
    items_value = db.Get_Object(Am_UNDO_OPTIONS).Peek(Am_ITEMS);
    Am_Object cmd_for_this = if_list_get_obj(items_value, false);
    char line[200];
    {
      OSTRSTREAM_CONSTR (oss,line, 200, ios::out);
      reset_ostrstream(oss);
      oss << "Repeat on ";
      object_modified_value=command_to_mark.Peek(Am_OBJECT_MODIFIED);
      if (object_modified_value.Valid()) 
        output_obj_or_list(object_modified_value, oss);
      oss << ends;
      OSTRSTREAM_COPY(oss,line,200);
      cmd_for_this.Set(Am_LABEL, line);
	
      text = db.Get_Object(Am_UNDO_LABEL);
      text_widget = db.Get_Object(Am_ACCELERATOR);
    
    }
    {
      int cnt = command_to_mark.Get(Am_RANK);
      OSTRSTREAM_CONSTR (oss,line, 200, ios::out);
      reset_ostrstream(oss);
      oss << "Mark Command: ";
      write_command_name(command_to_mark, cnt, oss, Am_No_Value);
      OSTRSTREAM_COPY(oss,line,200);
      text.Set(Am_TEXT, line);
      Am_Value mark_accel;
      mark_accel=command_to_mark.Peek(Am_MARK_ACCELERATOR);
      if (mark_accel.Valid()) {
        char s[Am_LONGEST_CHAR_STRING];
        Am_Input_Char i = mark_accel;
        i.As_Short_String(s);
        mark_accel = s;
      }
      else mark_accel = "";
      set_on_new(command_to_mark, db);
      Am_Start_Widget(text_widget, mark_accel);
      //return value is ignored since the action routines of the widgets
      //do the work
      Am_Value ret;
      Am_Pop_Up_Window_And_Wait(db, ret, true);
    }
  }
}

Am_Define_Method(Am_Text_Edit_Method, void, single_char_method,
		 (Am_Object text, Am_Input_Char ic, Am_Object /*inter*/ )) {
  if (ic.click_count == Am_NOT_MOUSE) {
    char s[Am_LONGEST_CHAR_STRING];
    ic.As_Short_String(s);
    text.Set(Am_TEXT, s);
    text.Set(Am_CURSOR_INDEX, 0);
  }
}

///////////////////////////////////////////////////////////////////////////

Am_Define_Method(Am_Object_Method, void, am_show_undo_box, (Am_Object cmd)) {
  Am_Object my_undo_dialog = cmd.Get(Am_UNDO_DIALOG_BOX_SLOT);
  my_undo_dialog.Set(Am_VISIBLE, true);
  Am_To_Top(my_undo_dialog);
  }

///////////////////////////////////////////////////////////////////////////
// Initialize
///////////////////////////////////////////////////////////////////////////

Am_Object Am_Show_Undo_Dialog_Box_Command;

void Am_Initialize_Undo_Dialog_Box() {
  Am_Object tiw;
  Am_Repeat_Other_Command = Am_Command.Create("Repeat_Other_Command")
    .Set(Am_DO_METHOD, repeat_other_do)
    .Set(Am_LABEL, "Repeat_Other_Command")
    .Set(Am_IMPLEMENTATION_PARENT, Am_NOT_USUALLY_UNDONE) //don't queue this
    ;

  Am_Object mcdb = Am_Window.Create("Mark Command Dialog Box")
	      .Set (Am_WIDTH, 400)
	      .Set (Am_HEIGHT, 150)
	      .Set (Am_VISIBLE, false)
	      .Add (Am_FONT, Am_From_Owner(Am_FONT))
	      .Set (Am_FILL_STYLE, Am_Motif_Light_Green)
	      .Add (Am_WIDGET_LOOK, Am_From_Owner(Am_WIDGET_LOOK))
	      .Set (Am_LEFT, Am_Center_X_Is_Center_Of_Owner)
	      .Set (Am_TOP, Am_Center_Y_Is_Center_Of_Owner)
	      .Add (Am_COMMAND, NULL) //set with command to mark
	      .Set (Am_DESTROY_WINDOW_METHOD, Am_Window_Hide_Method)
	      .Add_Part(Am_BORDER, Am_Rectangle.Create()
			.Set(Am_WIDTH, Am_From_Owner(Am_WIDTH))
			.Set(Am_HEIGHT, Am_From_Owner(Am_HEIGHT))
			.Set(Am_LINE_STYLE, Am_Line_2)
			.Set(Am_FILL_STYLE, Am_No_Style)
			)
	      .Add_Part(Am_UNDO_LABEL, Am_Text.Create("Label")
			.Set (Am_FONT, bold_bigger_font)
			.Set (Am_LEFT, 10)
			.Set (Am_TOP, 10)
			.Set (Am_TEXT, "Mark Command ")
			);
  //Visual C++ can't handle big statements
  mcdb
	      .Add_Part(Am_ACCELERATOR,tiw=Am_Text_Input_Widget.Create("accel")
			.Set(Am_LEFT, 10)
			.Set(Am_TOP, 40)
			.Set(Am_WIDTH, 300)
			.Set(Am_FILL_STYLE, Am_From_Owner(Am_FILL_STYLE))
			.Set(Am_WIDGET_LOOK, Am_From_Owner(Am_WIDGET_LOOK))
			.Set_Part(Am_COMMAND, Am_Command.Create()
				  .Set(Am_LABEL, "Accelerator key:")
				  )
			)
	      .Add_Part(Am_UNDO_OPTIONS,
				Am_Radio_Button_Panel.Create("this_or_new")
			.Set(Am_TOP, 70)
			.Set(Am_LEFT, 10)
			.Set(Am_ITEM_OFFSET, 0)
			.Set(Am_LAYOUT, Am_Vertical_Layout)
			.Set(Am_FILL_STYLE, Am_From_Owner(Am_FILL_STYLE))
			.Set(Am_ITEMS, Am_Value_List()
			     .Add(Am_Command.Create("this")
				  .Set(Am_LABEL, "Repeat on XXX")
				  .Set(Am_ID, ID_ON_THIS))
			     .Add(Am_Command.Create("sel")
				  .Set(Am_LABEL,"Repeat on Current Selection")
				  .Set(Am_ID, ID_ON_SELECTION))
			     )
			)
	      .Add_Part(Am_UNDO_BUTTON_PANEL, Am_Button_Panel.Create("ok")
			.Set(Am_LEFT, Am_Center_X_Is_Center_Of_Owner)
			.Set(Am_TOP, 120)
			.Set(Am_ITEM_OFFSET, 0)
			.Set(Am_LAYOUT, Am_Horizontal_Layout)
			.Set(Am_FILL_STYLE, Am_From_Owner(Am_FILL_STYLE))
			.Set(Am_ITEMS, Am_Value_List()
			     .Add(Am_Command.Create("OK")
				  .Set(Am_LABEL, "OK")
				  .Set(Am_DO_METHOD, mark_ok))
			     .Add(Am_Command.Create("Cancel")
				  .Set(Am_LABEL, "Cancel")
				  .Set(Am_DO_METHOD, mark_cancel))
			     .Add(Am_Command.Create("Remove")
				  .Set(Am_LABEL, "Remove")
				  .Set(Am_DO_METHOD, mark_remove))
			     )
			); // end mark-window
  Am_Undo_Dialog_Box = Am_Window.Create("Undo Dialog Box")
    .Set (Am_WIDTH, 425)  //defaults, can be overridden
    .Set (Am_HEIGHT, 400)
    .Set (Am_TITLE, "Amulet Selective Undo/Redo/Repeat")
    .Add (Am_FONT, Am_Default_Font)
    .Set (Am_FILL_STYLE, Am_Amulet_Purple)
    .Add (Am_WIDGET_LOOK, Am_Default_Widget_Look)
    .Add (Am_UNDO_HANDLER_TO_DISPLAY, NULL) //set with one to show values for
    .Add (Am_SELECTION_WIDGET, NULL) //set with selection handles widget
    .Add (Am_SCROLLING_GROUP_SLOT, NULL) //set with scrolling group widget
    //internal, do not set
    .Set (Am_UNDO_HANDLER, NULL) //don't undo stuff done here! 
    // if close this window from WM, just hide it
    .Set (Am_DESTROY_WINDOW_METHOD, Am_Window_Hide_Method)
    .Add_Part(Am_MARK_COMMAND_DIALOG_BOX, mcdb)
    .Add_Part(Am_UNDO_LABEL, Am_Text.Create("Label")
	      .Set (Am_FONT, bold_bigger_font)
	      .Set (Am_LEFT, 5)
	      .Set (Am_TOP, 5)
	      .Set (Am_TEXT, "Select Command to Undo or Repeat:")
	      )
    .Add_Part(Am_UNDO_OPTIONS, Am_Checkbox_Panel.Create("UNDO_OPTIONS")
	      .Set(Am_LEFT, check_panel_left)
	      .Set(Am_TOP, check_panel_top)
	      .Set(Am_FONT, Am_From_Owner(Am_FONT))
	      .Set(Am_FILL_STYLE, Am_From_Owner(Am_FILL_STYLE))
	      .Set(Am_WIDGET_LOOK, Am_From_Owner(Am_WIDGET_LOOK))
	      .Set(Am_FIXED_WIDTH, false)
	      .Set(Am_ITEMS, Am_Value_List ()
		   .Add (Am_Command.Create("Record Selections")
			 .Set (Am_LABEL, "Record Selections")
			 .Set (Am_DO_METHOD, toggle_record_selections)
			 .Set (Am_ACTIVE, true)
			 .Set (Am_VALUE, false)
			 )
		   .Add (Am_Command.Create("Record Scrolling")
			 .Set (Am_LABEL, "Record Scrolling")
			 .Set (Am_DO_METHOD, toggle_record_scrolling)
			 .Set (Am_ACTIVE, true)
			 .Set (Am_VALUE, false)
			 )
		   )
	      );
   //Visual C++ can't handle big statements
   Am_Value_List l = Am_Value_List ()
		   .Add (Am_Command.Create("Undo Last")
			 .Set (Am_LABEL, "Undo Last")
			 .Set (Am_DO_METHOD, do_undo_last)
			 .Set (Am_ACTIVE, last_undoable)
			 )
		   .Add (Am_Command.Create("Redo undone command")
			 .Set (Am_LABEL, "Redo Undone Command")
			 .Set (Am_DO_METHOD, am_do_redo)
			 .Set (Am_ACTIVE, redoable)
			 )
		   .Add (Am_Command.Create("Undo This")
			 .Set (Am_LABEL, "Undo This")
			 .Set (Am_DO_METHOD, do_undo_this)
			 .Set (Am_ACTIVE, selection_undoable)
			 )
		   .Add (Am_Command.Create("Repeat This")
			 .Set (Am_LABEL, "Repeat This")
			 .Set (Am_DO_METHOD, do_repeat_this)
			 .Set (Am_ACTIVE, selection_repeatable)
			 )
		   .Add (Am_Command.Create("Repeat This on Current Selection")
			 .Set (Am_LABEL, "Repeat This on Current Selection")
			 .Set (Am_DO_METHOD, do_repeat_this_on_new)
			 .Set (Am_ACTIVE, selection_repeatable_on_new)
			 )
		   .Add (Am_Command.Create("Flash")
			 .Set (Am_LABEL, "Flash Object")
			 .Set (Am_DO_METHOD, do_flash)
			 .Set (Am_ACTIVE, selection_flashable)
			 )
		   .Add (Am_Command.Create("Expand")
			 .Set (Am_LABEL, "Expand")
			 .Set (Am_DO_METHOD, NULL /* expand_command */)
			 .Set (Am_ACTIVE, selection_is_multiple)
			 )
		   .Add (Am_Command.Create("Mark Command")
			 .Set (Am_LABEL, "Mark Command...")
			 .Set (Am_DO_METHOD, display_mark_window)
			 .Set (Am_ACTIVE, exists_selection)
			 )
		   .Add (Am_Command.Create("Done")
			 .Set (Am_LABEL, "Done")
			 .Set (Am_DO_METHOD, do_dismiss)
			 .Set (Am_ACTIVE, true)
		   );
   Am_Undo_Dialog_Box
    .Add_Part(Am_UNDO_BUTTON_PANEL, Am_Button_Panel.Create("UNDO_BUTTON_PANEL")
	      .Set(Am_LEFT, 5)
	      .Set(Am_TOP, button_panel_top_formula)
	      .Set(Am_MAX_SIZE, Am_From_Owner(Am_WIDTH))
	      .Set(Am_FONT, Am_From_Owner(Am_FONT))
	      .Set(Am_FILL_STYLE, Am_From_Owner(Am_FILL_STYLE))
	      .Set(Am_WIDGET_LOOK, Am_From_Owner(Am_WIDGET_LOOK))
	      .Set(Am_FIXED_WIDTH, false)
	      .Set(Am_LAYOUT, Am_Horizontal_Layout)
	      .Set(Am_ITEMS, l)
		  )
    .Add_Part(Am_UNDO_SCROLL_GROUP,
	             Am_Scrolling_Group.Create("UNDO_SCROLL_GROUP")
	      .Set(Am_LEFT, 5)
	      .Set(Am_TOP, scroll_group_top)
	      .Set(Am_HEIGHT, scroll_group_height)
	      .Set(Am_WIDTH, scroll_group_width)
	      .Set(Am_INNER_WIDTH, Am_Width_Of_Parts)
	      .Set(Am_INNER_HEIGHT, Am_Height_Of_Parts)
	      .Set(Am_FILL_STYLE, Am_From_Owner(Am_FILL_STYLE))
	      .Set(Am_WIDGET_LOOK, Am_From_Owner(Am_WIDGET_LOOK))
	      .Add(Am_FONT, Am_From_Owner(Am_FONT))
	      .Add_Part(Am_UNDO_MENU_OF_COMMANDS, Am_Menu.Create("Undo Menu")
			.Set(Am_FONT, Am_From_Owner(Am_FONT))
			.Set(Am_FILL_STYLE, Am_From_Owner(Am_FILL_STYLE))
			.Set(Am_WIDGET_LOOK, Am_From_Owner(Am_WIDGET_LOOK))
			.Set(Am_LEFT, 0)
			.Set(Am_FINAL_FEEDBACK_WANTED, true)
			.Set(Am_TOP, 0)
			.Set(Am_ITEMS, generate_undo_menu_items)
			.Set(Am_HOW_SET, Am_CHOICE_TOGGLE)
	        )
		 )
    ; // end of undo dialog box

  tiw.Get_Object(Am_INTERACTOR)
    .Set_Name("Get_Accel_Key_Inter")
    .Set(Am_STOP_WHEN, NULL) //no keys stop it
    .Set(Am_TEXT_EDIT_METHOD, single_char_method)
    ;

  Am_Show_Undo_Dialog_Box_Command = Am_Command.Create("Show_Undo_DB")
    .Set (Am_DO_METHOD, am_show_undo_box)
    .Set (Am_LABEL, "Undo/Redo/Repeat...")
    .Set (Am_IMPLEMENTATION_PARENT, Am_NOT_USUALLY_UNDONE)
    ;

}

/* 

New comands to add:

Set Bookmark on Command
Name Bookmark... (also allows accelerator key)
Find Bookmark Command...

(Maybe need a menubar instead of buttons)
*/
