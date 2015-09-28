/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the functions for handling the text Interactor
   
   Designed and implemented by Alan Ferrency
*/

#include <am_inc.h>

#include AM_IO__H

#include INTER_ADVANCED__H
#include STANDARD_SLOTS__H
#include VALUE_LIST__H
#include OPAL_ADVANCED__H

#include TYPES__H
#include OPAL__H
#include TEXT_FNS__H
#include REGISTRY__H

//////////////////
// text interactor object functions
//////////////////

//x and y should be w.r.t. text's window
void move_text_cursor(Am_Object text, int x, int y) {
  int index = Am_Get_Cursor_Index (text, x, y);
  Am_Move_Cursor_To (text, index);
}

void set_initial_text_values(Am_Object &inter, Am_Object &cmd,
			     Am_Object &obj, int x, int y) {
  Am_Value value;
  value=obj.Peek(Am_TEXT);
  if (!value.Valid() || value.type != Am_STRING) {
    Am_ERRORO("For text interactor " << inter
	      << " the Am_TEXT slot of its object " << obj
	      << " must contain a value of type string, but it contains "
	      << value,
	      obj, Am_TEXT);
  }
  inter.Set(Am_OLD_VALUE, value, Am_OK_IF_NOT_THERE);
  cmd.Set(Am_OLD_VALUE, value);
  move_text_cursor(obj, x, y);
}

Am_Object text_set_impl_command(Am_Object inter,
				Am_Object object_modified,
				Am_Inter_Location /* data */) {
  Am_Object impl_command;
  impl_command = inter.Get_Object(Am_IMPLEMENTATION_COMMAND);
  if(impl_command.Valid()) {
    Am_String text_string;
    impl_command.Set(Am_OBJECT_MODIFIED, object_modified);

    text_string = inter.Get(Am_OLD_VALUE);
    impl_command.Set(Am_OLD_VALUE, (Am_Wrapper*)text_string);
    text_string = inter.Get(Am_VALUE);
    impl_command.Set(Am_VALUE, (Am_Wrapper*)text_string);
  }
  return impl_command;
}

Am_Text_Abort_Or_Stop_Code am_check_text_legal(Am_Object &inter,
					       Am_Object &text) {
  Am_Text_Abort_Or_Stop_Code code = Am_TEXT_OK;
  Am_Value v;
  v=inter.Peek(Am_TEXT_CHECK_LEGAL_METHOD);
  if (v.Valid()) {
    Am_Text_Check_Legal_Method check_method = v;
    code = check_method.Call(text, inter);
  }
  return code;
}

//////////////////////////////////////////////////////////////////
/// Main method routines
//////////////////////////////////////////////////////////////////

Am_Define_Method(Am_Inter_Internal_Method, void, Am_Text_Start_Method,
		 (Am_Object& inter, Am_Object& object,
		  Am_Object& event_window, Am_Input_Event *ev)) {
  Am_INTER_TRACE_PRINT(inter, "Text starting over " << object);
  
  // in case last time ran, stopped because pressed outside
  int real_run = inter.Get(Am_REAL_RUN_ALSO);
  if (real_run == 1) inter.Set(Am_RUN_ALSO, true);
  else if (real_run == 0) inter.Set(Am_RUN_ALSO, false);
  inter.Set(Am_REAL_RUN_ALSO, 3);

  //do this before prototype's method, so won't even change state
  if (!Am_Check_Inter_Abort_Inactive_Object(object, Am_TEXT_EDIT_INACTIVE,
					    inter))
    return;

  // first, call the prototype's method
  Am_Inter_Internal_Method inter_method;
  inter_method = Am_Interactor.Get(Am_INTER_START_METHOD);
  inter_method.Call(inter, object, event_window, ev);

  Am_Object command_obj;
  command_obj = inter.Get(Am_COMMAND);
  set_initial_text_values (inter, command_obj, object, ev->x, ev->y);

  //actually, there isn't an internal_command method for start, but that's ok
  Am_Inter_Call_Both_Method(inter, command_obj, Am_START_DO_METHOD,
		    ev->x, ev->y, event_window, ev->input_char, object, Am_No_Location);
}

Am_Define_Method(Am_Inter_Internal_Method, void, Am_Text_Outside_Method,
		 (Am_Object& inter, Am_Object& object,
		  Am_Object& event_window, Am_Input_Event *ev)) {
  // first, call the prototype's method
  Am_Inter_Internal_Method inter_method;
  inter_method = Am_Interactor.Get(Am_INTER_OUTSIDE_METHOD);
  inter_method.Call(inter, object, event_window, ev);

  // hide cursor on outside method. Only do it once to avoid unneccessary sets
  Am_Object command_obj, text;
  command_obj = inter.Get(Am_COMMAND);
  text = inter.Get(Am_START_OBJECT);
  int ci = 0;
  if (text.Valid () && (ci = text.Get(Am_CURSOR_INDEX)) > 0) {
    inter.Set(Am_CURSOR_INDEX, ci, Am_OK_IF_NOT_THERE);
    text.Set(Am_CURSOR_INDEX, Am_NO_CURSOR);
  }
  //don't call abort here since would restore the text string to its
  //original value, and don't want that
}

Am_Define_Method(Am_Inter_Internal_Method, void, Am_Text_Back_Inside_Method,
		 (Am_Object& inter, Am_Object& object,
		  Am_Object& event_window, Am_Input_Event *ev)) {
  // first, call the prototype's method
  Am_Inter_Internal_Method inter_method;
  inter_method = Am_Interactor.Get(Am_INTER_BACK_INSIDE_METHOD);
  inter_method.Call(inter, object, event_window, ev);

  // bring cursor back if it's invisible.
  Am_Object command_obj, text;
  command_obj = inter.Get(Am_COMMAND);
  text = inter.Get(Am_START_OBJECT);
  int ci = inter.Get(Am_CURSOR_INDEX);
  if (ci > 0) text.Set(Am_CURSOR_INDEX, ci);
  //actually, there isn't an internal_command method for start, but that's ok
  Am_Inter_Call_Both_Method(inter, command_obj, Am_START_DO_METHOD,
		    ev->x, ev->y, event_window, ev->input_char, text, Am_No_Location);
}

Am_Define_Method(Am_Inter_Internal_Method, void, Am_Text_Abort_Method,
		 (Am_Object& inter, Am_Object& object,
		  Am_Object& event_window, Am_Input_Event *ev)) {
  Am_INTER_TRACE_PRINT(inter, "Text Aborting");

  // first, call the prototype's method
  Am_Inter_Internal_Method inter_method;
  inter_method = Am_Interactor.Get(Am_INTER_ABORT_METHOD);
  inter_method.Call(inter, object, event_window, ev);

  Am_Object command_obj, text;
  command_obj = inter.Get(Am_COMMAND);
  text = inter.Get(Am_START_OBJECT);
  Am_Inter_Call_Both_Method(inter, command_obj, Am_ABORT_DO_METHOD,
	    ev->x, ev->y, event_window, ev->input_char, text, Am_No_Location);
}

Am_Define_Method(Am_Inter_Internal_Method, void, Am_Text_Running_Method,
		 (Am_Object& inter, Am_Object& object,
		  Am_Object& event_window, Am_Input_Event *ev)) {
  Am_INTER_TRACE_PRINT(inter, "Text running");
  // first, call the prototype's method
  Am_Inter_Internal_Method inter_method;
  inter_method = Am_Interactor.Get(Am_INTER_RUNNING_METHOD);
  inter_method.Call(inter, object, event_window, ev);
  
  Am_Object command_obj, text;
  command_obj = inter.Get(Am_COMMAND);
  text = inter.Get(Am_START_OBJECT);
  Am_Inter_Call_Both_Method(inter, command_obj, Am_INTERIM_DO_METHOD,
	    ev->x, ev->y, event_window, ev->input_char, text, Am_No_Location);
}

Am_Define_Method(Am_Inter_Internal_Method, void, Am_Text_Stop_Method,
		 (Am_Object& inter, Am_Object& object,
		  Am_Object& event_window, Am_Input_Event *ev)) {
  Am_INTER_TRACE_PRINT(inter, "Text stopping " << object);

  Am_Object command_obj, text;
  command_obj = inter.Get(Am_COMMAND);
  text = inter.Get(Am_START_OBJECT);

  //first check if OK
  Am_Text_Abort_Or_Stop_Code code = am_check_text_legal(inter, text);
  switch (code) {
  case Am_TEXT_OK:
  case Am_TEXT_STOP_ANYWAY: {
    // now call stop
    Am_Call_Final_Do_And_Register(inter, command_obj, ev->x, ev->y,
	     event_window, ev->input_char, text, Am_No_Location,
	     text_set_impl_command);

    // LAST, call the prototype's method
    Am_Inter_Internal_Method inter_method;
    inter_method = Am_Interactor.Get(Am_INTER_STOP_METHOD);
    inter_method.Call(inter, object, event_window, ev);
    break;
  }
  case Am_TEXT_KEEP_RUNNING: break; // just ignore this character
  case Am_TEXT_ABORT_AND_RESTORE:
    // cout << " Calling text abort\n" << flush;
    Am_Text_Abort_Method_proc (inter, object, event_window, ev);
    break;
  default: Am_ERRORO("Illegal code=" << (int)code
     << " returned by method in Am_TEXT_CHECK_LEGAL_METHOD of " << inter,
		     inter, Am_TEXT_CHECK_LEGAL_METHOD);
  }
}

//////////////////////////////////////////////////////////
// Text interactor's command object's method functions. //
//////////////////////////////////////////////////////////

bool text_has_been_edited(Am_Object inter, Am_Object text) {
  Am_String orig_text, new_text;
  orig_text = inter.Get(Am_OLD_VALUE);
  new_text = text.Get(Am_TEXT);
  return !(orig_text == new_text);
}

Am_Define_Method(Am_Mouse_Event_Method, void, Am_Text_Interim_Do,
		 (Am_Object inter, int mouse_x, int mouse_y,
		  Am_Object ref_obj, Am_Input_Char ic)) {
  Am_Object text;
  text = inter.Get(Am_START_OBJECT);

  // move cursor if another mouse down event occurs
  // in the OBJECT_MODIFIED.
  if ((ic.button_down == Am_BUTTON_DOWN) &&
      (ic.click_count >= Am_SINGLE_CLICK )) {
    if (Am_Point_In_Obj (text, mouse_x, mouse_y, ref_obj)) {
      if (ic.click_count >= Am_DOUBLE_CLICK &&
	  (bool)inter.Get(Am_WANT_PENDING_DELETE)) {
	Am_Set_Pending_Delete(text, true);
	Am_Set_Cut_Buffer_From_Text(text);
      }
      else move_text_cursor (text, mouse_x, mouse_y);
    }
    else { // click outside the object, exit interactor, but save event
      //check to see if current value is valid first
      Am_Text_Abort_Or_Stop_Code code = am_check_text_legal(inter, text);
      switch (code) {
      case Am_TEXT_OK:
      case Am_TEXT_STOP_ANYWAY: {
	if ((bool)inter.Get(Am_RUN_ALSO)) inter.Set(Am_REAL_RUN_ALSO, 1);
	else inter.Set(Am_REAL_RUN_ALSO, 0);
	inter.Set(Am_RUN_ALSO, true);
	//if the current string is different from the initial string, then call
	// stop_interactor, otherwise, abort the interactor (so stop
	// method not called if just clicking around)
	if (text_has_been_edited(inter, text)) {
	  //stop the interactor, but make sure the stop doesn't call
	  //the check again
	  Am_Value old_method = inter.Peek(Am_TEXT_CHECK_LEGAL_METHOD);
	  inter.Set(Am_TEXT_CHECK_LEGAL_METHOD, NULL);
	  Am_Stop_Interactor(inter, text, ic);
	  if (old_method.Valid())
	    inter.Set(Am_TEXT_CHECK_LEGAL_METHOD, old_method);
	}
	else Am_Abort_Interactor(inter);
	return;
      }
      case Am_TEXT_KEEP_RUNNING: return; // just ignore this character
      case Am_TEXT_ABORT_AND_RESTORE:
	Am_Abort_Interactor(inter);   
	return;
      default: Am_ERROR("Illegal code=" << (int)code);
      }//end switch
    }
  }
  Am_Text_Edit_Method text_edit_method;
  text_edit_method = inter.Get(Am_TEXT_EDIT_METHOD);
  text_edit_method.Call(text, ic, inter);
}

Am_Define_Method(Am_Object_Method, void, Am_Text_Abort_Do,
		 (Am_Object inter)) {
  Am_Object text;
  text = inter.Get(Am_START_OBJECT);
  if (text.Valid ()) {
    Am_Set_Pending_Delete(text, false);
    Am_String orig_text;
    orig_text = inter.Get(Am_OLD_VALUE);
    text.Set (Am_TEXT, (Am_Wrapper*)orig_text);
    text.Set (Am_CURSOR_INDEX, Am_NO_CURSOR);
  }
}

Am_Define_Method(Am_Object_Method, void, Am_Text_Do, (Am_Object inter)) {
  // put a copy of the finally edited text into Am_VALUE slot of command obj.
  // then put empty stuff in other slots.
  Am_Object text;
  Am_String edited_text;
  text = inter.Get(Am_START_OBJECT);
  Am_Set_Pending_Delete(text, false);
  edited_text = text.Get(Am_TEXT);
  inter.Set (Am_VALUE, (Am_Wrapper*)edited_text);
  inter.Set(Am_OBJECT_MODIFIED, text);
  text.Set (Am_CURSOR_INDEX, Am_NO_CURSOR);
  Am_Copy_Values_To_Command(inter);
}

//////////////////////////////////////////////////////////////////////////////
// Undo and Redo methods for Command
//////////////////////////////////////////////////////////////////////////////

void text_general_undo_redo(Am_Object command_obj, bool undo, bool selective,
		       bool reload_data, Am_Object obj) {
  Am_Object inter;
  inter = command_obj.Get(Am_SAVED_OLD_OWNER);
  if (reload_data) command_obj.Set(Am_OBJECT_MODIFIED, obj);
  else obj = command_obj.Get(Am_OBJECT_MODIFIED);

  #ifdef DEBUG
  if (inter.Valid () && Am_Inter_Tracing(inter)) {
    if (selective) cout << "Selective ";
    if (undo) cout << "Undo"; else cout << "Repeat";
    cout << " command " << command_obj << " on obj " << obj << endl << flush;
  }
  #endif

  Am_String current_text, orig_text;
  if (selective) current_text = obj.Get(Am_TEXT);
  else current_text = command_obj.Get (Am_VALUE);
  if (undo) orig_text = command_obj.Get(Am_OLD_VALUE);
  else orig_text = command_obj.Get(Am_VALUE);
    
  obj.Set (Am_TEXT, (Am_Wrapper*)orig_text);

  command_obj.Set (Am_VALUE, (Am_Wrapper*)orig_text);
  command_obj.Set (Am_OLD_VALUE, (Am_Wrapper*)current_text);
  if (inter.Valid()) inter.Set (Am_VALUE, (Am_Wrapper*)orig_text);
}
  
Am_Define_Method(Am_Object_Method, void, Am_Text_Command_Undo,
		 (Am_Object command_obj)) {
  text_general_undo_redo(command_obj, true, false, false, Am_No_Object);
}

//Am_Text_Command_Redo same as Am_Text_Command_Undo

//command obj is a copy of the original, so can modify it
Am_Define_Method(Am_Object_Method, void, Am_Text_Command_Selective_Undo,
		 (Am_Object command_obj)){
  text_general_undo_redo(command_obj, true, true, false, Am_No_Object);
}
 
//command obj is a copy of the original, so can modify it
Am_Define_Method(Am_Object_Method, void, Am_Text_Command_Repeat_Same,
		 (Am_Object command_obj)){
  text_general_undo_redo(command_obj, false, true, false, Am_No_Object);
}

//command obj is a copy of the original, so can modify it
Am_Define_Method(Am_Selective_Repeat_New_Method, void,
		 Am_Text_Command_Repeat_New,
		 (Am_Object command_obj, Am_Value new_selection)){
  Am_Object new_obj;
  if (new_selection.type == Am_OBJECT) new_obj = new_selection;
  else Am_Error("Am_Text_Command_Repeat_New called with non-object");
  if (new_obj.Is_Instance_Of(Am_Text))
    text_general_undo_redo(command_obj, false, true, true, new_obj);
  // else don't do anything
}

////////////////////////////////////////////////////////////
//global variables

Am_Object Am_Text_Edit_Internal_Command;
Am_Object Am_Text_Edit_Interactor;  // text editing

void Am_Initialize_Text_Interactor () {
  
  Am_Text_Edit_Internal_Command =
    Am_Command.Create("Am_Edit_Text_Internal_Command")
    .Set (Am_LABEL, "Edit_Text")
    .Set (Am_UNDO_METHOD, Am_Text_Command_Undo)
    .Set (Am_REDO_METHOD, Am_Text_Command_Undo) //works for both
    .Set (Am_SELECTIVE_UNDO_METHOD, Am_Text_Command_Selective_Undo)
    .Set (Am_SELECTIVE_REPEAT_SAME_METHOD, Am_Text_Command_Repeat_Same)
    .Set (Am_SELECTIVE_REPEAT_ON_NEW_METHOD, Am_Text_Command_Repeat_New)
    ;

  Am_Text_Edit_Interactor = Am_Interactor.Create("Am_Text_Interactor")
     .Set (Am_INTER_START_METHOD, Am_Text_Start_Method)
     .Set (Am_INTER_ABORT_METHOD, Am_Text_Abort_Method)
     .Set (Am_INTER_OUTSIDE_METHOD, Am_Text_Outside_Method)
     // default outside stop method is fine
     .Set (Am_INTER_BACK_INSIDE_METHOD, Am_Text_Back_Inside_Method)
     .Set (Am_INTER_RUNNING_METHOD, Am_Text_Running_Method)
     .Set (Am_INTER_STOP_METHOD, Am_Text_Stop_Method)
     
     // no text_start_do needed
     .Set (Am_INTERIM_DO_METHOD, Am_Text_Interim_Do)
     .Set (Am_ABORT_DO_METHOD, Am_Text_Abort_Do)
     .Set (Am_DO_METHOD, Am_Text_Do)

     .Set (Am_START_WHERE_TEST, Am_Inter_In_Text_Object_Or_Part)
     .Set (Am_STOP_WHEN, Am_Input_Char("RETURN"))
     .Add (Am_TEXT_EDIT_METHOD, Am_Default_Text_Edit_Method)
     .Add (Am_EDIT_TRANSLATION_TABLE,
	   Am_Edit_Translation_Table::Default_Table ())
     .Add (Am_TEXT_CHECK_LEGAL_METHOD, 0)
     .Add (Am_WANT_PENDING_DELETE, false)
     .Add (Am_REAL_RUN_ALSO, 3)
     .Add_Part(Am_IMPLEMENTATION_COMMAND,
	   Am_Text_Edit_Internal_Command.Create("Text_Internal_Command"))
     ;
  Am_Text_Edit_Interactor.Get_Object(Am_COMMAND)
  .Set (Am_LABEL, "Text_Edit")
  .Set_Name ("Am_Command_in_Text_Edit")
  ;
}
