/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the basic functions for Command objects and
   Undo handling
   
   Designed and implemented by Brad Myers
*/

#ifdef GCC
#include <string.h>
#elif defined(_MSC_VER) || defined(NEED_STRING)
extern "C" {
#include <string.h>
}
#endif

#include <am_inc.h>

#include AM_IO__H

#include INTER_ADVANCED__H
#include STANDARD_SLOTS__H
#include VALUE_LIST__H
#include FORMULA__H
#include OPAL__H  //for am_define_object_formula

Am_Define_Method_Type_Impl(Am_Selective_Allowed_Method);
Am_Define_Method_Type_Impl(Am_Selective_New_Allowed_Method);
Am_Define_Method_Type_Impl(Am_Selective_Repeat_New_Method);
Am_Define_Method_Type_Impl(Am_Handler_Selective_Undo_Method);
Am_Define_Method_Type_Impl(Am_Handler_Selective_Repeat_New_Method);

////////////////////////////////////////////////////////////
// Utility functions to process all the relevant commands
////////////////////////////////////////////////////////////

Am_Object Am_Inter_Find_Undo_Handler(const Am_Object& inter_or_widget) {
  Am_Value value;
  Am_Object undo_object, window;
  value=inter_or_widget.Peek(Am_WINDOW);
  if (value.Valid()) {
    window = value;
    value=window.Peek(Am_UNDO_HANDLER);
    if (value.Valid())
      undo_object = value;
  }
  return undo_object;
}

// command_obj's DO has already been "done", now do the DO methods of
// the Am_IMPLEMENTATION_PARENT's of command_obj, recursively on up
void Am_Process_All_Parent_Commands(Am_Object& command_obj,
			       Am_Object& undo_handler,
			       const Am_Register_Command_Method &reg_method) {
  Am_Value value;
  Am_Object_Method do_method;
  while (true) {
    value=command_obj.Peek(Am_IMPLEMENTATION_PARENT);
    if (!value.Valid() || value.type != Am_OBJECT) break;
    //here is an object, see if right type
    command_obj = value;
    if (!command_obj.Is_Instance_Of(Am_Command)) break; 
    value=command_obj.Peek(Am_DO_METHOD);
    if (Am_Object_Method::Test(value)) {
      command_obj.Set(Am_COMMAND_IS_ABORTING, false, Am_OK_IF_NOT_THERE);
      do_method = value;
      Am_INTER_TRACE_PRINT(command_obj, "%%Executing Am_DO_METHOD of " <<
			   command_obj << "=" << do_method);
      do_method.Call(command_obj);
      if ((bool)command_obj.Get(Am_COMMAND_IS_ABORTING)) {
	return;  //break out of loop without registering this command
      }
    }
    if(reg_method.Valid()) reg_method.Call(undo_handler, command_obj);
  }
}

void Am_Execute_Command (Am_Object& command, const Am_Object& widget)
{
  if (!command.Is_Instance_Of(Am_Command))
    return;
  Am_Value value;
  value=command.Peek(Am_DO_METHOD);
  if (!Am_Object_Method::Test(value))
    return;
  command.Set (Am_COMMAND_IS_ABORTING, false, Am_OK_IF_NOT_THERE);
  Am_Object_Method do_method (value);
  Am_INTER_TRACE_PRINT (command, "%%Executing Am_DO_METHOD of "
			<< command << "=" << do_method);
  do_method.Call(command);
  if ((bool)command.Get (Am_COMMAND_IS_ABORTING))
    return;  //break out of loop without registering this command
  Am_Object undo_handler = Am_Inter_Find_Undo_Handler (widget);
  if (!undo_handler.Valid ())
    return;
  Am_Register_Command_Method reg_method =
       undo_handler.Get (Am_REGISTER_COMMAND);
  if (reg_method.Valid ())
    reg_method.Call (undo_handler, command);
  Am_Process_All_Parent_Commands (command, undo_handler, reg_method);
}

//returns true if has a valid Am_IMPLEMENTATION_PARENT.  Also, if parent is a
//command object, sets its Am_IMPLEMENTATION_CHILD slot to me
bool has_impl_parent_and_set(Am_Object command_obj) {
  Am_Value value;
  value=command_obj.Peek(Am_IMPLEMENTATION_PARENT);
  if (value.Valid() && value.type == Am_OBJECT) {
    //here is an object, see if right type
    Am_Object parent;
    parent = value;
    if (parent.Is_Instance_Of(Am_Command)) {
      Am_Value impl_value;
      impl_value=parent.Peek(Am_IMPLEMENTATION_CHILD);
      //don't store the child if parent says not to
      if (impl_value.type != Am_INT ||
	  (int)impl_value != Am_DONT_UNDO_CHILDREN) {
	// put the actual command_obj there, will be copied later if necessary
	parent.Set(Am_IMPLEMENTATION_CHILD, command_obj);
      }
    }
  }
  return value.Valid(); //if Valid, then there is a parent
}

void Am_Modify_Command_Label_Name(Am_Object cmd, Am_Slot_Key slot) {
  Am_Value v_short, v;
  Am_String str;
  v_short=cmd.Peek(Am_SHORT_LABEL);
  if (v_short.Valid()) v = v_short;
  else v=cmd.Peek(Am_LABEL);
  if (v.type == Am_STRING) {
    str = v;
    char* old_string = str;
    char* prefix = 0;
    switch (slot) {
    case Am_SELECTIVE_UNDO_METHOD: prefix = "Undo "; break;
    case Am_SELECTIVE_REPEAT_SAME_METHOD: 
    case Am_SELECTIVE_REPEAT_ON_NEW_METHOD: prefix = "Repeat "; break;
    default: Am_Error("Bad slot in modify_label_name");
    } 
    char *new_string = (char*) new char[(strlen(prefix) + strlen(old_string)
					   + 1)];
    strcpy(new_string, prefix);
    strcat(new_string, old_string);
    //use false so doesn't copy the string since just allocated it
    Am_String str(new_string, false);
    if (v_short.Valid()) cmd.Set(Am_SHORT_LABEL, str);
    else cmd.Set(Am_LABEL, str);
  }
}

Am_Object do_undo_action_4_children(Am_Object last_command,
		 Am_Slot_Key slot, bool copy = false, Am_Value new_val = 0) {
  Am_Object_Method method;
  Am_Selective_Repeat_New_Method new_method;
  bool is_new = slot == Am_SELECTIVE_REPEAT_ON_NEW_METHOD;
  Am_Value value;
  Am_Object obj, prev;
  #ifdef DEBUG
  bool debug;
  #endif
  if (copy) {
    Am_INTER_TRACE_PRINT_NOENDL(last_command, "  Copying command obj " <<
				last_command);
    last_command = last_command.Copy_Value_Only();
    Am_INTER_TRACE_PRINT(last_command, " to " << last_command);
    Am_Modify_Command_Label_Name(last_command, slot);
    prev = last_command;
  }
  obj = last_command;
  //first go through and make all copies, and find the leaf
  while (true) {
    value=obj.Peek(Am_IMPLEMENTATION_CHILD);
    if (value.type != Am_OBJECT) break;
    obj = value;
    if (copy) {
      Am_INTER_TRACE_PRINT_NOENDL(obj, "  Copying command obj " << obj);
      obj = obj.Copy_Value_Only();
      Am_INTER_TRACE_PRINT(last_command, " to " << obj);
      prev.Set(Am_IMPLEMENTATION_CHILD, obj); //set with copy
      obj.Set(Am_IMPLEMENTATION_PARENT, prev);
      prev = obj;
    }
  }
  //here, obj is the leaf (child-most) object in the chain
  while (true) {
    #ifdef DEBUG
    debug = Am_Inter_Tracing(obj);
    if (debug) {
      switch(slot) {
      case Am_UNDO_METHOD: cout << "%%Executing Am_UNDO_METHOD"; break;
      case Am_REDO_METHOD: cout << "%%Executing Am_REDO_METHOD"; break;
      case Am_SELECTIVE_UNDO_METHOD:
	cout << "%%Executing Am_SELECTIVE_UNDO_METHOD"; break;
      case Am_SELECTIVE_REPEAT_SAME_METHOD:
	cout << "%%Executing Am_SELECTIVE_REPEAT_SAME_METHOD"; break;
      case Am_SELECTIVE_REPEAT_ON_NEW_METHOD:
	cout << "%%Executing Am_SELECTIVE_REPEAT_ON_NEW_METHOD"; break;
      }
      cout << " of " << obj;
    }
    #endif
    if (is_new) {
      new_method = obj.Get(Am_SELECTIVE_REPEAT_ON_NEW_METHOD);
      if (new_method.Valid()) {
	    Am_INTER_TRACE_PRINT(obj, "=" << new_method);
	    new_method.Call(obj, new_val);
      }
      else {
        Am_INTER_TRACE_PRINT(obj, " but no method");
      }
    }
    else {
      method = obj.Get(slot);
      if (method.Valid()) {
	    Am_INTER_TRACE_PRINT(obj, "=" << method);
	    method.Call(obj);
      }
      else {
        Am_INTER_TRACE_PRINT(obj, " but no method");
      }
    }
    value=obj.Peek(Am_IMPLEMENTATION_PARENT);
    if (!value.Valid() || value.type != Am_OBJECT) break;
    obj = value;
  }
  return last_command;
}

//copies command_obj and any Am_IMPLEMENTATION_CHILD recursively
Am_Object copy_command_tree(Am_Object command_obj) {
  command_obj = command_obj.Copy_Value_Only();
  Am_Object child, parent, child_copy;
  parent = command_obj;
  Am_Value value;
  while(true) {
    value=parent.Peek(Am_IMPLEMENTATION_CHILD);
    if (value.Valid() && value.type == Am_OBJECT) {
      child = value;
      child_copy = child.Copy_Value_Only();
      parent.Set(Am_IMPLEMENTATION_CHILD, child_copy);
      child_copy.Set(Am_IMPLEMENTATION_PARENT, parent);
      parent = child_copy;
    }
    else break; //leave loop
  }
  return command_obj;
}

//destroys command_obj and any Am_IMPLEMENTATION_CHILD recursively
void destroy_command_tree(Am_Object command_obj) {
  Am_Object parent;
  parent = command_obj;
  Am_Value value;
  while(true) {
    value=parent.Peek(Am_IMPLEMENTATION_CHILD);
    parent.Destroy();
    if (value.Valid() && value.type == Am_OBJECT) {
      parent = value;
    }
    else break; //leave loop
  }
}

////////////////////////////////////////////////////////////
// Single Undo, no selective undo
////////////////////////////////////////////////////////////

Am_Define_Method(Am_Register_Command_Method, bool,
		 single_undo_register_command,
		 (Am_Object undo_handler, Am_Object command_obj)) {
  if (has_impl_parent_and_set(command_obj)) return false;
  else {
    Am_Object old_command;
    old_command = undo_handler.Get(Am_COMMAND);

    if (old_command.Valid ())
      destroy_command_tree(old_command);
    undo_handler.Set(Am_COMMAND, copy_command_tree(command_obj));
    return true;
  }
}

Am_Define_Method(Am_Object_Method, void, single_perform_undo,
		 (Am_Object undo_handler)) {
  Am_Object last_command;
  last_command = undo_handler.Get(Am_COMMAND);
  if (!last_command.Valid ())
    Am_Error("No last command to undo for Single Undo Handler");
  else {
    do_undo_action_4_children(last_command, Am_UNDO_METHOD);
    // leave the command as current, in case Redo
  }
}

Am_Define_Method(Am_Object_Method, void, single_perform_redo,
		 (Am_Object undo_handler)) {
  Am_Object last_command;
  last_command = undo_handler.Get(Am_COMMAND);
  if (!last_command.Valid ())
    Am_Error("No last command to Redo for Single Redo Handler");
  else {
    do_undo_action_4_children(last_command,
					     Am_REDO_METHOD);
    // leave the command as current, in case undo this
  }
}

// Used in single undo and Redo handlers in the Am_UNDO_ALLOWED and
// Am_REDO_ALLOWED slots (both use the same function)
Am_Define_Object_Formula (single_undo_allowed_form) {
  Am_Object last_command;
  last_command = self.Get(Am_COMMAND);
  return last_command;
}

////////////////////////////////////////////////////////////
// Multiple Undo
////////////////////////////////////////////////////////////

void do_multiple_register(Am_Object undo_handler, Am_Object command_obj,
			  bool copy) {
  Am_Value_List command_list;
  undo_handler.Make_Unique(Am_COMMAND);
  command_list = undo_handler.Get(Am_COMMAND);
  if (copy) command_obj = copy_command_tree(command_obj);
  if (command_list.Empty()) {
    command_list.Add(command_obj, Am_HEAD); 
    undo_handler.Set(Am_COMMAND, command_list);
  }
  else {
    // false so don't copy the wrapper object
    command_list.Add(command_obj, Am_HEAD, false); 
    undo_handler.Note_Changed(Am_COMMAND);
  }
  
  // if a pending old_command for Redo, then remove it
  Am_Object old_command;
  old_command = undo_handler.Get(Am_LAST_UNDONE_COMMAND);
  if (old_command.Valid ())
    destroy_command_tree(old_command);
  undo_handler.Set(Am_LAST_UNDONE_COMMAND, 0);
}

Am_Define_Method(Am_Register_Command_Method, bool,
		 multiple_undo_register_command,
		 (Am_Object undo_handler, Am_Object command_obj)) {
  if (has_impl_parent_and_set(command_obj)) return false;
  else {
    do_multiple_register(undo_handler, command_obj, true);
    return true;
  }
}
    
Am_Define_Method(Am_Object_Method, void, multiple_perform_undo,
		 (Am_Object undo_handler)) {
  Am_Value_List command_list;
  command_list = undo_handler.Get(Am_COMMAND);
  if (command_list.Empty())
    Am_Error("Command list is empty for Multiple Undo Handler");
  else { // undo the current item and all its Am_IMPLEMENTATION_CHILD 
    command_list.Start();
    Am_Object last_command;
    last_command = command_list.Get();
    // false so don't copy the list
    command_list.Delete(false); // pop it from the undo list
    // now do the undo action
    do_undo_action_4_children(last_command, Am_UNDO_METHOD);

    // remember the command in case Redo
    Am_Object old_command;
    old_command = undo_handler.Get(Am_LAST_UNDONE_COMMAND);
    // destroy any old one saved there
    if (old_command.Valid ())
      destroy_command_tree(old_command);
    // now save the new last_command
    undo_handler.Set(Am_LAST_UNDONE_COMMAND, last_command);
    undo_handler.Note_Changed(Am_COMMAND); //destructively modified value
  }
}

Am_Define_Method(Am_Object_Method, void, multiple_perform_redo,
		 (Am_Object undo_handler)) {
  // if a pending last_command for Redo, then do it
  Am_Object last_command;
  last_command = undo_handler.Get(Am_LAST_UNDONE_COMMAND);
  if (!last_command.Valid ())
    Am_Error("No last command to Redo for Multiple Redo Handler");
  else {
    do_undo_action_4_children(last_command,
					     Am_REDO_METHOD);

    // now put the last_command back on the undo list so can be undone
    Am_Value_List command_list;
    // we know list must exist, or else couldn't have done the first
    // undo, so don't have to check for it to exist here.
    command_list = undo_handler.Get(Am_COMMAND, Am_NO_DEPENDENCY);
    // push new one on the front.  false so don't copy the list
    command_list.Add(last_command, Am_HEAD, false); 
    undo_handler.Note_Changed(Am_COMMAND);  //destructively modified
    // clear out the last_command
    undo_handler.Set(Am_LAST_UNDONE_COMMAND, 0);
  }
}

// Used in multiple undo handlers in the Am_UNDO_ALLOWED slot
Am_Define_Object_Formula (multiple_undo_allowed_form) {
  Am_Value_List command_list;
  command_list = self.Get(Am_COMMAND);
  if (command_list.Empty())  return 0;
  else {
    command_list.Start();
    return command_list.Get();
  }
}

// Used in multiple undo handlers in the Am_REDO_ALLOWED slot
Am_Define_Object_Formula (multiple_redo_allowed_form)
{
  Am_Object last_command;
  last_command = self.Get(Am_LAST_UNDONE_COMMAND);
  return last_command;
}

////////////////////////////////////////////////////////////
// Multiple Undo, with selective undo
////////////////////////////////////////////////////////////

bool Am_Object_Valid_And_Not_Part(Am_Object obj) {
  if (!obj.Valid()) return false;
  if (!(bool)obj.Get(Am_VISIBLE)) return false;
  if (obj.Get_Owner().Valid()) return false; // shouldn't have an owner
  return true;
}
  
bool Am_Valid_and_Visible_List_Or_Object(Am_Value value,
					 bool want_visible) {
  if (!value.Valid()) return true;   //if no value, then OK
  if (value.type == Am_OBJECT) {
    Am_Object obj;
    obj = value;
    if (want_visible)
      return Am_Object_And_Owners_Valid_And_Visible(obj);
    else return Am_Object_Valid_And_Not_Part(obj);
  }
//// BUG: Need to move add_ref to setter to avoid this annoyance
  else if (Am_Type_Class (value.type) == Am_WRAPPER &&
	   Am_Value_List::Test(value.value.wrapper_value)) {
    // test each value and return true if all ok
    Am_Value_List l;
    l = value;
    Am_Object o;
    for (l.Start(); !l.Last(); l.Next()) {
      o = l.Get();
      if (want_visible) {
	if (!Am_Object_And_Owners_Valid_And_Visible(o)) 
	  return false;
      } 
      else //want it to be valid but invisible
	if (!Am_Object_Valid_And_Not_Part(o))
	  return false;
    }
    return true; //all are ok
  }
  else return true; //slot does not contain an object
}

Am_Define_Method(Am_Selective_Allowed_Method, bool,
		 Am_Standard_Selective_Allowed, (Am_Object command_obj)) {
  if (!command_obj.Valid()) return false;
  Am_Value value;
  value=command_obj.Peek(Am_OBJECT_MODIFIED, Am_NO_DEPENDENCY);
  bool ret = Am_Valid_and_Visible_List_Or_Object(value);
  //cout << "*** valid and vis returns " << ret << " for obj mod " << value
  //     << " in command " << command_obj << endl << flush;
  return ret;
}

Am_Define_Method(Am_Selective_New_Allowed_Method, bool,
		 Am_Standard_Selective_New_Allowed,
		 (Am_Object command_obj, Am_Value new_selection)) {
  if (Am_Valid_and_Visible_List_Or_Object(new_selection))
    return Am_Check_One_Or_More_For_Inactive_Slot(new_selection, command_obj);
  else return false;
}

////// For the Undo Handler slots of selective undo handlers

//if command_obj or its first child that has a method say its OK
Am_Define_Method(Am_Selective_Allowed_Method, bool,
		 handler_selective_undo_allowed, (Am_Object command_obj)) {
  Am_Selective_Allowed_Method allowed_method;
  Am_Object_Method method;
  Am_Value value;
  while (true) {
    if (!command_obj.Valid()) return false;
    allowed_method = command_obj.Get(Am_SELECTIVE_UNDO_ALLOWED, Am_NO_DEPENDENCY);
    if (allowed_method.Valid())
      if(!(allowed_method.Call(command_obj))) return false;
    method = command_obj.Get(Am_SELECTIVE_UNDO_METHOD, Am_NO_DEPENDENCY);
    if (method.Valid()) return true;
    value=command_obj.Peek(Am_IMPLEMENTATION_CHILD, Am_NO_DEPENDENCY);
    if (value.Valid() && value.type == Am_OBJECT)
      command_obj = value;
    else return false;
  }
}

//if command_obj or its first child that has a method say its OK
Am_Define_Method(Am_Selective_Allowed_Method, bool,
		 handler_selective_repeat_same_allowed,
		 (Am_Object command_obj)) {
  Am_Selective_Allowed_Method allowed_method;
  Am_Value value;
  while (true) {
    if (!command_obj.Valid()) return false;
    allowed_method = command_obj.Get(Am_SELECTIVE_REPEAT_SAME_ALLOWED, Am_NO_DEPENDENCY);
    if (allowed_method.Valid())
      if(!(allowed_method.Call(command_obj))) return false;
    Am_Object_Method method;
    method = command_obj.Get(Am_SELECTIVE_REPEAT_SAME_METHOD, Am_NO_DEPENDENCY);
    if (method.Valid()) return true;

    value=command_obj.Peek(Am_IMPLEMENTATION_CHILD, Am_NO_DEPENDENCY);
    if (value.Valid() && value.type == Am_OBJECT)
      command_obj = value;
    else return false;
  }
}

//if command_obj or its first child that has a method say its OK
Am_Define_Method(Am_Selective_New_Allowed_Method, bool,
		 handler_selective_new_allowed,
		 (Am_Object command_obj, Am_Value new_selection)) {
  Am_Selective_New_Allowed_Method allowed_method;
  Am_Value value;
  while (true) {
    if (!command_obj.Valid()) return false;
    allowed_method = command_obj.Get(Am_SELECTIVE_REPEAT_NEW_ALLOWED, Am_NO_DEPENDENCY);
    if (allowed_method.Valid())
      if(!(allowed_method.Call(command_obj, new_selection))) return false;
    Am_Selective_Repeat_New_Method method;
    method = command_obj.Get(Am_SELECTIVE_REPEAT_ON_NEW_METHOD, Am_NO_DEPENDENCY);
    if (method.Valid()) return true;
    value=command_obj.Peek(Am_IMPLEMENTATION_CHILD, Am_NO_DEPENDENCY);
    if (value.Valid() && value.type == Am_OBJECT)
      command_obj = value;
    else return false;
  }
}

Am_Define_Method(Am_Selective_Allowed_Method, bool,
	 Am_Selective_Allowed_Return_True, (Am_Object /* command_obj */)) {
  return true;
}

Am_Define_Method(Am_Selective_Allowed_Method, bool,
		 Am_Selective_Allowed_Return_False, (Am_Object /* cmd */)) {
  return false;
}
Am_Define_Method(Am_Selective_New_Allowed_Method, bool,
		 Am_Selective_New_Allowed_Return_False,
		 (Am_Object /* cmd */, Am_Value /* new_selection */)) {
  return false;
}

Am_Define_Method(Am_Selective_New_Allowed_Method, bool,
		 Am_Selective_New_Allowed_Return_True,
		 (Am_Object /* cmd */, Am_Value /* new_selection */)) {
  return true;
}

//copy command_obj and all children, perform the selective undo action, then
//queue the new (copied) commands onto the front of the command list
Am_Define_Method(Am_Handler_Selective_Undo_Method, void,
		 handler_perform_selective_undo,
		 (Am_Object undo_handler, Am_Object command_obj)) {
  Am_Object new_top = 
    do_undo_action_4_children(command_obj, Am_SELECTIVE_UNDO_METHOD, true);
  do_multiple_register(undo_handler, new_top, false);
}

//copy command_obj and all children, perform the selective repeat action,
//then queue the new (copied) commands onto the front of the command list
Am_Define_Method(Am_Handler_Selective_Undo_Method, void,
		 handler_perform_selective_repeat,
		 (Am_Object undo_handler, Am_Object command_obj)) {
  Am_Object new_top = 
    do_undo_action_4_children(command_obj,
			      Am_SELECTIVE_REPEAT_SAME_METHOD, true);
  do_multiple_register(undo_handler, new_top, false);
}

//copy command_obj and all children, perform the selective repeat on new
//action, then queue the new commands onto the front of the command list
Am_Define_Method(Am_Handler_Selective_Repeat_New_Method, void,
		 handler_perform_selective_repeat_on_new,
		 (Am_Object undo_handler, Am_Object command_obj,
		  Am_Value new_selection)) {
  Am_Object new_top = 
    do_undo_action_4_children
      (command_obj, Am_SELECTIVE_REPEAT_ON_NEW_METHOD, true, new_selection);
  do_multiple_register(undo_handler, new_top, false);
}

////////////////////////////////////////////////////////////
// Inactive slot of objects
////////////////////////////////////////////////////////////

//returns true if valid.  I.e. returns false if object is marked as
//invalid for slot_for_inactive 
bool Am_Check_One_Object_For_Inactive(Am_Object &object,
				      Am_Slot_Key slot_for_inactive) {
  Am_Value value;
  value=object.Peek(Am_INACTIVE_COMMANDS);
  if (value.Valid()) {
    Am_Object cmd = value;
    value = cmd.Peek(slot_for_inactive);
    if (value.Valid()) return false;
    }
  return true;  //if get here, all selected items are fine
}

static Am_Slot_Key get_my_inactive_slot(Am_Object &self) {
  Am_Value value;
  value=self.Peek(Am_CHECK_INACTIVE_COMMANDS, Am_NO_DEPENDENCY);
  if (value.Valid()) {
    value=self.Peek(Am_SLOT_FOR_THIS_COMMAND_INACTIVE, Am_NO_DEPENDENCY);
    if (value.Valid())
      return (Am_Slot_Key)(int)value;
  }
  return 0;
}
//uses Am_SLOT_FOR_THIS_COMMAND_INACTIVE of self
bool Am_Check_One_Object_For_Inactive_Slot(Am_Object &object,
					   Am_Object &self) {
  Am_Slot_Key slot_for_inactive = get_my_inactive_slot(self);
  if (slot_for_inactive != 0)
    return Am_Check_One_Object_For_Inactive(object, slot_for_inactive);
  else return true;
}

//returns false if any items in selection are marked as invalid for
//slot_for_inactive
bool Am_Check_All_Objects_For_Inactive(Am_Value_List &selection,
				       Am_Slot_Key slot_for_inactive) {
  Am_Value value;
  Am_Object obj, cmd;
  for(selection.Start(); !selection.Last(); selection.Next()) {
    obj = selection.Get();
    value=obj.Peek(Am_INACTIVE_COMMANDS, Am_NO_DEPENDENCY);
    if (value.Valid()) {
      cmd = value;
      value = cmd.Peek(slot_for_inactive);
      if (value.Valid()) return false;
    }
  }
  return true;  //if get here, all selected items are fine
}

bool Am_Check_All_Objects_For_Inactive_Slot(Am_Value_List &selection,
					    Am_Object &self) {
  Am_Slot_Key slot_for_inactive = get_my_inactive_slot(self);
  if (slot_for_inactive != 0)
    return Am_Check_All_Objects_For_Inactive(selection, slot_for_inactive);
  else return true;
}

bool Am_Check_One_Or_More_For_Inactive(Am_Value list_or_obj,
				       Am_Slot_Key slot_for_inactive) {
  if (!list_or_obj.Valid()) return true; //if no value, then OK
  if (list_or_obj.type == Am_OBJECT) {
    Am_Object obj;
    obj = list_or_obj;
    return Am_Check_One_Object_For_Inactive(obj, slot_for_inactive);
  }
  else if (Am_Value_List::Test(list_or_obj)) {
    Am_Value_List vl;
    vl = list_or_obj;
    return Am_Check_All_Objects_For_Inactive(vl, slot_for_inactive);
  }
  else Am_ERROR("Wrong type value " << list_or_obj
		<< ".  Should be obj or list");
  return false; // never gets here
}
bool Am_Check_One_Or_More_For_Inactive_Slot(Am_Value list_or_obj,
					    Am_Object &self) {
  Am_Slot_Key slot_for_inactive = get_my_inactive_slot(self);
  if (slot_for_inactive != 0)
    return Am_Check_One_Or_More_For_Inactive(list_or_obj, slot_for_inactive);
  else return true;
}

//returns false if inactive so should abort, true if OK
bool Am_Check_Inter_Abort_Inactive_Object(Am_Object &object,
					  Am_Slot_Key slot_for_inactive,
					  Am_Object &inter) {
  Am_Value value;
  value=inter.Peek(Am_CHECK_INACTIVE_COMMANDS);
  if (value.Valid()) {
    if(!Am_Check_One_Object_For_Inactive(object, slot_for_inactive)) {
      if ((bool)inter.Get(Am_INTER_BEEP_ON_ABORT))
	Am_Beep();
      Am_INTER_TRACE_PRINT(inter, "Inter " << inter
	 << " aborting because Am_SLOT_FOR_THIS_COMMAND_INACTIVE of object "
	 << object);
      return false;
    }
  }
  return true;
}

////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////

Am_Object Am_Command;  // base of the command hierarchy
Am_Object Am_Undo_Handler; // general, prototype undo handler obj
Am_Object Am_Multiple_Undo_Object;  // can undo all top-level commands
Am_Object Am_Single_Undo_Object;    // can only undo last command

void Am_Initialize_Top_Command () {

  Am_Command = Am_Root_Object.Create ("Am_Command")
    .Add (Am_DO_METHOD, NULL)
    .Add (Am_UNDO_METHOD, NULL)
    .Add (Am_REDO_METHOD, NULL)
    .Add (Am_SELECTIVE_UNDO_METHOD, NULL)
    .Add (Am_SELECTIVE_REPEAT_SAME_METHOD, NULL)
    .Add (Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL)
    .Add (Am_SELECTIVE_UNDO_ALLOWED, Am_Standard_Selective_Allowed)
    .Add (Am_SELECTIVE_REPEAT_SAME_ALLOWED, Am_Standard_Selective_Allowed)
    .Add (Am_SELECTIVE_REPEAT_NEW_ALLOWED,
	  Am_Standard_Selective_New_Allowed)

    .Add (Am_START_DO_METHOD, NULL)
    .Add (Am_INTERIM_DO_METHOD, NULL)
    .Add (Am_ABORT_DO_METHOD, NULL)

    .Add (Am_LABEL, "A command")
    .Add (Am_SHORT_LABEL, 0) //if 0 then use Am_LABEL
    .Add (Am_ACTIVE, true)
    .Add (Am_DEFAULT, false)
    .Add (Am_IMPLEMENTATION_PARENT, 0)
    .Add (Am_IMPLEMENTATION_CHILD, 0)
    .Add (Am_COMPOSITE_PARENT, 0)
    .Add (Am_COMPOSITE_CHILDREN, 0)
    .Add (Am_VALUE, 0)
    .Add (Am_OLD_VALUE, 0)
    .Add (Am_OBJECT_MODIFIED, 0)
    
    .Add (Am_SAVED_OLD_OWNER, NULL)
    .Set_Inherit_Rule(Am_SAVED_OLD_OWNER, Am_LOCAL)
    
    .Add (Am_SAVED_OLD_OBJECT_OWNER, NULL)

    .Add (Am_CHECK_INACTIVE_COMMANDS, true)

    // used by menus
    .Add (Am_ID, Am_No_Value)
    .Add (Am_ITEMS, 0)
    .Add (Am_ACCELERATOR, 0)

    .Add (Am_HAS_BEEN_UNDONE, false)
   ;

  ///////////// now the Undo objects ////////////////////////////////

  Am_Undo_Handler = Am_Root_Object.Create ("Am_Undo_Handler")
    .Add (Am_REGISTER_COMMAND, NULL)
    .Add (Am_PERFORM_UNDO, NULL)
    .Add (Am_PERFORM_REDO, NULL)
    .Add (Am_UNDO_ALLOWED, NULL)
    .Add (Am_REDO_ALLOWED, NULL)
    .Add (Am_COMMAND, 0)

    .Add (Am_SELECTIVE_UNDO_METHOD, NULL)
    .Add (Am_SELECTIVE_REPEAT_SAME_METHOD, NULL)
    .Add (Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL)

    .Add (Am_SELECTIVE_UNDO_ALLOWED, NULL)
    .Add (Am_SELECTIVE_REPEAT_SAME_ALLOWED, NULL)
    .Add (Am_SELECTIVE_REPEAT_NEW_ALLOWED, NULL)
    ;
  Am_Single_Undo_Object = Am_Undo_Handler.Create("Am_Single_Undo_Object")
    .Set (Am_REGISTER_COMMAND, single_undo_register_command)
    .Set (Am_PERFORM_UNDO, single_perform_undo)
    .Set (Am_PERFORM_REDO, single_perform_redo)
    .Set (Am_UNDO_ALLOWED, single_undo_allowed_form)
    .Set (Am_REDO_ALLOWED, single_undo_allowed_form)
    ;
  Am_Multiple_Undo_Object = Am_Undo_Handler.Create("Am_Multiple_Undo_Object")
    .Set (Am_REGISTER_COMMAND, multiple_undo_register_command)
    .Set (Am_PERFORM_UNDO, multiple_perform_undo)
    .Set (Am_PERFORM_REDO, multiple_perform_redo)
    .Set (Am_UNDO_ALLOWED, multiple_undo_allowed_form)
    .Set (Am_REDO_ALLOWED, multiple_redo_allowed_form)
    .Add (Am_LAST_UNDONE_COMMAND, 0)

    .Set (Am_SELECTIVE_UNDO_METHOD, handler_perform_selective_undo)
    .Set (Am_SELECTIVE_REPEAT_SAME_METHOD,handler_perform_selective_repeat)
    .Set (Am_SELECTIVE_REPEAT_ON_NEW_METHOD,
	  handler_perform_selective_repeat_on_new)

    .Set (Am_SELECTIVE_UNDO_ALLOWED, handler_selective_undo_allowed)
    .Set (Am_SELECTIVE_REPEAT_SAME_ALLOWED, handler_selective_repeat_same_allowed)
    .Set (Am_SELECTIVE_REPEAT_NEW_ALLOWED, handler_selective_new_allowed)
    ;
}
