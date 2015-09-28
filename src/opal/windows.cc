/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifdef GCC
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#else
extern "C" {
#include <stdlib.h>
#include <limits.h> //INT_MIN
#if defined(_MSC_VER) || defined(NEED_STRING)
#include <string.h>
#endif
}
#endif

#include <am_inc.h>

#include FORMULA_ADVANCED__H
#include STANDARD_SLOTS__H
#include OPAL_ADVANCED__H
#include VALUE_LIST__H
#include INTER_ADVANCED__H // to get Interactor_Input_Event_Notify
#include PRIORITY_LIST__H // to get Am_Priority_List type

#include FORMULA__H
#include GEM__H
#include OPAL__H
#include WEB__H

Am_Define_Method_Type_Impl(Am_Timer_Method);

void Am_Draw (Am_Object object, Am_Drawonable* drawonable,
          int x_offset, int y_offset)
{
  Am_Draw_Method draw = object.Get (Am_DRAW_METHOD);
  draw.Call (object, drawonable, x_offset, y_offset);
}

void Am_Invalidate (Am_Object owner, Am_Object which_part,
            int left, int top, int width, int height)
{
#ifdef DEBUG
// extra test to see if owner is not a group (or window), then doesn't have an
// Invalidate method
  Am_Value v;
  v=owner.Peek(Am_INVALID_METHOD, Am_NO_DEPENDENCY);
  if (v.Valid()) {
    Am_Invalid_Method invalidate = v;
    invalidate.Call (owner, which_part, left, top, width, height);
  }
  else Am_ERRORO("Apparently added a part " << which_part <<
        " to a non-group-type object " << owner, owner, 0);
#else
  Am_Invalid_Method invalidate = owner.Get(Am_INVALID_METHOD);
  invalidate.Call (owner, which_part, left, top, width, height);
#endif
}

Am_State_Store* Am_State_Store::invalidation_list = NULL;
bool Am_State_Store::shutdown = false;

Am_State_Store::Am_State_Store (Am_Object in_self, Am_Object in_owner,
                    bool in_visible, int in_left, int in_top,
                    int in_width, int in_height)
{
  self = in_self;
  owner = in_owner;
  visible = in_visible;
  left = in_left;
  top = in_top;
  width = in_width;
  height = in_height;
  in_list = false;
  needs_update = false;
//// DEBUG
//cout << "New State " << *self << " <l" << left << ", t" << top << ", w"
//     << width << ", h" << height << ">" << endl;
}
                 
#ifdef AMULET2_INSTRUMENT
#undef Add
#endif
void Am_State_Store::Add (bool in_needs_update)
#ifdef AMULET2_INSTRUMENT
#define Add Am_Instrumented(Add)
#endif
{
  if (!in_list) {
    in_list = true;
    next = invalidation_list;
    invalidation_list = this;
  }
  needs_update = needs_update | in_needs_update;
}

void Am_State_Store::Remove ()
{
  if (in_list) {
    Invalidate ();
    Am_State_Store* prev = NULL;
    Am_State_Store* current = invalidation_list;
    while (current) {
      if (current == this) {
    if (prev)
      prev->next = next;
    else
      invalidation_list = next;
    return;
      }
      prev = current;
      current = current->next;
    }
  }
}

void Am_State_Store::Invalidate ()
{
  if (owner.Valid() && visible)
    Am_Invalidate (owner, self, left, top, width, height);
  if (needs_update && self.Valid () && !shutdown) {
    needs_update = false;
    owner = self.Get_Owner (Am_NO_DEPENDENCY);
    visible = self.Get (Am_VISIBLE,
			Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    left = self.Get (Am_LEFT, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    top = self.Get (Am_TOP, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    width = self.Get (Am_WIDTH, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    height = self.Get (Am_HEIGHT, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    if (owner.Valid() && visible)
      Am_Invalidate (owner, self, left, top, width, height);
  }
  in_list = false;
}

bool Am_State_Store::Visible (Am_Drawonable* drawonable,
                  int x_offset, int y_offset)
{
  if (visible) {
    bool total;
    return drawonable->In_Clip (left + x_offset, top + y_offset,
                width, height, total);
  }
  else
    return false;
}

//if debugging, then checks to see whether trying to invalidate a window
//after it was already started to invalidate, but didn't finish, which means
//that it crashed last time, in which case
//it doesn't try to invalidate it again, but only one time.

void Am_State_Store::Invoke () {
#ifdef DEBUG
  Am_State_Store* current = invalidation_list;
  Am_Object obj;
  while (current) {
    obj = current->self;
    int was_inprogress=obj.Get(Am_OBJECT_IN_PROGRESS, Am_RETURN_ZERO_ON_ERROR);
    if (was_inprogress & 1) {
      cerr << "** Invalidate on object " << obj
	   << " but crashed last time, so skipping it.\n" << flush;
      obj.Set(Am_OBJECT_IN_PROGRESS, 0);
    }
    else {
      obj.Set(Am_OBJECT_IN_PROGRESS, 1, Am_OK_IF_NOT_THERE);
      current->Invalidate ();
      obj.Set(Am_OBJECT_IN_PROGRESS, 0);
    }
    current = current->next;
  }
  invalidation_list = NULL;
}
#else  //not debugging
  Am_State_Store* current = invalidation_list;
  while (current) {
    current->Invalidate ();
    current = current->next;
  }
  invalidation_list = NULL;
}
#endif

Am_Define_Object_Formula (pass_window)
{
  Am_Object owner = self.Get_Owner ();
  if (owner.Valid() && (owner.Is_Instance_Of (Am_Graphical_Object) ||
        owner.Is_Instance_Of (Am_Window)))
    return owner.Get (Am_WINDOW);
  else
    return NULL;
}

static Am_Wrapper* return_self_proc ( Am_Object& self)
{
  return self;
}
Am_Formula return_self (return_self_proc, "return_self");
                        

Am_Define_Formula(bool, window_is_color)
{
  Am_Drawonable *d = Am_Drawonable::Narrow(self.Get(Am_DRAWONABLE));
// is there a better way to determine if d is valid?
  if (d) return d->Is_Color();
  else return true;
}

Am_Define_Formula (int, compute_depth)
{
  Am_Object owner = self.Get_Owner ();
  if (owner.Valid ()) {
    int depth = owner.Get (Am_OWNER_DEPTH);
    if (depth == -1)
      return -1;
    else
      return depth + 1;
  }
  else
    return -1;
}

Am_Window_ToDo* Window_ToDo_Head = NULL;
Am_Window_ToDo* Window_ToDo_Tail = NULL;

void Am_Window_ToDo::Merge_Rectangle (int in_left, int in_top, int in_width,
                      int in_height)
{
  if (width && height) {
    int far_left = (in_left < left) ? in_left : left;
    int far_top = (in_top < top) ? in_top : top;
    int far_width = ((in_left + in_width) > (left + width)) ?
                    in_left + in_width - far_left :
                    left + width - far_left;
    int far_height = ((in_top + in_height) > (top + height)) ?
                    in_top + in_height - far_top :
                    top + height - far_top;
    left = far_left;
    top = far_top;
    width = far_width;
    height = far_height;
  }
  else {
    left = in_left;
    top = in_top;
    width = in_width;
    height = in_height;
  }
}

#ifdef AMULET2_INSTRUMENT
#undef Add
#endif
void Am_Window_ToDo::Add ()
#ifdef AMULET2_INSTRUMENT
#define Add Am_Instrumented(Add)
#endif
{
  if (!prev && !next && (Window_ToDo_Head != this)) {
    prev = Window_ToDo_Tail;
    if (Window_ToDo_Tail)
      Window_ToDo_Tail->next = this;
    else
      Window_ToDo_Head = this;
    Window_ToDo_Tail = this;
  }
}

void Am_Window_ToDo::Remove ()
{
  if (prev || next || (Window_ToDo_Head == this)) {
    if (next)
      next->prev = prev;
    else
      Window_ToDo_Tail = prev;
    if (prev)
      prev->next = next;
    else
      Window_ToDo_Head = next;
    prev = NULL;
    next = NULL;
  }
}

// Called when an object is copied or created.
// components is the list of inherited graphical parts,
// parts = local parts (all parts the object has)
// We need to rename the parts because they're now instances.
// Also sets rank of parts.

void am_generic_renew_components (Am_Object object)
{
  Am_Value_List components;
  components = object.Get (Am_GRAPHICAL_PARTS);
  Am_Value_List new_components;
  Am_Part_Iterator parts = object;
  int parts_length = parts.Length ();
  // if the object has no inherited graphical parts (all the parts are
  // non-graphical) or no parts, then the new object will still have no
  // graphical parts, so exit quickly.
  if (components.Empty () || (parts_length == 0)) {
    object.Set(Am_GRAPHICAL_PARTS, Am_Value_List());
    return;
  }
  Am_Object* part_map = new Am_Object [parts_length*2];
  Am_Object part;
  int i;
  for (i = 0, parts.Start(); !parts.Last (); ++i, parts.Next ()) {
    part = parts.Get ();
    part_map[i+parts_length] = part;
    part_map[i] = part.Get_Prototype ();
  }
  Am_Object current_component;
  int current_rank = 0;
  for (components.Start(); !components.Last (); components.Next ()) {
    current_component = components.Get ();
    if (current_component.Get_Key () == Am_NO_INHERIT)
      continue;
    for (i = 0; i < parts_length; ++i)
      if (current_component == part_map[i]) {
        part_map[i+parts_length].Set (Am_RANK, current_rank);
        new_components.Add (part_map[i+parts_length]);
        ++current_rank;
        break;
      }
  }
  delete [] part_map;
  object.Set (Am_GRAPHICAL_PARTS, new_components);
}

void am_generic_renew_copied_comp (Am_Object object)
{
  Am_Value_List components;
  components = object.Get (Am_GRAPHICAL_PARTS);
  Am_Value_List new_components;
  Am_Part_Iterator parts = object;
  int parts_length = parts.Length ();
  if (components.Empty () || (parts_length == 0)) {
    object.Set(Am_GRAPHICAL_PARTS, Am_Value_List());
    return;
  }
  Am_Object* part_map = new Am_Object [parts_length*2];
  Am_Object part;
  int i;
  for (i = 0, parts.Start(); !parts.Last (); ++i, parts.Next ()) {
    part = parts.Get ();
    part_map[i+parts_length] = part;
    part_map[i] = part.Get (Am_SOURCE_OF_COPY);
  }
  Am_Object current_component;
  int current_rank = 0;
  for (components.Start(); !components.Last (); components.Next ()) {
    current_component = components.Get ();
    if (current_component.Get_Key () == Am_NO_INHERIT)
      continue;
    for (i = 0; i < parts_length; ++i)
      if (current_component == part_map[i]) {
        part_map[i+parts_length].Set (Am_RANK, current_rank);
        new_components.Add (part_map[i+parts_length]);
        ++current_rank;
        break;
      }
  }
  delete [] part_map;
  object.Set (Am_GRAPHICAL_PARTS, new_components);
}

static void screen_destroy_demon (Am_Object screen)
{
  Am_Drawonable* drawonable =
      Am_Drawonable::Narrow (screen.Get (Am_DRAWONABLE));
  if (drawonable) {
    drawonable->Set_Data_Store (NULL);
    drawonable->Destroy ();
    screen.Set (Am_DRAWONABLE, NULL);
  }
  drawonable = Am_Drawonable::Narrow (screen.Get (Am_OFFSCREEN_DRAWONABLE));
  if (drawonable) {
    drawonable->Set_Data_Store (NULL);
    drawonable->Destroy ();
    screen.Set (Am_OFFSCREEN_DRAWONABLE, NULL);
  }
  Am_Value_List draw_list;
  Am_Value fade_value;
  fade_value=screen.Peek(Am_DRAW_BUFFER);
  if (fade_value.Valid ()) {
    if (Am_Value_List::Test (fade_value)) {
      draw_list = fade_value;
      for (draw_list.Start (); !draw_list.Last (); draw_list.Next ()) {
        drawonable = Am_Drawonable::Narrow (draw_list.Get ());
        if (drawonable) {
          drawonable->Set_Data_Store (NULL);
          drawonable->Destroy ();
        }
      }
    }
    else {
      drawonable = Am_Drawonable::Narrow (fade_value);
      if (drawonable) {
        drawonable->Set_Data_Store (NULL);
        drawonable->Destroy ();
      }
    }
    screen.Set (Am_DRAW_BUFFER, NULL);
  }
  fade_value=screen.Peek(Am_MASK_BUFFER);
  if (fade_value.Valid ()) {
    if (Am_Value_List::Test (fade_value)) {
      draw_list = fade_value;
      for (draw_list.Start (); !draw_list.Last (); draw_list.Next ()) {
        drawonable = Am_Drawonable::Narrow (draw_list.Get ());
        if (drawonable) {
          drawonable->Set_Data_Store (NULL);
          drawonable->Destroy ();
        }
      }
    }
    else {
      drawonable = Am_Drawonable::Narrow (fade_value);
      if (drawonable) {
        drawonable->Set_Data_Store (NULL);
        drawonable->Destroy ();
      }
    }
    screen.Set (Am_MASK_BUFFER, NULL);
  }
}

//default window destroy method destroys the window, and then sees if
//this is the last visible window, and if so, exits the main loop.
//Many applications will want to override this method.
Am_Define_Method(Am_Object_Method, void, Am_Default_Window_Destroy_Method,
         (Am_Object window)) {
  Am_Object screen = window.Get_Owner();
  window.Destroy();
  if (screen.Is_Instance_Of(Am_Screen))  { //otherwise, destroying sub-window
    Am_Part_Iterator part_iter(screen);
    Am_Object part;
    for (part_iter.Start(); !part_iter.Last(); part_iter.Next()) {
      part = part_iter.Get();
      if (part.Is_Instance_Of(Am_Window) &&
      ((bool)part.Get(Am_VISIBLE) ||
       (bool)part.Get(Am_ICONIFIED))) {
    //found a visible or iconified window
    return;
      }
    }
    //if get to here, there are no visible or iconified windows
    Am_Exit_Main_Event_Loop ();
  }
}

Am_Define_Method(Am_Object_Method, void, Am_Window_Hide_Method,
         (Am_Object window)) {
  window.Set(Am_VISIBLE, false);
}

Am_Define_Method(Am_Object_Method, void, Am_Window_Destroy_And_Exit_Method,
         (Am_Object window)) {
  window.Destroy();
  Am_Exit_Main_Event_Loop ();
}

static void window_create (Am_Object window)
{
  Am_Object_Advanced ad_window = (Am_Object_Advanced&)window;
  ad_window.Set (Am_DRAWONABLE, (Am_Ptr)NULL); //##
  Am_Window_ToDo* win_todo = new Am_Window_ToDo;
  win_todo->window = window;
  win_todo->width = 0;
  win_todo->height = 0;
  win_todo->flags = 0;
  win_todo->prev = NULL;
  win_todo->next = NULL;
  ad_window.Add (Am_TODO, (Am_Ptr)win_todo);
  ad_window.Get_Slot (Am_TODO).Set_Inherit_Rule (Am_LOCAL);

  window.Set (Am_RANK, 0);
  am_generic_renew_components (window);

//// NDY: window might be created as a part already (currently doesn't matter)
}

//demon procedure
static void window_copy (Am_Object window) {
  window.Set (Am_DRAWONABLE, NULL);
  Am_Window_ToDo* win_todo = new Am_Window_ToDo;
  win_todo->window = window;
  win_todo->width = 0;
  win_todo->height = 0;
  win_todo->flags = 0;
  win_todo->prev = NULL;
  win_todo->next = NULL;
  window.Add (Am_TODO, (Am_Ptr)win_todo);
  am_generic_renew_copied_comp (window);

//// NDY: window might be created as a part already (currently doesn't matter)
}

//demon procedure
static void window_destroy_demon (Am_Object window) {
  if (window == Am_Window)
    return;
  Am_Part_Iterator parts = window;
  Am_Object part;
  parts.Start (); 
  while (!parts.Last ()) {
    part = parts.Get ();
    parts.Next ();
    if (part.Is_Instance_Of (Am_Window))
      part.Destroy ();
  }
  Am_Drawonable* drawonable =
      Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
  if (drawonable) {
    drawonable->Set_Data_Store (NULL);
    drawonable->Destroy ();
    window.Set (Am_DRAWONABLE, NULL);
  }
  Am_Value value;
  value=window.Peek(Am_TODO);
  if (value.Valid ()) {
    Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (value);
    win_todo->Remove ();
    delete win_todo;
    window.Set (Am_TODO, NULL);
  }
  if (window.Peek (Am_INTER_LIST).Exists()) {
    Am_Priority_List* inter_list =
        Am_Priority_List::Narrow (window.Get (Am_INTER_LIST));
	window.Set (Am_INTER_LIST, NULL);
    if (inter_list) {
      inter_list->Make_Empty ();
      inter_list->Release ();
    }
  }
}

static void revitalize_subwindow (Am_Object window)
{
  Am_Value value;
  value=window.Peek(Am_TODO);
  if (!value.Valid ())
    return;
  Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (value);
  Am_Drawonable* drawonable =
      Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
  if (drawonable) {
    win_todo->flags |= Am_WINTODO_REPARENT;
    win_todo->Add ();
  }
  else {
    win_todo->flags |= Am_WINTODO_CREATE;
    win_todo->Add ();
  }
  Am_Part_Iterator parts = window;
  Am_Object current;
  for (parts.Start (); !parts.Last (); parts.Next ()) {
    current = parts.Get ();
    if (current.Is_Instance_Of (Am_Window))
      revitalize_subwindow (current);
  }
}

static void kill_subwindow (Am_Object window)
{
  Am_Drawonable* drawonable =
      Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
  Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (window.Get (Am_TODO));
  if (drawonable) {
    win_todo->flags |= Am_WINTODO_DESTROY;
    win_todo->Add ();
  }
  Am_Part_Iterator parts = window;
  Am_Object current;
  for (parts.Start (); !parts.Last (); parts.Next ()) {
    current = parts.Get ();
    if (current.Is_Instance_Of (Am_Window))
      kill_subwindow (current);
  }
}

//this is a demon procedure
static void window_change_owner (Am_Object window, Am_Object /*old_owner*/,
				 Am_Object new_owner)
{
  Am_Drawonable* drawonable =
      Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
  Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (window.Get (Am_TODO));
  
  if (new_owner.Valid ()) {
    if (new_owner.Is_Instance_Of (Am_Screen) ||
    ((new_owner.Is_Instance_Of (Am_Group) ||
          new_owner.Is_Instance_Of (Am_Window)) &&
     !new_owner.Is_Part_Of (window))) {
      if (drawonable) {
        win_todo->flags |= Am_WINTODO_REPARENT;
        win_todo->Add ();
      }
      else {
        win_todo->flags |= Am_WINTODO_CREATE;
        win_todo->Add ();
      }
      Am_Part_Iterator parts = window;
      Am_Object current;
      for (parts.Start (); !parts.Last (); parts.Next ()) {
    current = parts.Get ();
    if (current.Is_Instance_Of (Am_Window))
      revitalize_subwindow (current);
      }
    }
    else
      Am_Error ("Must add a window to either a Screen, Group, "
        "or another Window.\n");
  }
  else {
    if (drawonable) {
      win_todo->flags |= Am_WINTODO_DESTROY;
      win_todo->Add ();
    }
    Am_Part_Iterator parts = window;
    Am_Object current;
    for (parts.Start (); !parts.Last (); parts.Next ()) {
      current = parts.Get ();
      if (current.Is_Instance_Of (Am_Window))
    kill_subwindow (current);
    }
  }
}

//demon procedure
static void graphics_create (Am_Object gr_object)
{
  Am_State_Store* state = new Am_State_Store (gr_object,
      gr_object.Get_Owner (), 
      gr_object.Get (Am_VISIBLE, Am_RETURN_ZERO_ON_ERROR),
      gr_object.Get (Am_LEFT, Am_RETURN_ZERO_ON_ERROR),
      gr_object.Get (Am_TOP, Am_RETURN_ZERO_ON_ERROR),
      gr_object.Get (Am_WIDTH, Am_RETURN_ZERO_ON_ERROR), 
      gr_object.Get (Am_HEIGHT, Am_RETURN_ZERO_ON_ERROR));
  gr_object.Set (Am_PREV_STATE, (Am_Ptr)state, Am_OK_IF_NOT_THERE);
}

//demon procedure
static void graphics_destroy (Am_Object self)
{
  Am_Value value = self.Peek (Am_PREV_STATE, Am_NO_DEPENDENCY);
  self.Set (Am_PREV_STATE, NULL);
  if (!value.Exists())
    return;
  Am_State_Store* state = Am_State_Store::Narrow (value);
  if (state) {
    state->Remove ();
    delete state;
  }
}

//This is a demon procedure
static void graphics_change_owner (Am_Object self, Am_Object, Am_Object)
{
  Am_State_Store* state = Am_State_Store::Narrow (self.Get (Am_PREV_STATE));
  if (state)
    state->Add (true);
}

//demon procedure
static void graphics_repaint (Am_Slot first_invalidated)
{
  Am_Object self = first_invalidated.Get_Owner ();
  Am_Value value;
  value=self.Peek(Am_PREV_STATE);
  if (!value.Exists())
    return;
  Am_State_Store* state = Am_State_Store::Narrow (value);
  if (state)
    state->Add (false);
}

//demon procedure
static void graphics_move (Am_Slot first_invalidated)
{
  Am_Object self = first_invalidated.Get_Owner ();
  Am_Value value;
  value=self.Peek(Am_PREV_STATE);
  if (!value.Exists())
    return;
  Am_State_Store* state = Am_State_Store::Narrow (value);
  if (state)
    state->Add (true);
}

//this goes in a demon procedure
void am_generic_add_part (Am_Object owner, Am_Object old_object,
			  Am_Object new_object)
{
  Am_Value_List components;
  if (old_object.Valid () && old_object.Is_Instance_Of (Am_Graphical_Object)) {
    owner.Make_Unique (Am_GRAPHICAL_PARTS);
    components = owner.Get (Am_GRAPHICAL_PARTS);
    components.Start ();
    if (components.Member (old_object)) {
      components.Delete (false);
      int current_rank = old_object.Get (Am_RANK);
      components.Next ();
      Am_Object current_obj;
      while (!components.Last ()) {
        current_obj = components.Get ();
        current_obj.Set (Am_RANK, current_rank);
        ++current_rank;
        components.Next ();
      }
      Main_Demon_Queue.Enqueue (graphics_move, 0,
			 ((Am_Object_Advanced&)old_object).Get_Slot (Am_LEFT));
      owner.Note_Changed (Am_GRAPHICAL_PARTS);
    }
  }
  if (new_object.Valid () && new_object.Is_Instance_Of (Am_Graphical_Object)) {
    owner.Make_Unique (Am_GRAPHICAL_PARTS);
    components = owner.Get (Am_GRAPHICAL_PARTS);
    bool was_valid = components.Valid ();
    new_object.Set (Am_RANK, (int)components.Length ());
    components.Add (new_object, Am_TAIL, false);
    Main_Demon_Queue.Enqueue (graphics_move, 0,
			 ((Am_Object_Advanced&)new_object).Get_Slot (Am_LEFT));
    if (was_valid)
      owner.Note_Changed (Am_GRAPHICAL_PARTS);
    else
      owner.Set (Am_GRAPHICAL_PARTS, components);
  }
}

// a cheap way to translates coordinates to be inside of me that works
// for any object except scrolling groups
void am_translate_coord_to_me(Am_Object in_obj, Am_Object ref_obj,
			      int &x, int &y)
{
  if (ref_obj == in_obj) {
    return; //x, y are already OK
  }
  else  {
    Am_Object owner;
    owner = in_obj.Get_Owner();
    if (owner.Valid() && owner == ref_obj) { // cheap transformation
      x -= (int)in_obj.Get (Am_LEFT); //simple translate coords to the inside
      y -= (int)in_obj.Get (Am_TOP);  // of object
    }
    else //not the owner, use expensive transformation
      Am_Translate_Coordinates (ref_obj, x, y, in_obj, x, y);
  }
}

Am_Define_Method (Am_Draw_Method, void, generic_mask,
          (Am_Object self, Am_Drawonable* drawonable,
           int x_offset, int y_offset))
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  drawonable->Draw_Rectangle (Am_No_Style, Am_On_Bits, left, top, width,
                  height);
}

// Check whether the point is inside the object.  Ignores
//   covering (i.e., just checks whether point is inside the
//   object even if the object is covered.
// Note: The coordinate system of x and y is defined to be the coordinate
//  system of the ref_obj.  
Am_Define_Method(Am_Point_In_Method, Am_Object, generic_point_in_obj,
         (const Am_Object& in_obj, int x, int y,
          const Am_Object& ref_obj)) {
  if ((bool)in_obj.Get (Am_VISIBLE)) {
    am_translate_coord_to_me(in_obj, ref_obj, x, y);     
    if ((x < 0) || (y < 0)) return Am_No_Object;
    if ((x >= (int)in_obj.Get (Am_WIDTH)) ||
    (y >= (int)in_obj.Get (Am_HEIGHT)))
      return Am_No_Object;
    return in_obj;
  }
  else
    return Am_No_Object;
}

// if a group and not Am_PRETEND_TO_BE_LEAF, returns true, else returns false
bool am_is_group_and_not_pretending(Am_Object in_obj)
{
  if (in_obj.Is_Instance_Of(Am_Group) || in_obj.Is_Instance_Of(Am_Map)) {
    Am_Value v;
    v=in_obj.Peek(Am_PRETEND_TO_BE_LEAF);
    if (!v.Valid())
      return true;
  }
  return false;
}

// for use in objects that don't have parts
Am_Define_Method(Am_Point_In_Or_Self_Method, Am_Object, generic_point_in_part,
         (const Am_Object& in_obj, int x, int y,
          const Am_Object& ref_obj, bool want_self, bool want_groups))
{
  if (want_self && (want_groups || !am_is_group_and_not_pretending(in_obj))) {
    Am_Point_In_Method method;
    method = in_obj.Get(Am_POINT_IN_OBJ_METHOD);
    return method.Call (in_obj, x, y, ref_obj);
  }
  else return Am_No_Object;
}

// for use in objects that don't have leaves: return self if in it
Am_Define_Method(Am_Point_In_Or_Self_Method, Am_Object, generic_point_in_leaf,
         (const Am_Object& in_obj, int x, int y,
          const Am_Object& ref_obj,
          bool /* want_self */, bool want_groups)) {
  if (!want_groups && am_is_group_and_not_pretending(in_obj))
    return Am_No_Object;
  Am_Point_In_Method method;
  method = in_obj.Get(Am_POINT_IN_OBJ_METHOD);
  return method.Call (in_obj, x, y, ref_obj);
}

Am_Define_Method(Am_Point_In_Or_Self_Method, Am_Object, am_group_point_in_part,
         (const Am_Object& in_obj, int x, int y,
          const Am_Object& ref_obj,
          bool want_self, bool want_groups)) {
  am_translate_coord_to_me(in_obj, ref_obj, x, y);
  if (Am_Point_In_Obj (in_obj, x, y, in_obj).Valid()) {
    Am_Value_List comp;
    comp = in_obj.Get (Am_GRAPHICAL_PARTS);
    Am_Object object;
    for (comp.End (); !comp.First (); comp.Prev ()) {
      object = comp.Get ();
      if ((want_groups || !am_is_group_and_not_pretending(object)) &&
      Am_Point_In_Obj (object, x, y, in_obj))
    return object;
    }
    //in in_obj but not in part
    if (want_self && (want_groups || !am_is_group_and_not_pretending(in_obj)))
      return in_obj;
    else return Am_No_Object;
  }
  else return Am_No_Object;
}

Am_Define_Method(Am_Point_In_Or_Self_Method, Am_Object, am_group_point_in_leaf,
         (const Am_Object& in_obj, int x, int y,
          const Am_Object& ref_obj,
          bool want_self, bool want_groups)) {
  am_translate_coord_to_me(in_obj, ref_obj, x, y);   
  if (Am_Point_In_Obj (in_obj, x, y, in_obj)) {
    Am_Value val;
    val=in_obj.Peek(Am_PRETEND_TO_BE_LEAF);
    if (val.Valid()) return in_obj; // true if slot exists and is non-null
    else {
      Am_Value_List comp;
      comp = in_obj.Get (Am_GRAPHICAL_PARTS);
      Am_Object object, ret;
      for (comp.End (); !comp.First (); comp.Prev ()) {
    object = comp.Get ();
    ret = Am_Point_In_Leaf (object, x, y, in_obj, want_self, want_groups);
    if (ret.Valid()) return ret;
      }
      //went through list, not in a part
      if (want_self &&
      (want_groups || !am_is_group_and_not_pretending(in_obj)))
    return in_obj;
      else return Am_No_Object;
    }
  }
  else //  not in me
    return Am_No_Object;
}

// screens don't use the graphical_parts list so just use the part iterator.
// Everything in a screen must be a window, so return NULL unless want_groups
Am_Define_Method(Am_Point_In_Or_Self_Method, Am_Object, screen_point_in_part,
         (const Am_Object& in_obj, int x, int y,
          const Am_Object& ref_obj,
          bool want_self, bool want_groups)) {
  if (!want_groups) return Am_No_Object;
  am_translate_coord_to_me(in_obj, ref_obj, x, y);   
  if (Am_Point_In_Obj (in_obj, x, y, ref_obj)) {
    Am_Part_Iterator parts(in_obj);
    Am_Object object;
    // ** NOTE: order of parts is not necessarily the order the windows are in!
    for (parts.Start(); !parts.Last (); parts.Next ()) {
    object = parts.Get ();
    if (Am_Point_In_Obj (object, x, y, in_obj))
      return object;
      }
    //went through list, not in a part
    if (want_self) return in_obj;
    else return Am_No_Object;
  }
  else // if not in me, return Am_No_Object
    return Am_No_Object;
}

// screens don't use the graphical_parts list so just use the part iterator.
Am_Define_Method(Am_Point_In_Or_Self_Method, Am_Object, screen_point_in_leaf,
         (const Am_Object& in_obj, int x, int y,
          const Am_Object& ref_obj,
          bool want_self, bool want_groups)) {
  am_translate_coord_to_me(in_obj, ref_obj, x, y);   
  if (Am_Point_In_Obj (in_obj, x, y, in_obj)) {
    Am_Part_Iterator parts(in_obj);
    Am_Object object, ret;
    // ** NOTE: order of parts is not necessarily the order the windows are in!
    for (parts.Start(); !parts.Last (); parts.Next ()) {
    object = parts.Get ();
    ret = Am_Point_In_Leaf (object, x, y, in_obj, want_self, want_groups);
    if (ret.Valid()) return ret;
      }
    //went through list, not in a part
    if (want_self && want_groups) //self is a screen, which is "like" a group
      return in_obj;
    else return Am_No_Object;
  }
  else // if not in me, return NULL
    return Am_No_Object;
}

#if defined(_WINDOWS)
#define ASSERT_WINDOW1(W)    if (!(W)) return;
#define ASSERT_WINDOW        ASSERT_WINDOW1(window)
#else
#define ASSERT_WINDOW
#define ASSERT_WINDOW1(W)
#endif

class Am_Standard_Opal_Handlers : public Am_Input_Event_Handlers {
public:
  void Iconify_Notify (Am_Drawonable* draw, bool iconified)
  {
    Am_Wrapper* window_data = (Am_Wrapper*)draw->Get_Data_Store ();
    window_data->Note_Reference ();
    Am_Object window ((Am_Object_Data*)window_data);
    ASSERT_WINDOW
    bool w_iconified = window.Get (Am_ICONIFIED);
    if (w_iconified != iconified)
      window.Set (Am_ICONIFIED, iconified);
  }
  void Frame_Resize_Notify (Am_Drawonable* draw, int left, int top,
                            int right, int bottom)
  {
    Am_Wrapper* window_data = (Am_Wrapper*)draw->Get_Data_Store ();
    window_data->Note_Reference ();
    Am_Object window ((Am_Object_Data*)window_data);
    ASSERT_WINDOW
    int w_left = window.Get (Am_LEFT_BORDER_WIDTH);
    int w_top = window.Get (Am_TOP_BORDER_WIDTH);
    int w_right = window.Get (Am_RIGHT_BORDER_WIDTH);
    int w_bottom = window.Get (Am_BOTTOM_BORDER_WIDTH);
    if (w_left != left)
      window.Set (Am_LEFT_BORDER_WIDTH, left);
    if (w_top != top)
      window.Set (Am_TOP_BORDER_WIDTH, top);
    if (w_right != right)
      window.Set (Am_RIGHT_BORDER_WIDTH, right);
    if (w_bottom != bottom)
      window.Set (Am_BOTTOM_BORDER_WIDTH, bottom);
  }
  void Destroy_Notify (Am_Drawonable* draw)
  {
    Am_Wrapper* window_data = (Am_Wrapper*)draw->Get_Data_Store ();
    window_data->Note_Reference ();
    Am_Object window ((Am_Object_Data*)window_data);
    //call the destroy_method in the window
    if (window.Valid()) {
      Am_Object_Method method;
      method = window.Get(Am_DESTROY_WINDOW_METHOD);
      if (method.Valid())
    method.Call(window);
    }
  }
  void Configure_Notify (Am_Drawonable *draw, int left, int top,
                   int width, int height)
  {
    Am_Wrapper* window_data = (Am_Wrapper*)draw->Get_Data_Store ();
    window_data->Note_Reference ();
    Am_Object window ((Am_Object_Data*)window_data);
    ASSERT_WINDOW
//// DEBUG
//cout << "configure event window=" << window << " left=" << left << " top="
//     << top << " width=" << width << " height=" << height << endl;
    int w_left = window.Get (Am_LEFT);
    int w_top = window.Get (Am_TOP);
    int w_width = window.Get (Am_WIDTH);
    int w_height = window.Get (Am_HEIGHT);
    if (w_left != left)
      window.Set (Am_LEFT, left);
    if (w_top != top)
      window.Set (Am_TOP, top);
    if (w_width != width)
      window.Set (Am_WIDTH, width);
    if (w_height != height)
      window.Set (Am_HEIGHT, height);
  }
  void Exposure_Notify (Am_Drawonable *draw, int left, int top,
                        int width, int height)
  {
    Am_Wrapper* window_data = (Am_Wrapper*)draw->Get_Data_Store ();
    window_data->Note_Reference ();
    Am_Object window ((Am_Object_Data*)window_data);
    ASSERT_WINDOW
//// DEBUG
//cout << "expose window left=" << left <<
//     " top=" << top << " width=" << width << " height=" << height << endl;
    Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (window.Get (Am_TODO));
    win_todo->flags |= Am_WINTODO_EXPOSE;
    win_todo->Merge_Rectangle (left, top, width, height);
    win_todo->Add ();
  }
  void Input_Event_Notify (Am_Drawonable *draw, Am_Input_Event *ev)
    // used for keys, mouse buttons, mouse moved, and enter-leave.
  {
    // get the window from the drawonable
    Am_Wrapper* window_data = (Am_Wrapper*)draw->Get_Data_Store ();
    window_data->Note_Reference ();
    Am_Object event_window ((Am_Object_Data*)window_data);
    ASSERT_WINDOW1(event_window)
    Interactor_Input_Event_Notify (event_window, ev);
  }
} Global_Opal_Handlers;

Am_Input_Event_Handlers *Am_Global_Opal_Handlers = &Global_Opal_Handlers;

// check slots of the window object to set the initial event mask of
// the drawonable.  After creation, the drawonable's event mask will be set
// automatically by interactors and so the slots are only relevant at
// object creation time.  They are needed since the drawonable is
// typically created for a window AFTER interactors are attached to
// the window (on X)
static void set_event_mask(Am_Object window, Am_Drawonable* drawonable) {
  Am_Value value;
  value=window.Peek(Am_INIT_WANT_ENTER_LEAVE);
  if (value.Valid()) drawonable->Set_Enter_Leave(true);
  value=window.Peek(Am_INIT_WANT_MULTI_WINDOW);
  if (value.Valid()) drawonable->Set_Multi_Window(true);
  value=window.Peek(Am_WINDOW_WANT_MOVE_CNT);
  if (value.Valid()) drawonable->Set_Want_Move(true);
}

Am_String get_icon_title(Am_Object &window) {
  Am_Value v = window.Get (Am_ICON_TITLE);
  if (v.Valid()) return v;
  else return window.Get(Am_TITLE);
}

static bool create_drawonable (Am_Object window)
{
  Am_Object owner = window.Get_Owner ();
  if (!owner)
    return false;
  if (!owner.Is_Instance_Of (Am_Screen) &&
      !owner.Is_Instance_Of (Am_Window)) {
    owner = owner.Get (Am_WINDOW);
    if (!owner)
      return false;
  }
  Am_Drawonable* parent = Am_Drawonable::Narrow (owner.Get (Am_DRAWONABLE));
  if (!parent) {
    Am_Window_ToDo* owner_todo = Am_Window_ToDo::Narrow (owner.Get (Am_TODO));
    if (owner_todo->flags) {
      Am_Window_ToDo* my_todo = Am_Window_ToDo::Narrow (window.Get (Am_TODO));
      my_todo->Add ();
    }
    return false;
  }
  int left = window.Get (Am_LEFT);
  int top = window.Get (Am_TOP);
  int width = window.Get (Am_WIDTH);
  int height = window.Get (Am_HEIGHT);
  Am_String title, icon_title;
  title = window.Get (Am_TITLE);
  icon_title = get_icon_title(window);
  bool visible = window.Get (Am_VISIBLE);
  bool iconified = window.Get (Am_ICONIFIED);
  Am_Style back_fill;
  back_fill = window.Get (Am_FILL_STYLE);
  bool save_under = window.Get (Am_SAVE_UNDER);
  int min_width;
  if ((bool)window.Get (Am_USE_MIN_WIDTH))
    min_width = window.Get (Am_MIN_WIDTH);
  else
    min_width = 1;
  int min_height;
  if ((bool)window.Get (Am_USE_MIN_HEIGHT))
    min_height = window.Get (Am_MIN_HEIGHT);
  else
    min_height = 1;
  int max_width;
  if ((bool)window.Get (Am_USE_MAX_WIDTH))
    max_width = window.Get (Am_MAX_WIDTH);
  else
    max_width = 0;
  int max_height;
  if ((bool)window.Get (Am_USE_MAX_HEIGHT))
    max_height = window.Get (Am_MAX_HEIGHT);
  else
    max_height = 0;
  bool query_pos = window.Get (Am_QUERY_POSITION);
  bool query_size = window.Get (Am_QUERY_SIZE);
  bool omit_title_bar = window.Get (Am_OMIT_TITLE_BAR);
  if (owner.Is_Instance_Of (Am_Window))
    omit_title_bar = true;

  Am_Drawonable* drawonable = parent->Create (
      left, top, width, height, title, icon_title, visible, iconified,
      back_fill, save_under, min_width, min_height,
      max_width, max_height, !omit_title_bar, query_pos, query_size, false,
      Am_Global_Opal_Handlers);
  drawonable->Set_Data_Store ((Am_Wrapper*)window);
  Am_Value v = window.Get(Am_CURSOR);
  if (v.Valid()) drawonable->Set_Cursor((Am_Cursor) v);

  window.Set (Am_DRAWONABLE, (Am_Am_Drawonable)drawonable);
  set_event_mask(window, drawonable);
  return true;
}

//demon procedure called when "uncommon" slots are changed
static void window_uncommon_slot (Am_Slot first_invalidated)
{
  Am_Object window = first_invalidated.Get_Owner ();
  Am_Drawonable* drawonable =
          Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
  if (drawonable) {
    Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (window.Get (Am_TODO));
    if (drawonable->Get_Visible () != (bool)window.Get (Am_VISIBLE)) {
      win_todo->flags |= Am_WINTODO_VISIBLE;
      win_todo->Add ();
    }
    if (!(Am_String (window.Get (Am_TITLE)) ==
      drawonable->Get_Title ())) {
      win_todo->flags |= Am_WINTODO_TITLE;
      win_todo->Add ();
    }
    if (!(Am_String (window.Get (Am_ICON_TITLE)) ==
      drawonable->Get_Icon_Title ())) {
      win_todo->flags |= Am_WINTODO_ICON_TITLE;
      win_todo->Add ();
    }
    if (drawonable->Get_Iconify () != (bool)window.Get (Am_ICONIFIED)) {
      win_todo->flags |= Am_WINTODO_ICONIFY;
      win_todo->Add ();
    }
    bool title_p;
    drawonable->Get_Titlebar (title_p);
    if (title_p == (bool)window.Get (Am_OMIT_TITLE_BAR)) {
      Am_Object owner = window.Get_Owner ();
      while (owner) {
        if (owner.Is_Instance_Of (Am_Screen)) {
          win_todo->flags |= Am_WINTODO_TITLE_BAR;
          win_todo->Add ();
          break;
    }
    else if (owner.Is_Instance_Of (Am_Window))
      break;
    owner = owner.Get_Owner ();
      }
    }
    if (drawonable->Get_Background_Color () !=
    Am_Style (window.Get (Am_FILL_STYLE))) {
      win_todo->flags |= Am_WINTODO_FILL_STYLE;
      win_todo->Add ();
    }
    if (drawonable->Get_Cursor() != (Am_Cursor) window.Get(Am_CURSOR)) {
      win_todo->flags |= Am_WINTODO_CURSOR;
      win_todo->Add ();
    }
    int width, height;
    drawonable->Get_Min_Size (width, height);
    if ((bool)window.Get (Am_USE_MIN_WIDTH)) {
      if (width != (int)window.Get (Am_MIN_WIDTH)) {
        win_todo->flags |= Am_WINTODO_MIN_SIZE;
        win_todo->Add ();
      }
    }
    else if (width != 1) {
      win_todo->flags |= Am_WINTODO_MIN_SIZE;
      win_todo->Add ();
    }
    if ((bool)window.Get (Am_USE_MIN_HEIGHT)) {
      if (height != (int)window.Get (Am_MIN_HEIGHT)) {
        win_todo->flags |= Am_WINTODO_MIN_SIZE;
        win_todo->Add ();
      }
    }
    else if (height != 1) {
      win_todo->flags |= Am_WINTODO_MIN_SIZE;
      win_todo->Add ();
    }
    drawonable->Get_Max_Size (width, height);
    if ((bool)window.Get (Am_USE_MAX_WIDTH)) {
      if (width != (int)window.Get (Am_MAX_WIDTH)) {
        win_todo->flags |= Am_WINTODO_MAX_SIZE;
        win_todo->Add ();
      }
    }
    else if (width != 0) {
      win_todo->flags |= Am_WINTODO_MAX_SIZE;
      win_todo->Add ();
    }
    if ((bool)window.Get (Am_USE_MAX_HEIGHT)) {
      if (height != (int)window.Get (Am_MAX_HEIGHT)) {
        win_todo->flags |= Am_WINTODO_MAX_SIZE;
        win_todo->Add ();
      }
    }
    else if (height != 0) {
      win_todo->flags |= Am_WINTODO_MAX_SIZE;
      win_todo->Add ();
    }
//// NDY: These need to be defined in gem.
// Am_CLIP_CHILDREN  Am_WINTODO_CLIP
// Am_SAVE_UNDER     Am_WINTODO_SAVE_UNDER
  }
}

//demon procedure called when "common" slots are changed
static void window_common_slot (Am_Slot first_invalidated)
{
  Am_Object window = first_invalidated.Get_Owner ();
  Am_Drawonable* drawonable =
          Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
  if (drawonable) {
    Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (window.Get (Am_TODO));
    int w_left = window.Get (Am_LEFT, Am_RETURN_ZERO_ON_ERROR);
    int w_top = window.Get (Am_TOP, Am_RETURN_ZERO_ON_ERROR);
    int w_width = window.Get (Am_WIDTH, Am_RETURN_ZERO_ON_ERROR);
    int w_height = window.Get (Am_HEIGHT, Am_RETURN_ZERO_ON_ERROR);
    int d_left, d_top, d_width, d_height;
    drawonable->Get_Position (d_left, d_top);
    drawonable->Get_Size (d_width, d_height);
    if ((w_left != d_left) || (w_top != d_top)) {
      win_todo->flags |= Am_WINTODO_POSITION;
      win_todo->Add ();
    }
    if ((w_width != d_width) || (w_height != d_height)) {
      win_todo->flags |= Am_WINTODO_SIZE;
      win_todo->Add ();
    }
//// NDY: May want to make other slots be common
  }
}

static void window_expose (Am_Object window)
{
  Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (window.Get (Am_TODO));
  if (win_todo->width && win_todo->height) {
    Am_Drawonable* drawonable =
      Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
    if (drawonable) {
      int double_buffer = window.Get(Am_DOUBLE_BUFFER);
      if (double_buffer == 1) {
	Am_Object screen;

	for (screen = window.Get_Owner();
	     (!(screen.Is_Instance_Of(Am_Screen)) && screen.Valid());
	     screen = screen.Get_Owner()) ;
	if (!screen.Valid())
	  Am_Error("** updating a window not added to a screen?\n");
	Am_Drawonable* o_drawonable =
	  Am_Drawonable::Narrow (screen.Get (Am_OFFSCREEN_DRAWONABLE));
	Am_Style fill;
	fill = window.Get(Am_FILL_STYLE);
	if (!o_drawonable) {
	  Am_Drawonable* d =
	    Am_Drawonable::Narrow (screen.Get(Am_DRAWONABLE));
	  o_drawonable = d->Create_Offscreen (win_todo->width,
					     win_todo->height, fill);
        // o_drawonable = d->Create (0, 0, win_todo->width,
	//				     win_todo->height);
	  screen.Set(Am_OFFSCREEN_DRAWONABLE, (Am_Ptr)o_drawonable);
	}
	int w, h, w2, h2;
	o_drawonable->Get_Size(w,h);
	drawonable->Get_Size(w2,h2);
	//if (w < win_todo->width || h < win_todo->height) {
	if (w < w2 || h < h2) {
// prevent thrashing between a (10x100) and (100x10) drawonable
          w = w > w2 ? w : w2;
          h = h > h2 ? h : h2;
//        w = w > win_todo->width ? w : win_todo->width;
//        h = h > win_todo->height ? h : win_todo->height;
          o_drawonable->Set_Size (w, h);
        }
        //o_drawonable->Flush_Output ();

        o_drawonable->Set_Background_Color (fill);
        //o_drawonable->Flush_Output ();
        o_drawonable->Set_Clip (win_todo->left, win_todo->top,
                                win_todo->width, win_todo->height);
        o_drawonable->Clear_Area (win_todo->left, win_todo->top,
                                  win_todo->width, win_todo->height);
        Am_Value_List components;
        components = window.Get (Am_GRAPHICAL_PARTS);
        Am_Object item;
        //o_drawonable->Flush_Output ();

        for (components.Start (); !components.Last (); components.Next ()) {
          item = components.Get ();
          Am_State_Store* state =
            Am_State_Store::Narrow (item.Get (Am_PREV_STATE));
          if (state->Visible (o_drawonable, 0,0))
            Am_Draw (item, o_drawonable, 0,0);
        }
        // objects are drawn.  now blit.
        drawonable->Set_Clip (win_todo->left, win_todo->top,
                              win_todo->width, win_todo->height);
        drawonable->Bitblt (win_todo->left, win_todo->top, win_todo->width,
                            win_todo->height, o_drawonable, win_todo->left, win_todo->top);
      } // if (double_buffer)
      else {
	if (double_buffer == Am_WIN_DOUBLE_BUFFER_EXTERNAL) {
	  drawonable =
	    Am_Drawonable::Narrow (window.Get (Am_OFFSCREEN_DRAWONABLE));
	}
	  
//// DEBUG
//cout << "draw window " << window << " <l"
//     << win_todo->left << " t" << win_todo->top << " w" << win_todo->width
//     << " h" << win_todo->height << ">" << endl << flush;
        drawonable->Set_Clip (win_todo->left, win_todo->top,
                              win_todo->width, win_todo->height);
//// DEBUG
//cout << "window clip <l" << win_todo->left << " t" << win_todo->top
//     << " w" << win_todo->width << " h" << win_todo->height << ">" << endl;
	if (double_buffer != Am_WIN_DOUBLE_BUFFER_EXTERNAL) {
	  drawonable->Clear_Area (win_todo->left, win_todo->top,
				  win_todo->width, win_todo->height);
	}
	Am_Value_List components;
	components = window.Get (Am_GRAPHICAL_PARTS);
	Am_Object item;
	for (components.Start (); !components.Last (); components.Next ()) {
	  item = components.Get ();
	  Am_State_Store* state =
	    Am_State_Store::Narrow (item.Get (Am_PREV_STATE));
	  if (state->Visible (drawonable, 0, 0))
	    Am_Draw (item, drawonable, 0, 0);
	}
      } // else (double_buffer)
      drawonable->Flush_Output ();
    }
    win_todo->width = 0; win_todo->height = 0;
  }
}

static bool destroy_drawonable (Am_Object window)
{
  Am_Drawonable* drawonable =
      Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
  if (drawonable) {
    Am_Part_Iterator parts = window;
    Am_Object current;
    Am_Drawonable* sub_drawonable;
    Am_Window_ToDo* my_todo;
    for (parts.Start (); !parts.Last (); parts.Next ()) {
      current = parts.Get ();
      if (current.Is_Instance_Of (Am_Window)) {
        sub_drawonable =
            Am_Drawonable::Narrow (current.Get (Am_DRAWONABLE));
        if (sub_drawonable) {
          my_todo = Am_Window_ToDo::Narrow (window.Get (Am_TODO));
          my_todo->Add ();
      return false;
        }
      }
    }
    drawonable->Set_Data_Store (NULL);
    drawonable->Destroy ();
    window.Set (Am_DRAWONABLE, NULL);
  }
  return true;
}

Am_Define_Method(Am_Draw_Method, void, window_draw,
         (Am_Object window, Am_Drawonable* /*drawonable */,
               int /* x_offset */, int /* y_offset */ ))
{
  Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (window.Get (Am_TODO));
  unsigned long flags = win_todo->flags;

//// NDY: Need to consider Am_WINTODO_REPARENT (currently impossible)
  if (flags & Am_WINTODO_CREATE) {
    if (!create_drawonable (window))
      return;
  }
  else if (flags & Am_WINTODO_DESTROY) {
    if (destroy_drawonable (window))
      win_todo->flags = 0;
    return;
  }
  else if (flags & Am_WINTODO_REPARENT)
    Am_Error ("I didn't think reparenting could happen\n");
  else {
    Am_Drawonable* drawonable =
        Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
    bool flush = false;
    if (!drawonable)
      return;
    if (flags & Am_WINTODO_POSITION) {
      int left = window.Get (Am_LEFT);
      int top = window.Get (Am_TOP);
      drawonable->Set_Position (left, top);
//// DEBUG
//cout << "Window " << window << " moved to ("
//     << left << ", " << top << ")" << endl;
      flush = true;
    }
    if (flags & Am_WINTODO_SIZE) {
      int width = window.Get (Am_WIDTH);
      int height = window.Get (Am_HEIGHT);
      drawonable->Set_Size (width, height);
      flush = true;
    }
    if (flags & Am_WINTODO_VISIBLE) {
      drawonable->Set_Visible (window.Get (Am_VISIBLE));
      flush = true;
    }
    if (flags & Am_WINTODO_TITLE) {
      drawonable->Set_Title (Am_String (window.Get (Am_TITLE)));
      flush = true;
    }
    if (flags & Am_WINTODO_ICON_TITLE) {
      Am_String title = get_icon_title(window);
      drawonable->Set_Icon_Title (title);
      flush = true;
    }
    if (flags & Am_WINTODO_TITLE_BAR) {
      // Subwindows are not allowed to have titlebars!
      bool omit_titlebar = window.Get (Am_OMIT_TITLE_BAR);
      Am_Object owner = window.Get_Owner ();
      if (owner.Is_Instance_Of (Am_Screen)) {
        if (omit_titlebar)
          drawonable->Set_Titlebar (false);
        else
          drawonable->Set_Titlebar (true);
        flush = true;
      }
    }
    if (flags & Am_WINTODO_ICONIFY) {
      drawonable->Set_Iconify (window.Get (Am_ICONIFIED));
      flush = true;
    }
    if (flags & Am_WINTODO_FILL_STYLE) {
      drawonable->Set_Background_Color
      (Am_Style (window.Get (Am_FILL_STYLE)));
      flush = true;
    }
    if (flags & Am_WINTODO_CURSOR) {
      drawonable->Set_Cursor((Am_Cursor) window.Get(Am_CURSOR));
      flush = true;
    }
    if (flags & Am_WINTODO_MIN_SIZE) {
      int width, height;
      if ((bool)window.Get (Am_USE_MIN_WIDTH))
    width = window.Get (Am_MIN_WIDTH);
      else
    width = 1;
      if ((bool)window.Get (Am_USE_MIN_HEIGHT))
    height = window.Get (Am_MIN_HEIGHT);
      else
    height = 1;
      drawonable->Set_Min_Size (width, height);
    }
    if (flags & Am_WINTODO_MAX_SIZE) {
      int width, height;
      if ((bool)window.Get (Am_USE_MAX_WIDTH))
    width = window.Get (Am_MAX_WIDTH);
      else
    width = 0;
      if ((bool)window.Get (Am_USE_MAX_HEIGHT))
    height = window.Get (Am_MAX_HEIGHT);
      else
    height = 0;
      drawonable->Set_Max_Size (width, height);
    }
//// NDY: implement other TODO features.  Need to be defined in gem.
//Am_WINTODO_SAVE_UNDER  Am_SAVE_UNDER
//Am_WINTODO_CLIP        Am_CLIP_CHILDREN
    if (flush)
      drawonable->Flush_Output ();
  }
  if (flags & Am_WINTODO_EXPOSE) {
    if ((bool)window.Get (Am_VISIBLE))
      window_expose (window);
  }
  win_todo->flags = 0;
}

Am_Define_Method(Am_Invalid_Method, void, window_invalid,
         (Am_Object window, Am_Object /*which_part*/,
          int left, int top, int width, int height))
{
  if ((bool)window.Get (Am_VISIBLE, Am_NO_DEPENDENCY) && (Am_Ptr)window.Get (Am_DRAWONABLE, Am_NO_DEPENDENCY)) {
//// DEBUG
//cout << "invalidate window, " << *window << ", l" << left << " t"
//     << top << " w" << width << " h" << height << endl;
    if (left < 0) {
      width += left;
      left = 0;
    }
    if (top < 0) {
      height += top;
      top = 0;
    }
    if ((width > 0) && (height > 0)) {
      Am_Window_ToDo* win_todo =
      Am_Window_ToDo::Narrow (window.Get (Am_TODO, Am_NO_DEPENDENCY));
      win_todo->flags |= Am_WINTODO_EXPOSE;
      win_todo->Merge_Rectangle (left, top, width, height);
      win_todo->Add ();
    }
  }
}

void Am_Update (Am_Object window)
{
  Am_Value value;
  value=window.Peek(Am_TODO);
  if (value.Valid ()) {
    Am_Window_ToDo* win_todo = Am_Window_ToDo::Narrow (value);
    if (win_todo->flags) {
      win_todo->Remove ();
      Am_Draw_Method draw_method;
      draw_method = window.Get (Am_DRAW_METHOD);
      //ignores drawonable and offset parameters
      draw_method.Call (window, NULL, 0, 0);
    }
  }
}

//if debugging, then checks to see whether trying to redraw a window
//after it was already started to draw, but didn't finish, which means
//that it crashed last time it was trying to be drawn, in which case
//it doesn't try to redraw it again, but only one time.
void Am_Update_All ()
{
  if (!Main_Demon_Queue.Empty ())
    Main_Demon_Queue.Invoke ();

  Main_Demon_Queue.Prevent_Invoke ();
  Am_State_Store::Invoke ();
  Am_Window_ToDo* current;
  Am_Draw_Method draw_method;
#ifdef DEBUG
  int was_inprogress;
#endif
  Am_Object win;
  while (Window_ToDo_Head) {
    current = Window_ToDo_Head;
    Window_ToDo_Head = Window_ToDo_Head->next;
    if (!Window_ToDo_Head)
      Window_ToDo_Tail = NULL;
    current->next = NULL;
    current->prev = NULL;
    win = current->window;
    if (win.Valid()) {
#ifdef DEBUG
      was_inprogress = win.Get(Am_OBJECT_IN_PROGRESS, Am_RETURN_ZERO_ON_ERROR);
      if (was_inprogress & 1) {
	cerr << "** Draw on window " << win
	     << " but crashed last time, so skipping it.\n" << flush;
	continue; //skip this object
      }
      else win.Set(Am_OBJECT_IN_PROGRESS, 1, Am_OK_IF_NOT_THERE);
#endif
      draw_method = win.Get (Am_DRAW_METHOD);
      draw_method.Call (win, NULL, 0, 0);
#ifdef DEBUG
      win.Set(Am_OBJECT_IN_PROGRESS, 0);
#endif
    }
  }
  Main_Demon_Queue.Release_Invoke ();
}

void Am_Wait_For_Event ()
{
  Am_Drawonable::Wait_For_Event ();
}

//
// Timing stuff
// 

Am_Timer_State Am_Global_Timer_State = Am_TIMERS_RUNNING;

// am_clock(): returns current time-of-day unless all timers are suspended, 
// in which case it returns the moment of suspension
static inline Am_Time am_clock ()
{
  static Am_Time clock;

  if (Am_Global_Timer_State != Am_TIMERS_SUSPENDED)
    clock = Am_Time::Now();

  return clock;
}

// This stores the information required to take care of the animation 
// interactors' timing events.  gdefs.h defines Am_Time.  

class Am_Interactor_Time_Event {
public:
  Am_Interactor_Time_Event (Am_Time new_delta, Am_Object obj, 
			    Am_Slot_Key key, bool once)
  {
    delta = new_delta;
    object = obj;
    method_slot = key;
    only_once = once;
    start_time = am_clock();
    next_timeout = start_time + delta;
  }
  Am_Object object;         // object to call timeout method on
  Am_Slot_Key method_slot;  // slot key containing timeout method
  bool only_once;           // if true, only time out once
  Am_Time delta;            // time between events
  Am_Time next_timeout;     // next time it should time out
  Am_Time start_time;    // time when this event was added to timing event list.
};

static Am_Value_List am_timing_events;

static void am_add_timing_event(Am_Interactor_Time_Event* new_event)
{
  Am_Interactor_Time_Event *e;
  Am_Time new_time = new_event->next_timeout;
  for (am_timing_events.Start(); !am_timing_events.Last(); 
       am_timing_events.Next()) {
    e = (Am_Interactor_Time_Event*)(Am_Ptr)(am_timing_events.Get());
    if (e->next_timeout > new_time) {
      am_timing_events.Insert((Am_Ptr)new_event, Am_BEFORE);
      return;
    }
  } // end for: if we get here, add to end of list.
  am_timing_events.Add((Am_Ptr)new_event, Am_TAIL);
}

void Am_Register_Timer(Am_Time wait_time, Am_Object obj,
               Am_Slot_Key method_slot, bool once)
{
  Am_Interactor_Time_Event* new_event = 
    new Am_Interactor_Time_Event(wait_time, obj, method_slot, once);
  am_add_timing_event(new_event);
}

// deletes the first timer event which matches obj and slot.
void Am_Stop_Timer(Am_Object obj, Am_Slot_Key slot)
{
  Am_Interactor_Time_Event *e;
  for (am_timing_events.Start(); !am_timing_events.Last(); 
       am_timing_events.Next()) {
    e = (Am_Interactor_Time_Event*)(Am_Ptr)am_timing_events.Get();
    if (e->object == obj && e->method_slot == slot) {
      am_timing_events.Delete();
      delete e;
      return;
    }
  }
  //Am_ERROR("Am_Stop_Timer on " << obj << " slot " << slot
  //     << " but hasn't been registered")
}

void Am_Reset_Timer_Start(Am_Object obj, Am_Slot_Key slot)
{
  Am_Interactor_Time_Event *e;
  for (am_timing_events.Start(); !am_timing_events.Last(); 
       am_timing_events.Next()) {
    e = (Am_Interactor_Time_Event*)(Am_Ptr)am_timing_events.Get();
    if (e->object == obj && e->method_slot == slot) {
      e->start_time = am_clock();
      return;
    }
  }
  Am_ERROR("Am_Reset_Timer_Start on " << obj << " slot " << slot
       << " but hasn't been registered")
}

// am_Handle_Timing_Events calls the methods for any timing events that have
// expired, and returns the deadline of the next event in the
// queue.  If a long period of time has passed since the last time we 
// handled timing events, the same event might have timed out many times in
// the mean time.  We make sure to call the method once for every time it
// timed out, to make sure the method is always called the correct number 
// of times in a given time period.
//
// Returns the deadline of the next timing event.
// If no timing events are active, returns time Zero.
//
static Am_Time am_Handle_Timing_Events()
{
  bool ticked = false;
  Am_Time return_time; // next deadline
  if (Am_Global_Timer_State != Am_TIMERS_SUSPENDED
      && !am_timing_events.Empty()) {
    Am_Timer_Method method;
    Am_Time next_timeout_time;
    Am_Time now = am_clock();
    Am_Interactor_Time_Event *next_event;
    Am_Time elapsed_time;

    Am_Object method_object;
    Am_Slot_Key method_slot;
    Am_Time inter_start_time;
    while (true) { // stop when the next event is not past
      am_timing_events.Start();
      if (am_timing_events.Empty()) {
	//deleted the last event
	return return_time; // will be uninitialized
      }
      next_event = (Am_Interactor_Time_Event*)(Am_Ptr)am_timing_events.Get();
      if (next_event->next_timeout < now ) {
	// Deal with moving the timing event in its queue _before_ we 
	// call the method.  The method might call Am_Stop_Timer, 
	// removing this event from the queue, which deletes it.
	next_event->next_timeout += next_event->delta;
	am_timing_events.Delete();
	method_object = next_event->object;
	method_slot = next_event->method_slot;
	inter_start_time = next_event->start_time;
	if (next_event->only_once)
	  delete next_event;
	else 
	  am_add_timing_event(next_event);
	if (method_object.Valid()) {
	  //  Call the method for this object
	  method = method_object.Get(method_slot);
	  if (method.Valid()) {
	    elapsed_time = now - inter_start_time;
	    method.Call(method_object, elapsed_time);
	  }
	}
	ticked = true;
      }
      else break;
    }

    if (ticked && Am_Global_Timer_State == Am_TIMERS_SINGLE_STEP)
      Am_Set_Timer_State (Am_TIMERS_SUSPENDED);
	
    return_time = next_event->next_timeout;
  }
  return return_time;
}

#if 0
// no longer necessary, now that Process_Event() takes a deadline rather
// than a timeout duration
static void am_Adjust_Timeout (Am_Time& timeout)
{
  if (!timeout.Zero()) {
    //time may have passed since the previous call to am_clock()
    timeout = timeout - am_clock();
    // Hack:
    // if we were _almost_ going to time out at the next_timeout.Is_Past()
    // test above, but failed, your timeout could be negative here.
    // Make it 1ms instead: 0ms will cause Unix gem to treat it as a 
    // single screen, no timeout case, which is also bad.
    // Bug: there's no difference between invalid time and 0 time
    // Another solution would be to simply not crash on select errors 
    // in Unix.  Ideally we want to have the Right Answer, but we can't
    // do all of this work atomically so we're doomed to occasional failure.
    if (timeout < Am_Time()) {
      timeout = Am_Time(1);
      cerr << "am_Adjust_Timeout: negative timeout.\n";
    }
  }
}
#endif

void Am_Set_Timer_State (Am_Timer_State new_state)
{
  Am_Time before = am_clock ();
  Am_Global_Timer_State = new_state;
  Am_Time after = am_clock();

  if (before != after) {
    // a discontinuity in the clock (caused by suspending and then resuming);
    // translate every timestamp on the queue to the new timeline
    Am_Time diff = after - before;
    
    Am_Interactor_Time_Event *next_event;
    Am_Value_List events = am_timing_events;
    for (events.Start(); !events.Last(); events.Next()) {
      next_event = (Am_Interactor_Time_Event*)(Am_Ptr)events.Get();
      next_event->next_timeout = next_event->next_timeout + diff;
      next_event->start_time = next_event->start_time + diff;
    }
  }
}

bool Am_Do_Events (bool wait)
{
  am_Handle_Timing_Events();
  Am_Update_All ();
  if (wait) {
    Am_Time deadline = Am_Time::Now() + 100UL; // 100 milliseconds from now
    Am_Drawonable::Process_Event (deadline);
  }
  else
    Am_Drawonable::Process_Immediate_Event ();
  Am_Update_All ();
  return Am_Main_Loop_Go;
}

void Am_Main_Event_Loop ()
{
  //// TODO: make exit when no windows are owned. (visible?)
  Am_Time deadline;
  while (Am_Main_Loop_Go) {
    deadline = am_Handle_Timing_Events();
    Am_Update_All (); 
    Am_Drawonable::Process_Event (deadline);
  }
}

void Am_Exit_Main_Event_Loop ()
{
  Am_Main_Loop_Go = false;
}

// Converts a point in one object's coordinate system, to that of another
// object.  If the objects are not comparable (like being on different screens
// or not being on a screen at all) then the function will return false.
// Otherwise, it will return true and dest_x and dest_y will contain the
// converted coordinates.  Note that the coordinates are for the
// INSIDE of dest_obj.  This means that if "obj" was at src_x, src_y
// in src_obj and you remove it from src_obj and add it to dest_obj at
// dest_x, dest_y then it will be at the same physical screen position.
bool Am_Translate_Coordinates (const Am_Object& src_obj, int src_x, int src_y,
                               const Am_Object& dest_obj, int& dest_x,
			       int& dest_y)
{
  Am_Translate_Coordinates_Method translate_method;
  bool using_constraint = true;
  int src_height = 0;
  int off_x, off_y;
  Am_Object owner = src_obj;
  while (owner) {
    ++src_height;
    owner = owner.Get_Owner (Am_NO_DEPENDENCY);
  }
  int dest_height = 0;
  owner = dest_obj;
  while (owner.Valid ()) {
    ++dest_height;
    owner = owner.Get_Owner (Am_NO_DEPENDENCY);
  }
  Am_Object src_part;
  Am_Object curr_src = src_obj;
  while (src_height > dest_height) {
    if (curr_src.Is_Instance_Of (Am_Graphical_Object) ||
        curr_src.Is_Instance_Of (Am_Window)) {
      translate_method = curr_src.Get(Am_TRANSLATE_COORDINATES_METHOD);
      translate_method.Call (curr_src, src_part, src_x, src_y,
                 src_x, src_y);
    }
    else
      return false;
    src_part = curr_src;
    if (using_constraint)
      curr_src = curr_src.Get_Owner ();
    else
      curr_src = curr_src.Get_Owner ();
    --src_height;
  }
  Am_Object dest_part;
  Am_Object curr_dest = dest_obj;
  while (dest_height > src_height) {
    if (curr_dest.Is_Instance_Of (Am_Graphical_Object) ||
        curr_dest.Is_Instance_Of (Am_Window)) {
      translate_method = curr_dest.Get(Am_TRANSLATE_COORDINATES_METHOD);
      translate_method.Call (curr_dest, dest_part, 0, 0, off_x, off_y);
      src_x -= off_x;
      src_y -= off_y;
    }
    else
      return false;
    dest_part = curr_dest;
    if (using_constraint)
      curr_dest = curr_dest.Get_Owner ();
    else
      curr_dest = curr_dest.Get_Owner ();
    --dest_height;
  }
  while (curr_src.Valid () && (curr_src != curr_dest)) {
    if (curr_src.Is_Instance_Of (Am_Graphical_Object) ||
        curr_src.Is_Instance_Of (Am_Window)) {
      translate_method = curr_src.Get(Am_TRANSLATE_COORDINATES_METHOD);
      translate_method.Call (curr_src, src_part, src_x, src_y,
                 src_x, src_y);
    }
    else
      break;
    if (curr_dest.Is_Instance_Of (Am_Graphical_Object) ||
        curr_dest.Is_Instance_Of (Am_Window)) {
      translate_method = curr_dest.Get(Am_TRANSLATE_COORDINATES_METHOD);
      translate_method.Call (curr_dest, dest_part, 0, 0, off_x, off_y);
      src_x -= off_x;
      src_y -= off_y;
    }
    else
      break;
    src_part = curr_src;
    dest_part = curr_dest;
    if (using_constraint) {
      curr_src = curr_src.Get_Owner();
      curr_dest = curr_dest.Get_Owner();
    }
    else {
      curr_src = curr_src.Get_Owner();
      curr_dest = curr_dest.Get_Owner();
    }
  }
  if (curr_src.Valid () && curr_src == curr_dest) {
    dest_x = src_x;
    dest_y = src_y;
    return true;
  }
  else {
    if (using_constraint) {
      while (curr_src.Valid ())
        curr_src = curr_src.Get_Owner ();
      while (curr_dest.Valid ())
        curr_dest = curr_dest.Get_Owner ();
    }
    return false;
  }
}

// Am_Translate_Coordinates_Method:
//  - Stored in slot Am_TRANSLATE_COORDINATES_METHOD
//  - Given a point in the coordinate system of obj, converts it to
//    be in the coordinate system of its owner
Am_Define_Method(Am_Translate_Coordinates_Method, void,
         generic_translate_coordinates,
         (const Am_Object& obj, const Am_Object& /*for_part*/,
          int in_x, int in_y, int& out_x, int& out_y)) {
  out_x = (int)obj.Get (Am_LEFT) + in_x;
  out_y = (int)obj.Get (Am_TOP) + in_y;
}

// windows need a special method because of the border of the window
Am_Define_Method(Am_Translate_Coordinates_Method, void,
              window_translate_coordinates,
              (const Am_Object& obj, const Am_Object& /*for_part*/,
               int in_x, int in_y, int& out_x, int& out_y))
{
  Am_Object owner = obj.Get_Owner (Am_NO_DEPENDENCY);
  if (owner.Is_Instance_Of (Am_Window)) { // sub window
    out_x = in_x + (int) obj.Get(Am_LEFT);
    out_y = in_y + (int) obj.Get(Am_TOP);
  } else {
    // compute out_x/y from slots, not from drawonable
    out_x = in_x + (int) obj.Get (Am_LEFT) + (int) obj.Get(Am_LEFT_BORDER_WIDTH);
    out_y = in_y + (int) obj.Get (Am_TOP) + (int) obj.Get(Am_TOP_BORDER_WIDTH);

    //get this always in case this is called from a formula so the
    //translate-coords will be called when the border changes which
    //might happen due to a frame_resize message from gem.
    obj.Get (Am_OMIT_TITLE_BAR);

    /* eab 6/16/97 removed reliance on drawonable
    //// HACK: This code forces the drawonable to be in the most up to date
    //// state.  It will only work for top level windows.  (And may have
    //// problems with top-level windows, too.)  The best implementation for
    //// this is to be able to calculate the position without needing a
    //// drawonable at all.
    Am_Drawonable* drawonable =
      Am_Drawonable::Narrow (obj.Get (Am_DRAWONABLE, Am_NO_DEPENDENCY));
    if (drawonable) {
      int left_border, top_border, right_border, bottom_border;
      int outer_left, outer_top;
      drawonable->Inquire_Window_Borders(left_border, top_border,
                                         right_border, bottom_border,
                                         outer_left, outer_top);
      out_x = in_x + left_border + outer_left;
      out_y = in_y + top_border + outer_top;
    } else {
      obj.Get (Am_DRAWONABLE); // Set a dependency in case drawonable appears later
      out_x = 0;
      out_y = 0;
    }
    */
  }
}

const char* Am_Check_Int_Type (const Am_Value& value)
{
  if (value.type == Am_INT || !value.Safe())
    return NULL;
  else {
    cerr << "** Amulet error, Expected int type in Object but got " << value
	 << endl << flush;
    return "int type check error";
  }
}

static bool screen_is_color (Am_Object screen)
{
  Am_Drawonable *d = Am_Drawonable::Narrow(screen.Get (Am_DRAWONABLE));
  return (d->Is_Color());
}

Am_Object Am_Create_Screen (const char* display_name)
{
  Am_Drawonable* new_root = Am_Drawonable::Get_Root_Drawonable (display_name);
  if (!new_root)
    return NULL;
  int width, height;
  new_root->Get_Size (width, height);
  Am_Object new_screen = Am_Screen.Create (display_name)
    .Set (Am_SCREEN, Am_Screen, Am_OK_IF_NOT_THERE)
    .Set (Am_DRAWONABLE, (Am_Am_Drawonable)new_root, Am_OK_IF_NOT_THERE)
    .Set (Am_WIDTH, width)
    .Set (Am_HEIGHT, height);
  Am_Object_Advanced temp = (Am_Object_Advanced&)new_screen;
//// NDY Create other slots that Am_Screen ought to have
  new_screen.Set(Am_IS_COLOR, screen_is_color(new_screen));
  return new_screen;
}

void Am_Move_Object (Am_Object object, Am_Object ref_object,
                     bool above)
{
  if (!object.Valid ()  || !ref_object.Valid ())
    Am_Error ("** Am_Move_Object called with NULL object or ref_object\n");
  if (object == ref_object) {
    cerr << "** Am_Move_Object called same object, " << object << ", as both "
      "moved object and reference object." << endl;
    Am_Error ();
  }
  Am_Object owner = object.Get_Owner ();
  if (!owner.Valid ())
    return;
  if (ref_object.Get_Owner () != owner) {
      cerr << "** Am_Move_Object:  the object " << object << " has owner " <<
	owner << "and the ref_object " << ref_object << " has owner " <<
        ref_object.Get_Owner () << " they must be the same." << endl;
    Am_Error ();
  }
  if (object.Is_Instance_Of (Am_Graphical_Object)) {
    if (!ref_object.Is_Instance_Of (Am_Graphical_Object)) {
      cerr << "** Am_Move_Object called with an object " << object <<
	" that is a graphical object and a ref_object " << ref_object <<
	" that is not." << endl;
      Am_Error ();
    }
    owner.Make_Unique (Am_GRAPHICAL_PARTS);
    Am_Value_List components;
    components = owner.Get (Am_GRAPHICAL_PARTS);
    components.Start ();
    components.Member (object);
    components.Delete (false);
    components.Start ();
    components.Member (ref_object);
    components.Insert (object, above ? Am_AFTER : Am_BEFORE, false);
    int rank = 0;
    Am_Object current;
    for (components.Start (); !components.Last (); components.Next ()) {
      current = components.Get ();
      current.Set (Am_RANK, rank);
      ++rank;
    }
    owner.Note_Changed (Am_GRAPHICAL_PARTS);
    Main_Demon_Queue.Enqueue (graphics_repaint, 0,
			  ((Am_Object_Advanced&)object).Get_Slot (Am_VISIBLE));
  }
  else if (object.Is_Instance_Of (Am_Window)) {
    if (!ref_object.Is_Instance_Of (Am_Window)) {
      cerr << "** Am_Move_Object called with an object " << object <<
	" that is a window and a ref_object " << ref_object <<
	" that is not." << endl;
      Am_Error ();
    };
    // objects must have same parent
    if (object.Get_Owner() != ref_object.Get_Owner())
      {
	cerr << "** Am_Move_Object called with two windows"
	     << " with different parents: " << object << ", " << ref_object;
      }
    Am_Drawonable* draw1 =
      Am_Drawonable::Narrow (object.Get(Am_DRAWONABLE));
    Am_Drawonable* draw2 =
      Am_Drawonable::Narrow (ref_object.Get(Am_DRAWONABLE));
    if (draw1 && draw2)
      if (above)
	draw1->Raise_Window(draw2);
      else
	draw1->Lower_Window(draw2);
  }
  else {
    cerr << "** Am_Move_Object: Attempt to move " << object <<
      " which is not a graphical object or window" << endl;
    Am_Error ();
  }
}

void Am_To_Top (Am_Object object)
{
  if (!object.Valid ())
    Am_Error ("** Am_To_Top called with NULL object\n");
  Am_Object owner = object.Get_Owner ();
  if (!owner.Valid ())
    return;
  if (object.Is_Instance_Of (Am_Graphical_Object)) {
    owner.Make_Unique (Am_GRAPHICAL_PARTS);
    Am_Value_List components;
    components = owner.Get (Am_GRAPHICAL_PARTS);
    int rank = object.Get (Am_RANK);
    if (rank+1 != components.Length ()) {
      components.Start ();
      components.Member (object);
      components.Delete (false);
      components.Next ();
      Am_Object current;
      while (!components.Last ()) {
	current = components.Get ();
        current.Set (Am_RANK, rank);
        ++rank;
        components.Next ();
      }
      object.Set (Am_RANK, (int)components.Length ());
      components.Add (object, Am_TAIL, false);
      owner.Note_Changed (Am_GRAPHICAL_PARTS);
      Main_Demon_Queue.Enqueue (graphics_repaint, 0,
			  ((Am_Object_Advanced&)object).Get_Slot (Am_VISIBLE));
    }
  }
  else if (object.Is_Instance_Of (Am_Window)) {
    Am_Drawonable* draw = Am_Drawonable::Narrow (object.Get(Am_DRAWONABLE));
    if (draw) draw->Raise_Window(NULL);
  }
  else {
    cerr << "** Am_To_Top: Attempt to raise " << object << " which is not "
      "a graphical object or window" << endl;
    Am_Error ();
  }
}

void Am_To_Bottom (Am_Object object)
{
  if (!object.Valid ())
    Am_Error ("** Am_To_Bottom called with NULL object\n");
  Am_Object owner = object.Get_Owner ();
  if (!owner.Valid ())
    return;
  if (object.Is_Instance_Of (Am_Graphical_Object)) {
    owner.Make_Unique (Am_GRAPHICAL_PARTS);
    Am_Value_List components;
    components = owner.Get (Am_GRAPHICAL_PARTS);
    int rank = object.Get (Am_RANK);
    if (rank != 0) {
      components.Start ();
      components.Member (object);
      components.Delete (false);
      Am_Object current;
      while (!components.First ()) {
	current = components.Get ();
	current.Set (Am_RANK, rank);
	--rank;
        components.Prev ();
      }
      object.Set (Am_RANK, 0);
      components.Add (object, Am_HEAD, false);
      owner.Note_Changed (Am_GRAPHICAL_PARTS);
      Main_Demon_Queue.Enqueue (graphics_repaint, 0,
			  ((Am_Object_Advanced&)object).Get_Slot (Am_VISIBLE));
    }
  }
  else if (object.Is_Instance_Of (Am_Window)) {
    Am_Drawonable* draw = Am_Drawonable::Narrow (object.Get(Am_DRAWONABLE));
    if (draw) draw->Lower_Window(NULL);
  }
  else {
    cerr << "** Am_To_Bottom: Attempt to lower " << object << " which is not "
      "a graphical object or window" << endl;
    Am_Error ();
  }
}

//////////////////////////////////////////////////////////////////////////
// Initialize section
//////////////////////////////////////////////////////////////////////////

// this function is called by Am_Initialize
void Am_Initialize_Aux ()
{
  Am_Object_Advanced temp;
  Am_Demon_Set demons;
  unsigned short demon_mask;

  int width, height;
  Am_Drawonable* root = Am_Drawonable::Get_Root_Drawonable ();
  root->Get_Size (width, height);

  Am_Screen = Am_Root_Object.Create ("Am_Screen")
    .Add (Am_LEFT, 0)
    .Add (Am_TOP, 0)
    .Add (Am_WIDTH, width)
    .Add (Am_HEIGHT, height)
    .Add (Am_DRAWONABLE, (Am_Am_Drawonable)root)
    .Add (Am_OWNER_DEPTH, 0)
    .Add (Am_RANK, 0)
    .Add (Am_VISIBLE, true)
    .Add (Am_POINT_IN_OBJ_METHOD, generic_point_in_obj)
    .Add (Am_POINT_IN_PART_METHOD, screen_point_in_part)
    .Add (Am_POINT_IN_LEAF_METHOD, screen_point_in_leaf)
    .Add (Am_TRANSLATE_COORDINATES_METHOD, generic_translate_coordinates)
    .Add (Am_OFFSCREEN_DRAWONABLE, NULL)
    .Add (Am_DRAW_BUFFER, NULL)
    .Add (Am_MASK_BUFFER, NULL)
    .Add (Am_AS_LINE, false) //screens are not like lines
    ;

  Am_Screen.Add (Am_SCREEN, Am_Screen)
    .Add (Am_IS_COLOR, screen_is_color(Am_Screen));
  
  temp = (Am_Object_Advanced&)Am_Screen;
  temp.Get_Slot (Am_SCREEN).Set_Inherit_Rule (Am_LOCAL);
  temp.Get_Slot (Am_DRAWONABLE).Set_Inherit_Rule (Am_LOCAL);
//// NYD: What other slots should Am_Screen have?  Color depth?
  Main_Demon_Queue = temp.Get_Queue ();
  demons = temp.Get_Demons ().Copy ();
  demons.Set_Object_Demon (Am_DESTROY_OBJ, screen_destroy_demon);
  temp.Set_Demons (demons);

  Am_Window = Am_Root_Object.Create ("Am_Window")
    .Add (Am_WINDOW, return_self)
#ifndef _MACINTOSH
    .Add (Am_LEFT, 0)
    .Add (Am_TOP, 0)
#else
	.Add (Am_LEFT, 20)
	.Add (Am_TOP, 50)
#endif
    .Add (Am_WIDTH, 100)
    .Add (Am_HEIGHT, 100)
    .Add (Am_GRAPHICAL_PARTS, Am_Value_List ())
    .Add (Am_MAX_WIDTH, 0)
    .Add (Am_MAX_HEIGHT, 0)
    .Add (Am_MIN_WIDTH, 1)
    .Add (Am_MIN_HEIGHT, 1)
    .Add (Am_TITLE, "Amulet")
    .Add (Am_ICON_TITLE, Am_No_Value) //use title if no icon title
    .Add (Am_ICONIFIED, false)
    .Add (Am_VISIBLE, true)
    .Add (Am_USE_MIN_WIDTH, false)
    .Add (Am_USE_MIN_HEIGHT, false)
    .Add (Am_USE_MAX_WIDTH, false)
    .Add (Am_USE_MAX_HEIGHT, false);

  Am_Window //need to split because of MSC compiler limit
    .Add (Am_DRAWONABLE, (Am_Ptr)NULL) //##
    .Add (Am_IS_COLOR, window_is_color)
    .Add (Am_RANK, 0)
    .Add (Am_QUERY_POSITION, false)
    .Add (Am_QUERY_SIZE, false)
    .Add (Am_LEFT_BORDER_WIDTH, 0)
    .Add (Am_TOP_BORDER_WIDTH, 0)
    .Add (Am_RIGHT_BORDER_WIDTH, 0)
    .Add (Am_BOTTOM_BORDER_WIDTH, 0)
    .Add (Am_FILL_STYLE, Am_White)
    .Add (Am_CURSOR, Am_Default_Cursor) // use arrow
    .Add (Am_OMIT_TITLE_BAR, false)
    .Add (Am_CLIP_CHILDREN, false)
    .Add (Am_SAVE_UNDER, false)
    .Add (Am_OWNER_DEPTH, compute_depth)
    .Add (Am_DRAW_METHOD, window_draw)
    .Add (Am_INVALID_METHOD, window_invalid)
    .Add (Am_POINT_IN_OBJ_METHOD, generic_point_in_obj)
    .Add (Am_POINT_IN_PART_METHOD, am_group_point_in_part)
    .Add (Am_POINT_IN_LEAF_METHOD, am_group_point_in_leaf)
    .Add (Am_TRANSLATE_COORDINATES_METHOD, window_translate_coordinates)
    .Add (Am_INTER_LIST, (Am_Ptr)NULL)
    .Add (Am_WINDOW_WANT_MOVE_CNT, 0)
    .Add (Am_DOUBLE_BUFFER, true)
    //.Set (Am_DOUBLE_BUFFER, false)
    .Add (Am_DESTROY_WINDOW_METHOD, Am_Default_Window_Destroy_Method)
    .Add (Am_WAITING_FOR_COMPLETION, (int)Am_INTER_NOT_WAITING)
    .Add (Am_COMPLETION_VALUE, NULL)
    .Add (Am_AS_LINE, false) //windows are not like lines
    // These are for grouping and alignment methods
    .Add (Am_LEFT_OFFSET, 0)
    .Add (Am_TOP_OFFSET, 0)
    .Add (Am_H_SPACING, 0)
    .Add (Am_V_SPACING, 0)
    .Add (Am_RIGHT_OFFSET, 0)
    .Add (Am_BOTTOM_OFFSET, 0)
    .Add (Am_H_ALIGN, Am_CENTER_ALIGN)
    .Add (Am_V_ALIGN, Am_CENTER_ALIGN)
    .Add (Am_FIXED_WIDTH, Am_NOT_FIXED_SIZE)
    .Add (Am_FIXED_HEIGHT, Am_NOT_FIXED_SIZE)
    .Add (Am_INDENT, 0)
    .Add (Am_MAX_RANK, false)
    .Add (Am_MAX_SIZE, false)
    .Add (Am_UNDO_HANDLER, NULL)
    ;
  temp = (Am_Object_Advanced&)Am_Window;
  // slots of all Windows that must be local only
  temp.Get_Slot(Am_INTER_LIST).Set_Inherit_Rule(Am_LOCAL);
  temp.Get_Slot(Am_WINDOW_WANT_MOVE_CNT).Set_Inherit_Rule(Am_LOCAL);
  //next two are for windows used as pop-ups
  temp.Get_Slot(Am_WAITING_FOR_COMPLETION).Set_Inherit_Rule(Am_LOCAL);
  temp.Get_Slot(Am_COMPLETION_VALUE).Set_Inherit_Rule(Am_LOCAL);
  
#ifdef DEBUG
  Am_Window.Add(Am_OBJECT_IN_PROGRESS, 0)
    .Set_Inherit_Rule(Am_OBJECT_IN_PROGRESS, Am_LOCAL);
#endif

  demons = temp.Get_Demons ().Copy ();
  demons.Set_Object_Demon (Am_CREATE_OBJ, window_create);
  demons.Set_Object_Demon (Am_COPY_OBJ, window_copy);
  demons.Set_Object_Demon (Am_DESTROY_OBJ, window_destroy_demon);
  demons.Set_Part_Demon (Am_ADD_PART, am_generic_add_part);
  demons.Set_Part_Demon (Am_CHANGE_OWNER, window_change_owner);
  demons.Set_Slot_Demon (Am_COMMON_SLOT, window_common_slot,
             Am_DEMON_PER_OBJECT | Am_DEMON_ON_CHANGE);
  demons.Set_Slot_Demon (Am_UNCOMMON_SLOT, window_uncommon_slot,
             Am_DEMON_PER_OBJECT | Am_DEMON_ON_CHANGE);
  demon_mask = temp.Get_Demon_Mask ();
  demon_mask |= 0x0004 | 0x0008;
  temp.Set_Demon_Mask (demon_mask);
  temp.Set_Demons (demons);
  
  temp.Get_Slot (Am_LEFT).Set_Demon_Bits (Am_COMMON_SLOT | Am_EAGER_DEMON);
  temp.Get_Slot (Am_TOP).Set_Demon_Bits (Am_COMMON_SLOT | Am_EAGER_DEMON);
  temp.Get_Slot (Am_WIDTH).Set_Demon_Bits (Am_COMMON_SLOT | Am_EAGER_DEMON);
  temp.Get_Slot (Am_HEIGHT).Set_Demon_Bits (Am_COMMON_SLOT | Am_EAGER_DEMON);
  
  temp.Get_Slot (Am_MAX_WIDTH).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                           Am_EAGER_DEMON);
  temp.Get_Slot (Am_MAX_HEIGHT).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                        Am_EAGER_DEMON);
  temp.Get_Slot (Am_MIN_WIDTH).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                           Am_EAGER_DEMON);
  temp.Get_Slot (Am_MIN_HEIGHT).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                        Am_EAGER_DEMON);
  temp.Get_Slot (Am_TITLE).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                       Am_EAGER_DEMON);
  temp.Get_Slot (Am_ICON_TITLE).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                        Am_EAGER_DEMON);
  temp.Get_Slot (Am_ICONIFIED).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                           Am_EAGER_DEMON);
  temp.Get_Slot (Am_VISIBLE).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                         Am_EAGER_DEMON);
  temp.Get_Slot (Am_USE_MIN_WIDTH).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                           Am_EAGER_DEMON);
  temp.Get_Slot (Am_USE_MIN_HEIGHT).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                            Am_EAGER_DEMON);
  temp.Get_Slot (Am_USE_MAX_WIDTH).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                           Am_EAGER_DEMON);
  temp.Get_Slot (Am_USE_MAX_HEIGHT).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                            Am_EAGER_DEMON);
/* temporary since need new form of Set to override the read-only
  temp.Get_Slot (Am_LEFT_BORDER_WIDTH).Set_Read_Only (true);
  temp.Get_Slot (Am_TOP_BORDER_WIDTH).Set_Read_Only (true);
  temp.Get_Slot (Am_RIGHT_BORDER_WIDTH).Set_Read_Only (true);
  temp.Get_Slot (Am_BOTTOM_BORDER_WIDTH).Set_Read_Only (true);
*/
  temp.Get_Slot (Am_FILL_STYLE).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                        Am_EAGER_DEMON);
  temp.Get_Slot (Am_CURSOR).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                        Am_EAGER_DEMON);
  temp.Get_Slot (Am_OMIT_TITLE_BAR).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                            Am_EAGER_DEMON);
  temp.Get_Slot (Am_CLIP_CHILDREN).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                           Am_EAGER_DEMON);
  temp.Get_Slot (Am_SAVE_UNDER).Set_Demon_Bits (Am_UNCOMMON_SLOT |
                        Am_EAGER_DEMON);

  Am_Graphical_Object = Am_Root_Object.Create ("Am_Graphical_Object")
    .Add (Am_LEFT, 0)
    .Add (Am_TOP, 0)
    .Add (Am_WIDTH, 10)
    .Add (Am_HEIGHT, 10)
    .Add (Am_VISIBLE, true)
    .Add (Am_RANK, -1)
    .Add (Am_OWNER_DEPTH, compute_depth)
    .Add (Am_WINDOW, pass_window)
    .Add (Am_PREV_STATE, NULL)
    .Add (Am_DRAW_METHOD, NULL)
    .Add (Am_MASK_METHOD, generic_mask)
    .Add (Am_INVALID_METHOD, NULL)
    .Add (Am_POINT_IN_OBJ_METHOD, generic_point_in_obj)
    .Add (Am_POINT_IN_PART_METHOD, generic_point_in_part)
    .Add (Am_POINT_IN_LEAF_METHOD, generic_point_in_leaf)
    .Add (Am_TRANSLATE_COORDINATES_METHOD, generic_translate_coordinates)
    .Add (Am_AS_LINE, false) //most objects are not like lines
    ;
  temp = (Am_Object_Advanced&)Am_Graphical_Object;
  demons = temp.Get_Demons ().Copy ();
  demons.Set_Object_Demon (Am_CREATE_OBJ, graphics_create);
  demons.Set_Object_Demon (Am_COPY_OBJ, graphics_create);
  demons.Set_Object_Demon (Am_DESTROY_OBJ, graphics_destroy);
  demons.Set_Part_Demon (Am_CHANGE_OWNER, graphics_change_owner);
  demons.Set_Slot_Demon (Am_STATIONARY_REDRAW, graphics_repaint,
             Am_DEMON_PER_OBJECT | Am_DEMON_ON_CHANGE);
  demons.Set_Slot_Demon (Am_MOVING_REDRAW, graphics_move,
             Am_DEMON_PER_OBJECT | Am_DEMON_ON_CHANGE);
  demons.Set_Type_Check (1, Am_Check_Int_Type);
  demon_mask = temp.Get_Demon_Mask ();
  demon_mask |= Am_STATIONARY_REDRAW | Am_MOVING_REDRAW;
  temp.Set_Demon_Mask (demon_mask);
  temp.Set_Demons (demons);
  temp.Get_Slot (Am_LEFT).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_LEFT).Set_Type_Check (1);
  temp.Get_Slot (Am_TOP).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_TOP).Set_Type_Check (1);
  temp.Get_Slot (Am_WIDTH).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_HEIGHT).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_VISIBLE).Set_Demon_Bits (Am_MOVING_REDRAW |
                         Am_EAGER_DEMON);
  temp.Get_Slot (Am_PREV_STATE).Set_Inherit_Rule (Am_LOCAL);
#ifdef DEBUG
  Am_Graphical_Object.Add(Am_OBJECT_IN_PROGRESS, 0)
    .Set_Inherit_Rule(Am_OBJECT_IN_PROGRESS, Am_LOCAL);
#endif

}
