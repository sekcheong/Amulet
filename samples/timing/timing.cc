/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#include <amulet.h>
#include OPAL_ADVANCED__H //for update_all
#include GEM__H //for pretend input

#ifdef _MACINTOSH
#include <SIOUX.h>
#endif

/* This file contains code to time various functions in Amulet.
   It is used to measure improvements and to make sure new features
   don't make Amulet slower.
*/   

class timing_info
{
public:
  const char * name;
  unsigned long total_time;
  unsigned long cnt;
  float time_each;
};
int my_index = 0;
timing_info all_times[100];

void print_summary() {
  cout << "\n---------------------\nSummary (DEBUGGING "
#ifdef DEBUG
       << "on"
#else
       << "off"
#endif       
       << ") (in msec for each): \n";
  int i;
  for (i=0;i<my_index; i++) {
    cout << i << ".  " << all_times[i].time_each << " for "
	 << all_times[i].name << endl;
  }
  cout << flush;
}
 
Am_Time start_time, end_time;

void start_timing() {
  start_time = Am_Time::Now();
}

void end_timing(const char* message, long cnt) {
  Am_Time total_time;

  end_time = Am_Time::Now();
  total_time = end_time - start_time;

  cout << my_index << ". Time to " << message << " = " << total_time << endl;

  unsigned long total_msec;
  total_msec = total_time.Milliseconds();
  float time_each;
  if (cnt > 1) {
    time_each = (float) total_msec / (float) cnt;

    float persec = 1000.0 / time_each;
    cout << "    for " << cnt << " times = " << time_each
	 << " msec each, or " << persec << " per sec\n";
  }
  else time_each = total_msec;

  cout << flush;
  all_times[my_index].name = message;
  all_times[my_index].total_time = total_msec;
  all_times[my_index].cnt = cnt;
  all_times[my_index].time_each = time_each;
  my_index++;
}

Am_Slot_Key NEW_SLOT = Am_Register_Slot_Name ("NEW_SLOT");
Am_Slot_Key MY_LEFT = Am_Register_Slot_Name ("MY_LEFT");
Am_Slot_Key NEW_SLOT_INH = Am_Register_Slot_Name ("NEW_SLOT_INH");
Am_Slot_Key NEW_SLOT_INH2 = Am_Register_Slot_Name ("NEW_SLOT_INH2");

Am_Object win;

Am_Define_Formula (int, int_constraint) {
/*
  static int idx = 0;
  idx++;
  if( idx % 10 == 0 )
    cout << "in int_constraint " << idx++ << endl;
*/
  return (int)self.Get(NEW_SLOT) + 1;
}
Am_Define_Style_Formula(toggle_color) {
  if ((bool)self.Get(Am_SELECTED)) return Am_Red;
  else return Am_Black;
}

int meth_sum = 0;
Am_Define_Method(Am_Object_Method, void, small_method,
		 (Am_Object /* obj */)) {
  meth_sum++;
}

#define NUM_OBJ_CREATE 5000
#define NUM_OBJ_SET 100000
#define NUM_UPDATE_RECT 3000
#define NUM_UPDATE_RECT_OVER_OBJS 100
#define NUM_PRETEND_PRESS 2000
#define NUM_OBJ_DESTROY 50

Am_Drawonable* draw_for_win;
Am_Input_Event_Handlers *evh;
Am_Input_Event ev;

void init_pretend_input(Am_Object win) {
  draw_for_win = (Am_Drawonable*)(Am_Ptr)win.Get(Am_DRAWONABLE);
  draw_for_win->Get_Input_Dispatch_Functions(evh);
  ev.input_char = Am_Input_Char(Am_LEFT_MOUSE, false, false, false,
				Am_BUTTON_DOWN, Am_SINGLE_CLICK);
  ev.draw = draw_for_win;
  //hope ev.timestamp doesn't matter
}
void pretend_input(Am_Button_Down down, int x, int y) {
  ev.input_char.button_down = down;
  ev.x = x;
  ev.y = y;
  evh->Input_Event_Notify(draw_for_win, &ev);
}

void time_object_creation() {
  Am_Object objs[NUM_OBJ_CREATE];
  long i;
  int cnt;
  
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    objs[i] = Am_Root_Object.Create();
  }
  Am_Update_All ();
  end_timing("create objects from Root", NUM_OBJ_CREATE);
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    objs[i].Destroy();
  }
  Am_Update_All ();
  end_timing("destroy objects from Root", NUM_OBJ_CREATE);
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    objs[i] = Am_Root_Object.Create();
  }
  Am_Update_All ();
  end_timing("create objects from Root 2nd time", NUM_OBJ_CREATE);
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    objs[i].Destroy();
  }
  Am_Update_All ();
  end_timing("destroy objects from Root 2nd time", NUM_OBJ_CREATE);

  Am_Graphical_Object.Add(NEW_SLOT_INH2, 2);
  Am_Rectangle.Add(NEW_SLOT_INH, 1);
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    objs[i] = Am_Rectangle.Create();
  }
  Am_Update_All ();
  end_timing("create objects from Am_Rectangle", NUM_OBJ_CREATE);
  
  cnt = 1;
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    objs[i].Add(NEW_SLOT, cnt++);
  }
  Am_Update_All ();
  end_timing("Add (set) a NEW slot in obj", NUM_OBJ_CREATE);
  
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    objs[i].Set(NEW_SLOT, cnt--);
  }
  Am_Update_All ();
  end_timing("set slot again diff objs", NUM_OBJ_CREATE);
  

  Am_Object one_rect = Am_Rectangle.Create();
  Am_Object one_root = Am_Root_Object.Create();
  one_root.Add(MY_LEFT, -1);
  one_rect.Add(MY_LEFT, -1);
  start_timing();
  for (i=0;i<NUM_OBJ_SET; i++) {
    if (cnt++ > 30000) cnt = 0;
    one_root.Set(MY_LEFT, cnt);
  }
  Am_Update_All ();
  end_timing("set MY_LEFT slot repeatedly in root", NUM_OBJ_SET);
  start_timing();
  for (i=0;i<NUM_OBJ_SET; i++) {
    if (cnt++ > 30000) cnt = 0;
    one_rect.Set(Am_LEFT, cnt);
  }
  Am_Update_All ();
  end_timing("set Am_LEFT slot repeatedly in rect", NUM_OBJ_SET);

  Am_Value v;
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    v = objs[i].Get(NEW_SLOT);
  }
  end_timing("get value as Am_Value", NUM_OBJ_CREATE);

  long sum = 0;
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    sum = sum + (int)objs[i].Get(NEW_SLOT);
  }
  end_timing("get value as int different objects", NUM_OBJ_CREATE);

  one_rect.Add(NEW_SLOT, 15);
  sum = 0;
  start_timing();
  for (i=0;i<NUM_OBJ_SET; i++) {
    sum = sum + (int)one_rect.Get(NEW_SLOT);
  }
  end_timing("get value same rect as int", NUM_OBJ_SET);

  one_root.Add(NEW_SLOT, Am_Window);
  Am_Object o;
  start_timing();
  for (i=0;i<NUM_OBJ_SET; i++) {
    o = one_root.Get(NEW_SLOT);
  }
  end_timing("get value as Am_Object", NUM_OBJ_SET);

  start_timing();
  int int2;
  for (i=0;i<NUM_OBJ_SET; i++) {
    if (cnt++ > 30000) cnt = 0;
    one_rect.Set(MY_LEFT, cnt);
    int2 = one_rect.Get(MY_LEFT);
  }
  Am_Update_All ();
  end_timing("set + get as int", NUM_OBJ_SET);
  
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
//    if( i % 200 == 0 )
//      cout << i << endl;
    objs[i].Add(MY_LEFT, int_constraint);
  }
  Am_Update_All ();
//  cout << "Am_Update_All completed" << endl;
  end_timing("install a constraint into MY_LEFT", NUM_OBJ_CREATE);
  
  one_rect.Get(MY_LEFT);
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    if (cnt++ > 30000) cnt = 0;
    objs[i].Set(NEW_SLOT, cnt);
  }
  end_timing("set NEW_SLOT, invalidating all constraints", NUM_OBJ_CREATE);
  
  start_timing();
  one_rect.Get(MY_LEFT);
  end_timing("one get which validates all constraints", NUM_OBJ_CREATE);

  one_rect.Set(MY_LEFT, int_constraint);
  start_timing();
  for (i=0;i<NUM_OBJ_SET; i++) {
    if (cnt++ > 30000) cnt = 0;
    one_rect.Set(NEW_SLOT, cnt);
    int2 = one_rect.Get(MY_LEFT);
  }
  Am_Update_All ();
  end_timing("set NEW_SLOT, get(MY_LEFT) from constraint", NUM_OBJ_SET);

  sum = 0;
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    sum = sum + (int)objs[i].Get(NEW_SLOT_INH);
  }
  Am_Update_All ();
  end_timing("get value rect INHERITED one level", NUM_OBJ_CREATE);

  sum = 0;
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    sum = sum + (int)objs[i].Get(NEW_SLOT_INH2);
  }
  Am_Update_All ();
  end_timing("get value rect inherited TWO levels", NUM_OBJ_CREATE);

  Am_Object callme = Am_Rectangle.Create()
    .Add(NEW_SLOT, small_method);
  Am_Object_Method meth;
  start_timing();
  for (i=0;i<NUM_OBJ_SET; i++) {
    meth = callme.Get(NEW_SLOT);
    meth.Call(callme);
  }
  Am_Update_All ();
  end_timing("get method and call it", NUM_OBJ_SET);
  

  Am_Object win;
  start_timing();
  win = Am_Window.Create()
    .Set(Am_TOP, 50)
    .Set(Am_LEFT, 50)
    .Set(Am_WIDTH, 400)
    .Set(Am_HEIGHT, 400);
  Am_Screen.Add_Part(win);
  Am_Update_All();
  end_timing("create window and update", 0);

  Am_Object new_rect = Am_Rectangle.Create()
    .Set(Am_LEFT,0)
    .Set(Am_TOP, 112)
    .Set(Am_WIDTH, 40)
    .Set(Am_HEIGHT, 40)
    .Set(Am_FILL_STYLE, Am_Red);
  win.Add_Part(new_rect);
  Am_Update_All();
  cnt = 0;
  start_timing();
  for (i=0;i<NUM_UPDATE_RECT; i++) {
    if (cnt++ > 350) cnt = 0;
    new_rect.Set(Am_LEFT, cnt);
    Am_Update_All();
  }
  end_timing("update rect double-buffered", NUM_UPDATE_RECT);
  new_rect.Destroy();

  new_rect = Am_Button.Create()
    .Set(Am_LEFT,0)
    .Set(Am_TOP, 112);
  win.Add_Part(new_rect);
  Am_Update_All();
  cnt = 0;
  start_timing();
  for (i=0;i<NUM_UPDATE_RECT; i++) {
    if (cnt++ > 350) cnt = 0;
    new_rect.Set(Am_LEFT, cnt);
    Am_Update_All();
  }
  end_timing("update button double-buffered", NUM_UPDATE_RECT);
  new_rect.Destroy();

  static int tria [8] = {25, 50, 50, 100, 75, 50, 25, 50};
  Am_Point_List triangle (tria, 8);
  new_rect = Am_Polygon.Create ("triangle")
    .Set (Am_FILL_STYLE, Am_Red)
    .Set (Am_POINT_LIST, triangle);
  win.Add_Part(new_rect);
  Am_Update_All();
  cnt = 0;
  start_timing();
  for (i=0;i<NUM_UPDATE_RECT; i++) {
    if (cnt++ > 300) cnt = 0;
    new_rect.Set(Am_LEFT, cnt);
    Am_Update_All();
  }
  end_timing("update triangle double-buffered", NUM_UPDATE_RECT);
  new_rect.Destroy();

  //////////////////////// not double buffered
  Am_Object win2;
  win2 = Am_Window.Create()
    .Set(Am_TOP, 50)
    .Set(Am_LEFT, 470)
    .Set(Am_WIDTH, 400)
    .Set(Am_HEIGHT, 400)
    .Set(Am_DOUBLE_BUFFER, false);
  Am_Screen.Add_Part(win2);
  Am_Update_All();

  new_rect = Am_Rectangle.Create()
    .Set(Am_LEFT,0)
    .Set(Am_TOP, 112)
    .Set(Am_WIDTH, 40)
    .Set(Am_HEIGHT, 40)
    .Set(Am_FILL_STYLE, Am_Red);
  win2.Add_Part(new_rect);
  Am_Update_All();
  cnt = 0;
  start_timing();
  for (i=0;i<NUM_UPDATE_RECT; i++) {
    if (cnt++ > 350) cnt = 0;
    new_rect.Set(Am_LEFT, cnt);
    Am_Update_All();
  }
  end_timing("update rect NOT double-buffered", NUM_UPDATE_RECT);
  new_rect.Destroy();

  new_rect = Am_Button.Create()
    .Set(Am_LEFT,0)
    .Set(Am_TOP, 112);
  win2.Add_Part(new_rect);
  Am_Update_All();
  cnt = 0;
  start_timing();
  for (i=0;i<NUM_UPDATE_RECT; i++) {
    if (cnt++ > 350) cnt = 0;
    new_rect.Set(Am_LEFT, cnt);
    Am_Update_All();
  }
  end_timing("update button NOT double-buffered", NUM_UPDATE_RECT);
  new_rect.Destroy();

  new_rect = Am_Polygon.Create ("triangle")
    .Set (Am_FILL_STYLE, Am_Red)
    .Set (Am_POINT_LIST, triangle);
  win2.Add_Part(new_rect);
  Am_Update_All();
  cnt = 0;
  start_timing();
  for (i=0;i<NUM_UPDATE_RECT; i++) {
    if (cnt++ > 300) cnt = 0;
    new_rect.Set(Am_LEFT, cnt);
    Am_Update_All();
  }
  end_timing("update triangle NOT double-buffered", NUM_UPDATE_RECT);

  start_timing();
  win2.Destroy();
  Am_Update_All();
  end_timing("destroy window containing polygon", 0);
  

  ///////////////////////////////// pretend input ////////////////
  init_pretend_input(win);
  start_timing();
  for (i=0;i<NUM_PRETEND_PRESS; i++) {
    pretend_input(Am_BUTTON_DOWN, 10, 10);
    pretend_input(Am_BUTTON_UP, 10, 10);
  }
  Am_Update_All ();
  end_timing("pretend DOWN+UP, no interactors", NUM_PRETEND_PRESS);
  
  new_rect = Am_Rectangle.Create()
    .Set(Am_LEFT,112)
    .Set(Am_TOP, 112)
    .Set(Am_WIDTH, 40)
    .Set(Am_HEIGHT, 40)
    .Set(Am_FILL_STYLE, Am_Red)
    .Add_Part(Am_One_Shot_Interactor.Create());
  win.Add_Part(new_rect);
  Am_Update_All();
  start_timing();
  for (i=0;i<NUM_PRETEND_PRESS; i++) {
    pretend_input(Am_BUTTON_DOWN, 113, 113);
    pretend_input(Am_BUTTON_UP, 113, 113);
  }
  Am_Update_All ();
  end_timing("invoke one_shot_inter, no graphics", NUM_PRETEND_PRESS);

  new_rect.Set(Am_FILL_STYLE, toggle_color);
  Am_Update_All();
  start_timing();
  for (i=0;i<NUM_PRETEND_PRESS; i++) {
    pretend_input(Am_BUTTON_DOWN, 113, 113);
    pretend_input(Am_BUTTON_UP, 113, 113);
    Am_Update_All();
  }
  end_timing("invoke one_shot_inter, toggle color", NUM_PRETEND_PRESS);
  new_rect.Destroy();

  new_rect = Am_Rectangle.Create()
    .Set(Am_LEFT, 2)
    .Set(Am_TOP, 112)
    .Set(Am_WIDTH, 40)
    .Set(Am_HEIGHT, 40)
    .Set(Am_FILL_STYLE, Am_Green)
    .Add_Part(Am_Move_Grow_Interactor.Create());
  win.Add_Part(new_rect);
  pretend_input(Am_BUTTON_DOWN, 3, 113);
  ev.input_char = Am_Input_Char(Am_MOUSE_MOVED, false, false, false,
				Am_NEITHER, Am_SINGLE_CLICK);
  Am_Update_All();
  start_timing();
  cnt = 0;
  for (i=0;i<NUM_PRETEND_PRESS; i++) {
    if (cnt++ > 350) cnt = 0;
    pretend_input(Am_NEITHER, cnt, 113);
    Am_Update_All();
  }
  end_timing("move_grow_inter", NUM_PRETEND_PRESS);
  pretend_input(Am_BUTTON_UP, cnt, 113);
  new_rect.Destroy();

  ///////////////////////////////// rects in window ////////////////
  cnt = 0;
  int y = 0;
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    objs[i].Set(Am_LEFT, cnt);
    objs[i].Set(Am_TOP, y);
    objs[i].Set(Am_FILL_STYLE, Am_Yellow);
    cnt += 5;
    if (cnt > 350) {
      cnt = 0;
      y += 5;
    }
  }
  start_timing();
  for (i=0;i<NUM_OBJ_CREATE; i++) {
    win.Add_Part(objs[i]);
  }
  end_timing("Add_Part rect to window", NUM_OBJ_CREATE);
  start_timing();
  Am_Update_All();
  end_timing("Update Window with 5000 Rectangles", 0);

  new_rect = Am_Rectangle.Create()
    .Set(Am_LEFT,0)
    .Set(Am_TOP, 112)
    .Set(Am_WIDTH, 40)
    .Set(Am_HEIGHT, 40)
    .Set(Am_FILL_STYLE, Am_Red);
  win.Add_Part(new_rect);
  Am_Update_All();
  cnt = 0;
  start_timing();
  for (i=0;i<NUM_UPDATE_RECT_OVER_OBJS; i++) {
    if (cnt++ > 350) cnt = 0;
    new_rect.Set(Am_LEFT, cnt);
    Am_Update_All();
  }
  end_timing("update rect over other rects", NUM_UPDATE_RECT_OVER_OBJS);

  start_timing();
  for (i=0;i<NUM_OBJ_DESTROY; i++) {
    objs[i].Destroy();
  }
  Am_Update_All ();
  end_timing("destroy objects while in window", NUM_OBJ_DESTROY);

  //destroy the rest of the objects
  win.Destroy();
  for (i=NUM_OBJ_DESTROY;i<NUM_OBJ_CREATE; i++) {
    objs[i].Destroy();
  }
}
  

  
int main( void )
{
#ifdef DEBUG
  cout << "Started, debugging is ON\n" << flush;
#else
  cout << "Started, debugging is OFF\n" << flush;
#endif

#ifdef _MACINTOSH
  SIOUXSettings.asktosaveonclose = true;
#endif

  start_timing();
  Am_Initialize ();
  Am_Update_All ();
  end_timing("Initialize", 0);
  
  time_object_creation();

  Am_Update_All();
  start_timing();
  Am_Cleanup ();
  end_timing("Cleanup", 0);
  print_summary();

  return 0;
}
