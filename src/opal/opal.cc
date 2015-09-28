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

#include AM_IO__H
#include MISC__H // Am_Get_Amulet_Pathname

#include STANDARD_SLOTS__H
#include OPAL_ADVANCED__H
#include VALUE_LIST__H
#include INTER_ADVANCED__H // to get Am_Initialize_Interactors
#include WIDGETS_ADVANCED__H

#include DEBUGGER__H
#include FORMULA__H
#include GEM__H
#include OPAL__H
#include WEB__H
#include REGISTRY__H
#include GESTURE__H  // to get Am_Gesture_Initialize

// Data for the built-in Am_Diamond_Stipple
#if !defined(_WINDOWS)
static char diamond_bits[] = {
  0x80, 0x00, 0xc0, 0x01, 0xe0, 0x03, 0xf0, 0x07, 0xf8, 0x0f, 0xfc, 0x1f,
  0xfe, 0x3f, 0xff, 0x7f, 0xfe, 0x3f, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x07,
  0xe0, 0x03, 0xc0, 0x01, 0x80, 0x00, 0x00, 0x00};
#else
static const short diamond_bits[] = {
//	0x07, 0x13, 0x31, 0x70,  // an interesting pattern
//	0x0e, 0x8c, 0xc8, 0xe0   // but not diamond fill
  0x08, 0x1c, 0x3e, 0x7f,
  0xfe, 0x7c, 0x38, 0x10
};
#endif

///////////////////////
// Standard Opal styles
// defined in Am_Initialize ();

Am_Style Am_Red;
Am_Style Am_Green;
Am_Style Am_Blue;
Am_Style Am_Yellow;
Am_Style Am_Purple;
Am_Style Am_Cyan;
Am_Style Am_Orange;
Am_Style Am_Black;
Am_Style Am_White;
Am_Style Am_Amulet_Purple;

Am_Style Am_Motif_Gray;
Am_Style Am_Motif_Light_Gray;
Am_Style Am_Motif_Blue;
Am_Style Am_Motif_Light_Blue;
Am_Style Am_Motif_Green;
Am_Style Am_Motif_Light_Green;
Am_Style Am_Motif_Orange;
Am_Style Am_Motif_Light_Orange;

Am_Style Am_Thin_Line;
Am_Style Am_Line_0;
Am_Style Am_Line_1;
Am_Style Am_Line_2;
Am_Style Am_Line_4;
Am_Style Am_Line_8;
Am_Style Am_Dotted_Line;
Am_Style Am_Dashed_Line;
					    
Am_Style Am_Gray_Stipple;
Am_Style Am_Opaque_Gray_Stipple;
Am_Style Am_Light_Gray_Stipple;
Am_Style Am_Dark_Gray_Stipple;
Am_Style Am_Diamond_Stipple;
Am_Style Am_Opaque_Diamond_Stipple;

Am_Font Am_Default_Font (Am_FONT_FIXED, false, false, false, Am_FONT_MEDIUM);
Am_Font Am_Japanese_Font (Am_FONT_JFIXED, false, false, false, Am_FONT_TMEDIUM);
		// Japanese standard font

Am_Object Am_Screen;

Am_Object Am_Graphical_Object;
Am_Object Am_Window;
Am_Object Am_Rectangle;
Am_Object Am_Roundtangle;
Am_Object Am_Line;
Am_Object Am_Arrow_Line;
Am_Object Am_Polygon;
Am_Object Am_Arc;
Am_Object Am_Text;
Am_Object Am_Hidden_Text; //for passwords
Am_Object Am_Bitmap;

Am_Object Am_Aggregate;
Am_Object Am_Group;
Am_Object Am_Map;
Am_Object Am_Resize_Parts_Group;
Am_Object Am_Fade_Group;
Am_Object Am_Flip_Book_Group;

// exported types
Am_Define_Method_Type_Impl (Am_Draw_Method);
Am_Define_Method_Type_Impl (Am_Invalid_Method);
Am_Define_Method_Type_Impl (Am_Point_In_Method);
Am_Define_Method_Type_Impl (Am_Point_In_Or_Self_Method);
Am_Define_Method_Type_Impl (Am_Translate_Coordinates_Method);
Am_Define_Method_Type_Impl (Am_Item_Method);
Am_Define_Method_Type_Impl (Am_Save_Object_Method);

Am_Define_Enum_Support (Am_Alignment, "Am_CENTER_ALIGN Am_TOP_ALIGN "
			"Am_BOTTOM_ALIGN Am_LEFT_ALIGN Am_RIGHT_ALIGN");

// main global demon queue

Am_Demon_Queue Main_Demon_Queue;

inline int imin(int i1, int i2) {if (i1<i2) return i1; else return i2;}
inline int imax(int i1, int i2) {if (i1>i2) return i1; else return i2;}
inline int iabs(int i1) {if (i1<0) return -i1; else return i1;}

// These are utility functions that return either the drawonable of the
// window that the object is displayed in, or the drawonable of the
// root window (e.g., if the object is not yet in a window, or the window
// has not been mapped yet).
Am_Drawonable* Get_a_drawonable (Am_Object obj) {
  Am_Drawonable *drawonable;
  Am_Object window, owner;

  // Please don't try to clean this up (i.e. by turning it into a single
  // complicated if statement).  Solaris gcc 2.7.0 produces buggy code
  // if you do. -- rcm
  if (obj.Valid()) {
    window = obj.Get (Am_WINDOW, Am_NO_DEPENDENCY);
    if (window.Valid()) {
      owner = obj.Get_Owner (Am_NO_DEPENDENCY);
      if (owner.Valid()) {
	drawonable = Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE, Am_NO_DEPENDENCY));
	if (drawonable)
	  return drawonable;
      }
    }
  }
  // ELSE...
    // If the object is not valid, or
    // if the object is not in a window yet,
    // or the window's drawonable has not yet been created,
    // then use the drawonable for the root screen.
  drawonable = Am_Drawonable::Narrow (Am_Screen.Get (Am_DRAWONABLE, Am_NO_DEPENDENCY));
  return drawonable;
}

Am_Drawonable* GV_a_drawonable (Am_Object obj) {
  Am_Drawonable *drawonable;
  Am_Object window, owner;

  // Please don't try to clean this up (i.e. by turning it into a single
  // complicated if statement).  Solaris gcc 2.7.0 produces buggy code
  // if you do. -- rcm
  window = obj.Get (Am_WINDOW, Am_OK_IF_NOT_THERE);
  if (window.Valid()) {
    owner = obj.Get_Owner ();
    if (owner.Valid()) {
      drawonable = Am_Drawonable::Narrow (window.Get (Am_DRAWONABLE));
      if (drawonable)
	return drawonable;
    }
  }

  // ELSE...
    // If the object is not valid, or
    // if the object is not in a window yet,
    // or the window's drawonable has not yet been created,
    // then use the drawonable for the root screen.
  drawonable = Am_Drawonable::Narrow (Am_Screen.Get (Am_DRAWONABLE));
  return drawonable;
}

// Beeps at window's drawonable.  If window is not valid, beeps at default
// screen.
// Defaults: window = Am_No_Object

void Am_Beep (Am_Object window)
{
  Am_Drawonable *d = Get_a_drawonable (window);
  d->Beep();
  d->Flush_Output();
}

// Color conversion routines.  RGB to HSV and HSV to RGB
// Algorithms grabbed from Foley & vanDam
void Am_RGB_To_HSV (float r, float g, float b, float& h, float& s, float& v)
{
  float min, max;
  if (r > g) {
    max = r; min = g;
  }
  else {
    max = g; min = r;
  }
  if (max < b)
    max = b;
  if (min > b)
    min = b;
  v = max;
  if (max != 0.0)
    s = (max - min) / max;
  else
    s = 0.0;
  if (s != 0.0) {
    float delta = max - min;
    if (r == max)
      h = (g - b) / delta;
    else if (g == max)
      h = 2 + (b - r) / delta;
    else if (b == max)
      h = 4 + (r - g) / delta;
    h = h * 60.0;
    if (h < 0.0)
      h += 360.0;
  }
  else
    h = 0.0;
}

void Am_HSV_To_RGB (float h, float s, float v, float& r, float& g, float& b)
{
  if (s == 0.0) {
    r = v;
    g = v;
    b = v;
  }
  else {
    while (h >= 360.0)
      h -= 360.0;
    h = h / 60.0;
    int region = (int)h;
    float frac = h - (float)region;
    float p = v * (1 - s);
    float q = v * (1 - (s * frac));
    float t = v * (1 - (s * (1 - frac)));
    switch (region) {
    case 0:
      r = v; g = t; b = p;
      break;
    case 1:
      r = q; g = v; b = p;
      break;
    case 2:
      r = p; g = v; b = t;
      break;
    case 3:
      r = p; g = q; b = v;
      break;
    case 4:
      r = t; g = p; b = v;
      break;
    case 5:
      r = v; g = p; b = q;
      break;
    }
  }
}

//////////////////////////////////////////////////////////
///  helper functions for POINT_IN_PART methods

// gets the real line_thickness of the line_style of obj
static int get_line_thickness (Am_Object obj)
{
  Am_Style ls = obj.Get(Am_LINE_STYLE);
  if (ls) {
    Am_Line_Cap_Style_Flag cap;
    short thickness;
    ls.Get_Line_Thickness_Values (thickness, cap);
    return thickness ? thickness : 1;
  }
  else
    return 0;  // no thickness because no line_style
}

///  point_in_line_segment: returns true iff (x,y) lies within 
///   distance "threshold" of the line segment <(x1,y1), (x2,y2)>

static bool point_in_line_segment (int x, int y, 
				   int x1, int y1, int x2, int y2,
				   int threshold)
{
  int left, right, top, bottom;
  if (x1 < x2) {
    left = x1; right = x2;
  }
  else {
    left = x2; right = x1;
  }
  if (y1 < y2) {
    top = y1; bottom = y2;
  }
  else {
    top = y2; bottom = y1;
  }
  
  // do simple bounding box test first
  if (x < left-threshold || x > right+threshold ||
      y < top-threshold || y > bottom+threshold) {
    return false;
  }

  // The old line hit formula.  Seems to work okay.
  
  // equation for line is ax + by + c = 0
  // d/sqrt(a^2+b^2) is the distance between line and point <x,y>
  long a = y1 - y2; //long type needed for Windows
  long b = x2 - x1;
  float c = (float)(long(x1)*y2 - long(x2)*y1);
  float d = ((float)a)*x + ((float)b)*y + c;
  
  return (d*d <= long(threshold) * threshold * (a*a + b*b));
}

/****************************************
 ** Functions to support drawing lines **
 ****************************************/

//web functions
bool line_x_create (const Am_Slot& slot)
{
  return slot.Get_Key () == Am_X1;
}

void line_x_init (const Am_Slot& slot,
		  Am_Web_Init& init)
{
  Am_Object_Advanced obj = slot.Get_Owner ();
  init.Note_Input (obj, Am_X1);
  init.Note_Input (obj, Am_X2);
  init.Note_Input (obj, Am_LEFT);
  init.Note_Input (obj, Am_WIDTH);
  init.Note_Input (obj, Am_LINE_STYLE);
  init.Note_Output (obj, Am_X2);
  init.Note_Output (obj, Am_LEFT);
  init.Note_Output (obj, Am_WIDTH);
}

static void get_thickness_adjustments (const Am_Style& ls, int& lt_adjustment,
				       int& rb_adjustment, int& wh_adjustment)
{
  if (ls.Valid ()) {
    Am_Line_Cap_Style_Flag cap;
    short thickness;
    ls.Get_Line_Thickness_Values (thickness, cap);
    thickness = thickness ? thickness : 1;
    if (cap == Am_CAP_PROJECTING) {
      lt_adjustment = thickness;
      rb_adjustment = thickness;
      wh_adjustment = (2 * thickness);
    }
    else {
      lt_adjustment = thickness / 2;
      rb_adjustment = (thickness+1) / 2;
      wh_adjustment = thickness;
    }
  }
  else {
    lt_adjustment = 0;
    rb_adjustment = 0;
    wh_adjustment = 1;
  }
}

void line_x_validate (Am_Web_Events& events)
{
  bool x1x2_changed = false;
  bool leftwidth_changed = false;

  events.Start ();
  Am_Slot slot = events.Get ();
  Am_Object self = slot.Get_Owner ();
  int x1 = self.Get (Am_X1);
  int x2 = self.Get (Am_X2);
  int left = self.Get (Am_LEFT);
  int width = self.Get (Am_WIDTH);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  int lt_adjustment, rb_adjustment, wh_adjustment;
  get_thickness_adjustments (ls, lt_adjustment, rb_adjustment, wh_adjustment);
  while (!events.Last ()) {
    slot = events.Get ();
    // cout << "__Line_x_Web " << self << " slot " << slot.Get_Key ()
    //  << " l,w,x1,x2 " << left << " " << width << " " << x1 << " " << x2
    //	 << endl << flush;
    switch (slot.Get_Key ()) {
    case Am_X1:
    case Am_X2:
      left = imin (x1, x2) - lt_adjustment;
      width  = iabs (x2 - x1) + wh_adjustment;
      leftwidth_changed = true;
      // cout << "__  left " << left << " width " << width << endl << flush;
      break;
    case Am_LEFT:
    case Am_WIDTH:
      if (x1 < x2) {
	x1 = left + lt_adjustment;
	x2 = left + width - rb_adjustment;
      } else {
	x1 = left + width - rb_adjustment;
	x2 = left + lt_adjustment;
      }
      x1x2_changed = true;
      // cout << "__  x1 " << x1 << " x2 " << x2 << endl << flush;
      break;
    case Am_LINE_STYLE:
      break;
    default:
      Am_ERROR ("** Bug: unexpected slot in line_x_validate: " << Am_Get_Slot_Name (slot.Get_Key()));
    }
    events.Next ();
  }
  
  if (x1x2_changed) {
    self.Set (Am_X1, x1);
    self.Set (Am_X2, x2);
  }

  if (leftwidth_changed) {
    self.Set (Am_LEFT, left);
    self.Set (Am_WIDTH, width);
  }
}

bool line_y_create (const Am_Slot& slot)
{
  return slot.Get_Key () == Am_Y1;
}

void line_y_init (const Am_Slot& slot,
		  Am_Web_Init& init)
{
  Am_Object_Advanced obj = slot.Get_Owner ();
  init.Note_Input (obj, Am_Y1);
  init.Note_Input (obj, Am_Y2);
  init.Note_Input (obj, Am_TOP);
  init.Note_Input (obj, Am_HEIGHT);
  init.Note_Input (obj, Am_LINE_STYLE);
  init.Note_Output (obj, Am_Y2);
  init.Note_Output (obj, Am_TOP);
  init.Note_Output (obj, Am_HEIGHT);
}

void line_y_validate (Am_Web_Events& events)
{
  bool y1y2_changed = false;
  bool topheight_changed = false;

  events.Start ();
  Am_Slot slot = events.Get ();
  Am_Object self = slot.Get_Owner ();
  int y1 = self.Get (Am_Y1);
  int y2 = self.Get (Am_Y2);
  int top = self.Get (Am_TOP);
  int height = self.Get (Am_HEIGHT);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  int lt_adjustment, wh_adjustment, rb_adjustment;
  get_thickness_adjustments (ls, lt_adjustment, rb_adjustment, wh_adjustment);
  while (!events.Last ()) {
    slot = events.Get ();
    // cout << "__Line_y_Web " << self << " slot " << slot.Get_Key ()
    //  << " t,h,y1,y2 " << top << " " << height << " " << y1 << " " << y2
    //	 << endl << flush;
    switch (slot.Get_Key ()) {
    case Am_Y1:
    case Am_Y2:
      top = imin (y1, y2) - lt_adjustment;
      height  = iabs (y2 - y1) + wh_adjustment;
      topheight_changed = true;
      // cout << "__  top " << top << " height " << height << endl << flush;
      break;
    case Am_TOP:
    case Am_HEIGHT:
      if (y1 < y2) {
	y1 = top + lt_adjustment;
	y2 = top + height - rb_adjustment;
      }
      else {
	y1 = top + height - rb_adjustment;
	y2 = top + lt_adjustment;
      }
      y1y2_changed = true;
      // cout << "__  y1 " << y1 << " y2 " << y2 << endl << flush;
      break;
    case Am_LINE_STYLE:
      break;
    default:
      Am_ERROR ("** Bug: unexpected slot in line_y_validate: " << Am_Get_Slot_Name (slot.Get_Key()));
    }
    events.Next ();
  }

  if (y1y2_changed) {
    self.Set (Am_Y1, y1);
    self.Set (Am_Y2, y2);
  }

  if (topheight_changed) {
    self.Set (Am_TOP, top);
    self.Set (Am_HEIGHT, height);
  }
}

Am_Define_Method (Am_Draw_Method, void, line_draw,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int x1 = (int)self.Get (Am_X1) + x_offset;
  int y1 = (int)self.Get (Am_Y1) + y_offset;
  int x2 = (int)self.Get (Am_X2) + x_offset;
  int y2 = (int)self.Get (Am_Y2) + y_offset;
  Am_Style ls = self.Get (Am_LINE_STYLE);
  drawonable->Draw_Line (ls, x1, y1, x2, y2);
}

Am_Define_Method (Am_Draw_Method, void, line_mask,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int x1 = (int)self.Get (Am_X1) + x_offset;
  int y1 = (int)self.Get (Am_Y1) + y_offset;
  int x2 = (int)self.Get (Am_X2) + x_offset;
  int y2 = (int)self.Get (Am_Y2) + y_offset;
  Am_Style ls = self.Get (Am_LINE_STYLE);
  drawonable->Draw_Line (ls, x1, y1, x2, y2, Am_DRAW_MASK_COPY);
}

void arrow_line_x_init (const Am_Slot& slot,
			Am_Web_Init& init)
{
  Am_Object_Advanced obj = slot.Get_Owner ();
  init.Note_Input (obj, Am_X1);
  init.Note_Input (obj, Am_X2);
  init.Note_Input (obj, Am_LEFT);
  init.Note_Input (obj, Am_WIDTH);
  init.Note_Input (obj, Am_LINE_STYLE);
  init.Note_Input (obj, Am_HEAD_LENGTH);
  init.Note_Input (obj, Am_HEAD_WIDTH);
  init.Note_Output (obj, Am_X2);
  init.Note_Output (obj, Am_LEFT);
  init.Note_Output (obj, Am_WIDTH);
}

void arrow_line_y_init (const Am_Slot& slot,
			Am_Web_Init& init)
{
  Am_Object_Advanced obj = slot.Get_Owner ();
  init.Note_Input (obj, Am_Y1);
  init.Note_Input (obj, Am_Y2);
  init.Note_Input (obj, Am_TOP);
  init.Note_Input (obj, Am_HEIGHT);
  init.Note_Input (obj, Am_LINE_STYLE);
  init.Note_Input (obj, Am_HEAD_LENGTH);
  init.Note_Input (obj, Am_HEAD_WIDTH);
  init.Note_Output (obj, Am_Y2);
  init.Note_Output (obj, Am_TOP);
  init.Note_Output (obj, Am_HEIGHT);
}

void arrow_line_x_validate (Am_Web_Events& events)
{
  bool x1x2_changed = false;
  bool leftwidth_changed = false;

  events.Start ();
  Am_Slot slot = events.Get ();
  Am_Object self = slot.Get_Owner ();
  int x1 = self.Get (Am_X1);
  int x2 = self.Get (Am_X2);
  int left = self.Get (Am_LEFT);
  int width = self.Get (Am_WIDTH);
  int head_length = self.Get (Am_HEAD_LENGTH);
  int head_width = self.Get (Am_HEAD_WIDTH);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  int lt_adjustment, rb_adjustment, wh_adjustment;
  get_thickness_adjustments (ls, lt_adjustment, rb_adjustment, wh_adjustment);
  if (head_length > head_width) {
    lt_adjustment += head_length;
    rb_adjustment += head_length;
    wh_adjustment += 2 * head_length;
  } else {
    lt_adjustment += head_width;
    rb_adjustment += head_width;
    wh_adjustment += 2 * head_width;
  }
  while (!events.Last ()) {
    slot = events.Get ();
    // cout << "__Line_x_Web " << self << " slot " << slot.Get_Key ()
    //  << " l,w,x1,x2 " << left << " " << width << " " << x1 << " " << x2
    //	 << endl << flush;
  switch (slot.Get_Key ()) {
    case Am_X1:
    case Am_X2:
      left = imin (x1, x2) - lt_adjustment;
      width  = iabs (x2 - x1) + wh_adjustment;
      leftwidth_changed = true;
      // cout << "__  left " << left << " width " << width << endl << flush;
      break;
    case Am_LEFT:
    case Am_WIDTH:
      if (x1 < x2) {
	x1 = left + lt_adjustment;
	x2 = left + width - rb_adjustment;
      }
      else {
	x1 = left + width - rb_adjustment;
	x2 = left + lt_adjustment;
      }
      x1x2_changed = true;
      // cout << "__  x1 " << x1 << " x2 " << x2 << endl << flush;
      break;
    case Am_LINE_STYLE:
    case Am_HEAD_WIDTH:
    case Am_HEAD_LENGTH:
      break;
    default:
      Am_ERROR ("** Bug: unexpected slot in arrow_line_x_validate: " << Am_Get_Slot_Name (slot.Get_Key()));
    }
    events.Next ();
  }
  
  if (x1x2_changed) {
    self.Set (Am_X1, x1);
    self.Set (Am_X2, x2);
  }

  if (leftwidth_changed) {
    self.Set (Am_LEFT, left);
    self.Set (Am_WIDTH, width);
  }
}

void arrow_line_y_validate (Am_Web_Events& events)
{
  bool y1y2_changed = false;
  bool topheight_changed = false;

  events.Start ();
  Am_Slot slot = events.Get ();
  Am_Object self = slot.Get_Owner ();
  int y1 = self.Get (Am_Y1);
  int y2 = self.Get (Am_Y2);
  int top = self.Get (Am_TOP);
  int height = self.Get (Am_HEIGHT);
  int head_length = self.Get (Am_HEAD_LENGTH);
  int head_width = self.Get (Am_HEAD_WIDTH);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  int lt_adjustment, wh_adjustment, rb_adjustment;
  get_thickness_adjustments (ls, lt_adjustment, rb_adjustment, wh_adjustment);
  if (head_length > head_width) {
    lt_adjustment += head_length;
    rb_adjustment += head_length;
    wh_adjustment += 2 * head_length;
  } else {
    lt_adjustment += head_width;
    rb_adjustment += head_width;
    wh_adjustment += 2 * head_width;
  }
  while (!events.Last ()) {
    slot = events.Get ();
    switch (slot.Get_Key ()) {
    case Am_Y1:
    case Am_Y2:
      top = imin (y1, y2) - lt_adjustment;
      height  = iabs (y2 - y1) + wh_adjustment;
      topheight_changed = true;
      break;
    case Am_TOP:
    case Am_HEIGHT:
      if (y1 < y2) {
	y1 = top + lt_adjustment;
	y2 = top + height - rb_adjustment;
      }
      else {
	y1 = top + height - rb_adjustment;
	y2 = top + lt_adjustment;
      }
      y1y2_changed = true;
      break;
    case Am_LINE_STYLE:
    case Am_HEAD_WIDTH:
    case Am_HEAD_LENGTH:
      break;
    default:
      Am_ERROR ("** Bug: unexpected slot in arrow_line_y_validate: " << Am_Get_Slot_Name (slot.Get_Key()));
    }
    events.Next ();
  }

  if (y1y2_changed) {
    self.Set (Am_Y1, y1);
    self.Set (Am_Y2, y2);
  }

  if (topheight_changed) {
    self.Set (Am_TOP, top);
    self.Set (Am_HEIGHT, height);
  }
}

#include <math.h>

Am_Define_Method (Am_Draw_Method, void, arrow_line_draw,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int x1 = (int)self.Get (Am_X1) + x_offset;
  int y1 = (int)self.Get (Am_Y1) + y_offset;
  int x2 = (int)self.Get (Am_X2) + x_offset;
  int y2 = (int)self.Get (Am_Y2) + y_offset;
  int head_width = self.Get (Am_HEAD_WIDTH);
  int head_length = self.Get (Am_HEAD_LENGTH);
  int dx = x2 - x1;
  int dy = y2 - y1;
  double d = sqrt ((double)(dx * dx + dy * dy));
  if (d < 1.0) {
    d = 1.0;
    dx = 1;
  }
  double lx = (head_length * dx) / d;
  double ly = (head_length * dy) / d;
  double wx = (head_width * dx) / d;
  double wy = (head_width * dy) / d;
  Am_Style ls = self.Get (Am_LINE_STYLE);
  drawonable->Draw_Line (ls, x1, y1, x2, y2);
  drawonable->Draw_Line (ls, (int)(x2 - lx + wy + 0.5),
			 (int)(y2 - ly - wx + 0.5), x2, y2);
  drawonable->Draw_Line (ls, (int)(x2 - lx - wy + 0.5),
			 (int)(y2 - ly + wx + 0.5), x2, y2);
}

Am_Define_Method (Am_Draw_Method, void, arrow_line_mask,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int x1 = (int)self.Get (Am_X1) + x_offset;
  int y1 = (int)self.Get (Am_Y1) + y_offset;
  int x2 = (int)self.Get (Am_X2) + x_offset;
  int y2 = (int)self.Get (Am_Y2) + y_offset;
  int head_width = self.Get (Am_HEAD_WIDTH);
  int head_length = self.Get (Am_HEAD_LENGTH);
  int dx = x2 - x1;
  int dy = y2 - y1;
  double d = sqrt ((double)(dx * dx + dy * dy));
  if (d < 1.0) {
    d = 1.0;
    dx = 1;
  }
  double lx = (head_length * dx) / d;
  double ly = (head_length * dy) / d;
  double wx = (head_width * dx) / d;
  double wy = (head_width * dy) / d;
  Am_Style ls = self.Get (Am_LINE_STYLE);
  drawonable->Draw_Line (ls, x1, y1, x2, y2, Am_DRAW_MASK_COPY);
  drawonable->Draw_Line (ls, (int)(x2 - lx + wy + 0.5),
			 (int)(y2 - ly - wx + 0.5), x2, y2, Am_DRAW_MASK_COPY);
  drawonable->Draw_Line (ls, (int)(x2 - lx - wy + 0.5),
			 (int)(y2 - ly + wx + 0.5), x2, y2, Am_DRAW_MASK_COPY);
}

/////////////////////////////////////////////////////////
// drawing other shapes: rectangles, roundtangles, arcs

Am_Define_Method (Am_Draw_Method, void, rectangle_draw,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  Am_Style fs = self.Get (Am_FILL_STYLE);
  drawonable->Draw_Rectangle (ls, fs, left, top, width, height);
}

Am_Define_Method (Am_Draw_Method, void, rectangle_mask,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  Am_Style fs = self.Get (Am_FILL_STYLE);
  drawonable->Draw_Rectangle (ls, fs, left, top, width, height,
			      Am_DRAW_MASK_COPY);
}

Am_Define_Method (Am_Draw_Method, void, arc_draw,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  int angle1 = self.Get(Am_ANGLE1);
  int angle2 = self.Get(Am_ANGLE2);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  Am_Style fs = self.Get (Am_FILL_STYLE);
  if ((width > 0) && (height > 0))
    drawonable->Draw_Arc (ls, fs, left, top, width, height, angle1, angle2);
}

Am_Define_Method (Am_Draw_Method, void, arc_mask,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  int angle1 = self.Get(Am_ANGLE1);
  int angle2 = self.Get(Am_ANGLE2);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  Am_Style fs = self.Get (Am_FILL_STYLE);
  if ((width > 0) && (height > 0))
    drawonable->Draw_Arc (ls, fs, left, top, width, height, angle1, angle2,
			  Am_DRAW_MASK_COPY);
}

Am_Define_Formula (int, compute_draw_radius) {
  int radius = self.Get (Am_RADIUS);
  int smaller_side = imin (self.Get(Am_WIDTH), self.Get(Am_HEIGHT));
  if (radius >= 0)
    return imin (radius, (int)(smaller_side / 2));
  else {
    radius = (Am_Radius_Flag) radius;
    switch (radius) {
    case Am_SMALL_RADIUS:  return (int)(smaller_side / 5);
    case Am_MEDIUM_RADIUS: return (int)(smaller_side / 4);
    case Am_LARGE_RADIUS:  return (int)(smaller_side / 3);
    default:               return 0;
    }
  }
}
      

Am_Define_Method(Am_Draw_Method, void, roundtangle_draw,
		 (Am_Object self, Am_Drawonable* drawonable,
		  int x_offset, int y_offset)) {
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  int radius = self.Get(Am_DRAW_RADIUS);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  Am_Style fs = self.Get (Am_FILL_STYLE);
  drawonable->Draw_Roundtangle (ls, fs, left, top, width, height,
		                radius, radius);
}

Am_Define_Method(Am_Draw_Method, void, roundtangle_mask,
		 (Am_Object self, Am_Drawonable* drawonable,
		  int x_offset, int y_offset)) {
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  int radius = self.Get(Am_DRAW_RADIUS);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  Am_Style fs = self.Get (Am_FILL_STYLE);
  drawonable->Draw_Roundtangle (ls, fs, left, top, width, height,
		                radius, radius, Am_DRAW_MASK_COPY);
}

//////////////////////////////////////////////////////////
///  functions to support drawing polygons

Am_Define_Method(Am_Draw_Method, void, polygon_draw,
		 (Am_Object self, Am_Drawonable* drawonable,
		  int x_offset, int y_offset)) {
  Am_Style ls = self.Get (Am_LINE_STYLE);
  Am_Style fs = self.Get (Am_FILL_STYLE);

  Am_Point_List pl = self.Get (Am_POINT_LIST);
  Am_Point_Array pts (pl, x_offset, y_offset);
  /// NDY: cache pts in a slot for faster drawing

  int left = self.Get (Am_LEFT);
  int top = self.Get (Am_TOP);
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);

  drawonable->Push_Clip (left + x_offset, top + y_offset, width, height);
  drawonable->Draw_Lines (ls, fs, pts);
  drawonable->Pop_Clip();
}

Am_Define_Method(Am_Draw_Method, void, polygon_mask,
		 (Am_Object self, Am_Drawonable* drawonable,
		  int x_offset, int y_offset)) {
  Am_Style ls = self.Get (Am_LINE_STYLE);
  Am_Style fs = self.Get (Am_FILL_STYLE);

  Am_Point_List pl = self.Get (Am_POINT_LIST);
  Am_Point_Array pts (pl, x_offset, y_offset);
  /// NDY: cache pts in a slot for faster drawing

  int left = self.Get (Am_LEFT);
  int top = self.Get (Am_TOP);
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);

  drawonable->Push_Clip (left + x_offset, top + y_offset, width, height);
  drawonable->Draw_Lines (ls, fs, pts, Am_DRAW_MASK_COPY);
  drawonable->Pop_Clip();
}

/// Returns non-zero if the line segment with endpoints <x1,y1> and <x2,y2>
/// crosses the ray pointing from <x,y> to <infinity,y>.
/// Returns 1 if the line segment crosses the ray going up 
/// (from low y to higher y), or -1 if it crosses the ray going down.

static int crosses_to_right_of (int x, int y, int x1, int y1, int x2, int y2)
{
  if ((y1 < y && y < y2) &&
      (x2-x)*(y1-y) < (x1-x)*(y2-y))
    return 1;
  else if ((y2 < y && y < y1) &&
	   (x1-x)*(y2-y) < (x2-x)*(y1-y))
    return -1;
  else
    return 0;
}

//  The coordinate system of x and y is defined w.r.t. ref_obj
Am_Define_Method(Am_Point_In_Method, Am_Object, polygon_point_in_obj,
		 (const Am_Object& in_obj, int x, int y,
		  const Am_Object& ref_obj))
{
  if ((bool)in_obj.Get (Am_VISIBLE)) {
    am_translate_coord_to_me(in_obj, ref_obj, x, y);
    // now x,y is so that 0,0 is the upper left corner of this line (left,top)
    int left = in_obj.Get (Am_LEFT);
    int top = in_obj.Get (Am_TOP);
    int width = in_obj.Get (Am_WIDTH);
    int height = in_obj.Get (Am_HEIGHT);

    int hit_threshold = in_obj.Get (Am_HIT_THRESHOLD);
    int thickness = get_line_thickness (in_obj);
    int threshold = imax (hit_threshold, (thickness+1)/2);
    // *** HACK; why is this necessary??  Otherwise thickness 1 and 2 lines
    // *** are not selectable 
    if (threshold == 1) threshold = 2;

    // do simple bounding box test first
    if (x < -threshold || x > width+threshold ||
	y < -threshold || y > height+threshold) {
      return Am_No_Object;
    }

    Am_Point_List pl = in_obj.Get (Am_POINT_LIST);
    int xfirst, yfirst;
    int x1, y1, x2, y2;
    int crossings = 0;
    bool last;

    pl.Start ();
    if (pl.Last()) return Am_No_Object; // no points!

    pl.Get(xfirst, yfirst);
    xfirst -= left;  yfirst -= top;
    x2 = xfirst;  y2 = yfirst;

    do {
      x1 = x2;  y1 = y2;

      pl.Next(); 
      last = pl.Last();
      if (last) {
	if ((bool) in_obj.Get (Am_SELECT_OUTLINE_ONLY)) {
	  // do not close an open polygon
	  // never came close to an edge, so fail
	  return Am_No_Object;
	}
	x2 = xfirst; y2 = yfirst; 
      }
      else { 
	pl.Get(x2, y2);
	x2 -= left;  y2 -= top; 
      }
      
      if (point_in_line_segment (x, y, x1, y1, x2, y2, threshold))
	// point is near an edge -- definitely a hit
	return in_obj;
      
      crossings += crosses_to_right_of (x, y, x1, y1, x2, y2);

    } while (!last);

    if ((bool)in_obj.Get (Am_SELECT_FULL_INTERIOR)) {
      // any unpaired crossings ==> inside outer edge of polygon
      if (crossings) return in_obj;
      else return Am_No_Object;
    } else {
      // odd number of crossings ==> inside filled space of polygon
      if (crossings & 1) return in_obj;
      else return Am_No_Object;
    }
  }
  else return Am_No_Object;
}

//web functions
static bool polygon_web_create (const Am_Slot& slot)
{
  return slot.Get_Key () == Am_LEFT;
}

static void polygon_web_init (const Am_Slot& slot,
			      Am_Web_Init& init)
{
  Am_Object_Advanced obj = slot.Get_Owner ();
  init.Note_Input (obj, Am_POINT_LIST);
  init.Note_Input (obj, Am_LEFT);
  init.Note_Input (obj, Am_TOP);
  init.Note_Input (obj, Am_WIDTH);
  init.Note_Input (obj, Am_HEIGHT);
  init.Note_Input (obj, Am_LINE_STYLE);
  init.Note_Output (obj, Am_POINT_LIST);
  init.Note_Output (obj, Am_TOP);
  init.Note_Output (obj, Am_WIDTH);
  init.Note_Output (obj, Am_HEIGHT);
}

static inline float scale_factor (int new_dim, int old_dim)
{  
  // if original dimension or new dimension is 1 (implying zero delta
  // between edges), use a delta of .25 instead of 0, to preserve 
  // the actual separation between points in point-list
  return 
    (float) ((new_dim > 1 ? new_dim-1 : 0.25f) 
    / (old_dim > 1 ? old_dim-1 : 0.25f));
}

/// general_polygon_validate() makes every effort to scale & translate
/// pl such that its bounding box is (left,top,width,height).
/// If it cannot (for instance, because all points in pl have same
/// x coordinate but width > 1), then scales pl as close as possible and
/// sets width and height to pl's actual bounding box.  
/// (left and top are never changed.)

static void general_polygon_validate (Am_Point_List &pl,
			    int &left, int &top,
			    int &width, int &height,
			    Am_Style ls, Am_Drawonable *draw)
{
  int pl_left, pl_top, pl_width, pl_height;
  draw->Get_Polygon_Bounding_Box(pl, ls, pl_left, pl_top, 
				 pl_width, pl_height);

  if (left == pl_left && top == pl_top &&
      width == pl_width && height == pl_height)
    return;  // no work to do!
    

#ifdef TEST_POLYGON_CONSTRAINT
  cout << "**** Searching for polygon scaling factors" << endl;
#endif

  // Try a binary search for the right scaling factors.

  // initialize induction variables for loop:
  float xfactor = scale_factor (width, pl_width);
  float xscale_low = 0.5f * xfactor;
  float xscale_high = 2.0f * xfactor;
  float xscale = (xscale_low + xscale_high)/2;

  float yfactor = scale_factor (height, pl_height);
  float yscale_low = 0.5f * yfactor;
  float yscale_high = 2.0f * yfactor;
  float yscale = (yscale_low + yscale_high)/2;

  Am_Point_List new_pl = pl;
  new_pl.Scale (xscale, yscale, pl_left, pl_top);

  int curr_left, curr_top, curr_width, curr_height;
  draw->Get_Polygon_Bounding_Box(new_pl, ls, curr_left, curr_top,
				 curr_width, curr_height);

  int n = 16;         // limits depth of binary search

  // now begin loop
  while ((curr_width != width || curr_height != height) && n > 0) {
    // update induction variables
    if (curr_width < width)         xscale_low = xscale;
    else if (curr_width > width)    xscale_high = xscale;
    xscale = (xscale_low + xscale_high)/2;

    if (curr_height < height)         yscale_low = yscale;
    else if (curr_height > height)    yscale_high = yscale;
    yscale = (yscale_low + yscale_high)/2;

    new_pl = pl;
    new_pl.Scale (xscale, yscale, pl_left, pl_top);
    draw->Get_Polygon_Bounding_Box(new_pl, ls, curr_left, curr_top,
				   curr_width, curr_height);

    --n;
  }

  // curr_width and curr_height are now as close as they're gonna get.
  // translate to (left,top)
  new_pl.Translate (left - curr_left, top - curr_top);

  // return the final points list
  pl = new_pl;
}

static void polygon_web_validate (
				  Am_Web_Events& events)
{
  events.End ();
  Am_Slot slot = events.Get ();
  Am_Object self = slot.Get_Owner ();
  Am_Drawonable* draw = Get_a_drawonable (self);
  Am_Point_List pl = self.Get (Am_POINT_LIST);
  Am_Style ls = self.Get (Am_LINE_STYLE);
  int left = self.Get (Am_LEFT);
  int top  = self.Get (Am_TOP);
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);

  bool rescaled = false;
  bool pl_changed = false;
  bool bbox_changed = false;

  // compute ORIGINAL values of left/top/width/height by working
  // backwards through the change events
  events.End ();
  while (!events.First ()) {
    slot = events.Get ();

    Am_Slot_Key key = slot.Get_Key ();
    if (key == Am_POINT_LIST) {
      // these changes supersedes all earlier changes, so find its
      // bounding box and stop looking backwards
      if (pl.Valid() && draw)
	draw->Get_Polygon_Bounding_Box(pl, ls, left, top, width, height);
      bbox_changed = true;
      break;
    }
    else 
      switch (key) {
      case Am_WINDOW:
      case Am_DRAWONABLE:
	break;
      case Am_LINE_STYLE:
	ls = events.Get_Prev_Value ();
	break;
      case Am_LEFT:
	left = events.Get_Prev_Value ();
	break;
      case Am_TOP:
	top = events.Get_Prev_Value ();
	break;
      case Am_WIDTH:
	width = events.Get_Prev_Value ();
	break;
      case Am_HEIGHT:
	height = events.Get_Prev_Value ();
	break;

      default:
	Am_ERROR ("** Bug: unexpected slot in polygon_web_validate: " << Am_Get_Slot_Name (key));
      }

    events.Prev ();
  }

  // now go forwards, looking at the CHANGES made to each slot
  events.Next ();
  while (!events.Last ()) {
    slot = events.Get ();
    switch (slot.Get_Key ()) {
#if 0
    case Am_POINT_LIST:
      pl = self.Get (Am_POINT_LIST);
      if (pl.Valid() && draw)
	draw->Get_Polygon_Bounding_Box(pl, ls, left, top, width, height);
      bbox_changed = true;
      break;
#endif

    case Am_WINDOW:
    case Am_DRAWONABLE:
      if (pl.Valid() && draw)
	draw->Get_Polygon_Bounding_Box(pl, ls, left, top, width, height);
      bbox_changed = true;
      break;

    case Am_LINE_STYLE:
      // recompute bounding box with new line style
      ls = self.Get (Am_LINE_STYLE, Am_NO_DEPENDENCY);
      if (pl.Valid() && draw)
	draw->Get_Polygon_Bounding_Box(pl, ls, left, top, width, height);
      bbox_changed = true;
      break;

    case Am_LEFT:
      {
	int new_left = self.Get (Am_LEFT, Am_NO_DEPENDENCY);
	pl.Translate (new_left - left, 0);
	left = new_left;
	pl_changed = true;
	break;
      }

    case Am_TOP:
      {
	int new_top = self.Get (Am_TOP, Am_NO_DEPENDENCY);
	pl.Translate (0, new_top - top);
	top = new_top;
	pl_changed = true;
	break;
      }

    case Am_WIDTH:
      {
	int new_width = self.Get (Am_WIDTH, Am_NO_DEPENDENCY);
	pl.Scale (scale_factor (new_width, width), 1.0f, left, top);
	width = new_width;
	rescaled = true;
	pl_changed = true;
	break;
      }

    case Am_HEIGHT:
      {
	int new_height = self.Get (Am_HEIGHT, Am_NO_DEPENDENCY);
	pl.Scale (1.0f, scale_factor (new_height, height), left, top);
	height = new_height;
	rescaled = true;
	pl_changed = true;
	break;
      }
    }
    events.Next ();
  }

  if (rescaled && pl.Valid() && draw)
    // check that scaling produced a polygon with the correct bounding box.
    // how could this fail?  several possibilites: line thickness
    // doesn't scale;  and mitered joins may grow or shrink 
    // orthogonally to the scaling.

    general_polygon_validate (pl, left, top, width, height, ls, draw);

  
#ifdef TEST_POLYGON_CONSTRAINT
  // debugging check that bounding box is correct
  if (pl.Valid() && draw) {
    int curr_left, curr_top, curr_width, curr_height;
    draw->Get_Polygon_Bounding_Box(pl, ls, curr_left, curr_top, 
				   curr_width, curr_height);
    if (!(left == curr_left &&
	  top == curr_top &&
	  width == curr_width &&
	  height == curr_height))
      cout << "** polygon_web_validate: bounding box ("
	<< left << "," << top << "," << width << "," << height << ")"
	  << " inconsistent with point list ("
	    << curr_left << "," << curr_top << "," << curr_width << "," << 
	      curr_height << ")" << endl;
  }
#endif

  if (pl_changed)
    self.Set (Am_POINT_LIST, pl);

  if (bbox_changed) {
    self.Set (Am_LEFT, left);
    self.Set (Am_TOP, top);
    self.Set (Am_WIDTH, width);
    self.Set (Am_HEIGHT, height);
  }
}

#if 0  // obsolete
Am_Define_Formula (int, compute_polygon_left)
{
  Am_Object window, owner;
  Am_Drawonable* draw;
  Am_Point_List pl;
  pl = self.GV (Am_POINT_LIST);
  if (pl && (window = self.GV (Am_WINDOW))
      && (draw = Am_Drawonable::Narrow (window.GV (Am_DRAWONABLE)))
      && (owner = self.GV_Owner ()))
    {
      Am_Style ls = self.GV (Am_LINE_STYLE);
      int n, left;
      
      draw->Get_Polygon_Bounding_Box(pl, ls, left, n, n, n);
      return left;
    }
  else return 0;
}

Am_Define_Formula (int, compute_polygon_top)
{
  Am_Object window, owner;
  Am_Drawonable* draw;
  Am_Point_List pl;
  pl = self.GV (Am_POINT_LIST);
  if (pl && (window = self.GV (Am_WINDOW))
      && (draw = Am_Drawonable::Narrow (window.GV (Am_DRAWONABLE)))
      && (owner = self.GV_Owner ()))
    {
      Am_Style ls = self.GV (Am_LINE_STYLE);
      int n, top;
      
      draw->Get_Polygon_Bounding_Box(pl, ls, n, top, n, n);
      return top;
    }
  else return 0;
}

Am_Define_Formula (int, compute_polygon_width)
{
  Am_Object window, owner;
  Am_Drawonable* draw;
  Am_Point_List pl;
  pl = self.GV (Am_POINT_LIST);
  if (pl && (window = self.GV (Am_WINDOW))
      && (draw = Am_Drawonable::Narrow (window.GV (Am_DRAWONABLE)))
      && (owner = self.GV_Owner ()))
    {
      Am_Style ls = self.GV (Am_LINE_STYLE);
      int n, width;
      
      draw->Get_Polygon_Bounding_Box(pl, ls, n, n, width, n);
      return width;
    }
  else return 0;
}

Am_Define_Formula (int, compute_polygon_height)
{
  Am_Object window, owner;
  Am_Drawonable* draw;
  Am_Point_List pl;
  pl = self.GV (Am_POINT_LIST);
  if (pl && (window = self.GV (Am_WINDOW))
      && (draw = Am_Drawonable::Narrow (window.GV (Am_DRAWONABLE)))
      && (owner = self.GV_Owner ()))
    {
      Am_Style ls = self.GV (Am_LINE_STYLE);
      int n, height;
      
      draw->Get_Polygon_Bounding_Box(pl, ls, n, n, n, height);
      return height;
    }
  else return 0;
}
#endif

/***************************************
 ** Functions to support drawing text **
 ***************************************/

Am_Define_Formula (int, compute_string_width)
{
  Am_Font font (self.Get (Am_FONT));
  Am_String string (self.Get (Am_TEXT));

  if (string.Valid ()) {
    Am_Object screen = Am_GV_Screen (self);
    int str_len = strlen (string);
    int width = Am_Get_String_Width (screen, font, string, str_len);

    // assure text boxes are always at least 10 pixels wide so you can click
    return width > 10 ? width : 10;
  }
  else
    return 10;
}

Am_Define_Formula (int, compute_string_height)
{
  Am_Font font (self.Get (Am_FONT));
  Am_Object screen = Am_GV_Screen (self);

  int max_width, min_width, ascent, descent;
  Am_Get_Font_Properties (screen, font, max_width, min_width,
			  ascent, descent);
  return (ascent + descent);
}

// get the x position for the cursor in pixels
Am_Define_Formula (int, compute_cursor_offset) {
  int cursor_index = self.Get (Am_CURSOR_INDEX);
  if (cursor_index >= 0) {
    Am_Drawonable *drawonable = GV_a_drawonable (self);
    Am_Font font(self.Get (Am_FONT));
    Am_String str(self.Get (Am_TEXT));
    int max_cursor_index = strlen (str);
    cursor_index = imin (imax (0, cursor_index), max_cursor_index);
    int new_offset = drawonable->Get_String_Width (font, str, cursor_index);
    if (cursor_index > 0 &&
	cursor_index == max_cursor_index) new_offset--; //make sure < width
    return new_offset;
  }
  else return 0;
}
// get the x position for the cursor in pixels, for hidden text that
// uses the same character for all characters
Am_Define_Formula (int, compute_cursor_offset_for_hidden) {
  int cursor_index = self.Get (Am_CURSOR_INDEX);
  if (cursor_index >= 0) {
    Am_Drawonable *drawonable = GV_a_drawonable (self);
    Am_Font font(self.Get (Am_FONT));
    Am_String str(self.Get (Am_TEXT));
    char replacement = self.Get(Am_START_CHAR);
    int max_cursor_index = strlen (str);
    cursor_index = imin (imax (0, cursor_index), max_cursor_index);
    int char_width = drawonable->Get_Char_Width (font, replacement);
    int new_offset = cursor_index * (char_width + 1);
    if (cursor_index > 0 &&
	cursor_index == max_cursor_index) new_offset--; //make sure < width
    return new_offset;
  }
  else return 0;
}

//make sure the cursor is visible inside the object by scrolling string l or r
Am_Define_Formula (int, compute_string_offset) {
  Am_String str;
  str = self.Get(Am_TEXT);
  if (!str.Valid()) return 0; // no string in there, so offset 0.

  int cursor_index = self.Get (Am_CURSOR_INDEX);
  // get old value of this slot
  int old_string_offset;
  Am_Value old_string_offset_value;
  old_string_offset_value=self.Peek(Am_X_OFFSET, Am_NO_DEPENDENCY);
  if(!old_string_offset_value.Exists())
    old_string_offset=0;
  else
    old_string_offset = old_string_offset_value;

  int new_offset = old_string_offset;
  int width = self.Get(Am_WIDTH);
  Am_Drawonable *drawonable = GV_a_drawonable (self);
  Am_Font font(self.Get (Am_FONT));
  
  int max_cursor_index = strlen (str);
  int full_string_width =
    drawonable->Get_String_Width (font, str, max_cursor_index);
  if (old_string_offset > 0 && 
      full_string_width - old_string_offset < width) {
    //then need to scroll because width changed
    new_offset = imax(0, full_string_width - width);
  }
  else if (cursor_index >= 0) { //otherwise, don't change position
    Am_Drawonable *drawonable = GV_a_drawonable (self);
    int cursor_offset = self.Get(Am_CURSOR_OFFSET);
    if (cursor_offset < old_string_offset) { // then scroll right
      if (cursor_offset == 0) new_offset = 0;
      else {
	// find the position of the character at the cursor
	cursor_index = imin (imax (0, cursor_index), max_cursor_index);
	new_offset = drawonable->Get_String_Width (font, str, cursor_index);
      }
    }
    else {
      if (cursor_offset > old_string_offset + width) { // scroll left
	// find the position of the character at the cursor
	cursor_index = imin (imax (0, cursor_index), max_cursor_index);
	new_offset = drawonable->Get_String_Width (font, str, cursor_index);
	new_offset = new_offset - width;
	if (new_offset < 0) new_offset = 0;
      }
    }
  }
  return new_offset;
}
      
static void text_draw_internal(Am_Object self, Am_Drawonable* drawonable,
			       int x_offset, int y_offset, const char *str,
			       bool mask = false)
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  int my_x_offset = 0;
  Am_Value v;
  v = self.Peek(Am_X_OFFSET);
  if (v.Valid()) my_x_offset = v; //might be invalid if no string
  
  Am_Style ls = self.Get (Am_LINE_STYLE);
  Am_Style fs = self.Get (Am_FILL_STYLE);
  Am_Font font = self.Get (Am_FONT);  
  int str_len = strlen (str);
  bool invert = self.Get (Am_INVERT);
  
  // set a clip region in case string is bigger than text box
  drawonable->Push_Clip (left, top, width, height);
  drawonable->Draw_Text (ls, str, str_len, font, left - my_x_offset, top,
			 mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY, fs, invert);
    
  if (((int)(self.Get (Am_CURSOR_INDEX)) >= 0)) {
    int cursor_offset = self.Get(Am_CURSOR_OFFSET);
    int cursor_left = left + cursor_offset - my_x_offset;
    // cursor_height can't just be Am_HEIGHT in case user resized box.
    int ascent, descent, garbage;
    drawonable->Get_Font_Properties (font, garbage, garbage, ascent, descent);
    int cursor_bottom = ascent + descent + top;
    drawonable->Draw_Line (ls, cursor_left, top, cursor_left,
	                   cursor_bottom, mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY);
  }
  drawonable->Pop_Clip(); 
}

Am_Define_Method (Am_Draw_Method, void, text_draw,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset)) {
  Am_Value v;
  Am_String str;
  v=self.Peek(Am_TEXT);
  if (!v.Valid()) return;  //if not a valid string, don't draw anything
  str = v;
  text_draw_internal(self, drawonable, x_offset, y_offset, str);
}

Am_Define_Method (Am_Draw_Method, void, text_mask,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  Am_Value v;
  Am_String str;
  v = self.Peek (Am_TEXT);
  if (!v.Valid ()) return;  //if not a valid string, don't draw anything
  str = v;
  text_draw_internal (self, drawonable, x_offset, y_offset, str, true);
}

Am_Define_Method (Am_Draw_Method, void, hidden_text_draw,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset)) {
  Am_Value v;
  Am_String str;
  v=self.Peek(Am_TEXT);
  if (!v.Valid()) return;  //if not a valid string, don't draw anything
  str = v;
  int str_len = strlen (str);
  char new_str[250];
  if (str_len > 249) str_len = 249;
  char replacement = self.Get(Am_START_CHAR);
  for (int i=0; i<str_len; i++) {
    new_str[i] = replacement;
  }
  new_str[str_len] = 0;
  text_draw_internal(self, drawonable, x_offset, y_offset, new_str);
}

/******************************************
 ** Functions to support drawing bitmaps **
 ******************************************/

Am_Define_Formula (int, compute_bitmap_width) {
  Am_Image_Array image;
  image = self.Get (Am_IMAGE);
  if (image.Valid ()) {
    int ret_width, ret_height;
    Am_Drawonable *drawonable = GV_a_drawonable (self);
    drawonable->Get_Image_Size (image, ret_width, ret_height);
    return ret_width;
  }
  // no image, so return 0
  return 0;
}

Am_Define_Formula (int, compute_bitmap_height) {
  Am_Image_Array image;
  image = self.Get (Am_IMAGE);
  if (image.Valid ()) {
    int ret_width, ret_height;
    Am_Object window, owner;
    Am_Drawonable *drawonable = GV_a_drawonable (self);
    drawonable->Get_Image_Size (image, ret_width, ret_height);
    return ret_height;
  }
  // no image, so return 0
  return 0;
}

Am_Define_Method (Am_Draw_Method, void, bitmap_draw,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  Am_Image_Array image = self.Get (Am_IMAGE);
  if (image.Valid ()) {
    int left = (int)self.Get (Am_LEFT) + x_offset;
    int top = (int)self.Get (Am_TOP) + y_offset;
    Am_Style ls = self.Get (Am_LINE_STYLE);
    Am_Style fs = self.Get (Am_FILL_STYLE);
    bool monochrome = self.Get (Am_DRAW_MONOCHROME);
    drawonable->Draw_Image (left, top, -1, -1, image, 0, 0, ls, fs, monochrome);
  }
}

Am_Define_Method (Am_Draw_Method, void, bitmap_mask,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  Am_Image_Array image = self.Get (Am_IMAGE);
  if (image.Valid()) {
    int left = (int)self.Get (Am_LEFT) + x_offset;
    int top = (int)self.Get (Am_TOP) + y_offset;
    Am_Style ls = self.Get (Am_LINE_STYLE);
    Am_Style fs = self.Get (Am_FILL_STYLE);
    bool monochrome = self.Get (Am_DRAW_MONOCHROME);
    drawonable->Draw_Image (left, top, -1, -1, image, 0, 0, ls, fs,
			    monochrome, Am_DRAW_MASK_COPY);
  }
}

//  The coordinate system of x and y is defined w.r.t. ref_obj
Am_Define_Method(Am_Point_In_Method, Am_Object, line_point_in_obj,
		 (const Am_Object& in_obj, int x, int y,
		  const Am_Object& ref_obj)) {
  if ((bool)in_obj.Get (Am_VISIBLE)) {
    am_translate_coord_to_me(in_obj, ref_obj, x, y);
    // now x,y is so that 0,0 is the upper left corner of this line (left,top)
      int left = in_obj.Get (Am_LEFT);
    int top = in_obj.Get (Am_TOP);
    int x1 = (int)in_obj.Get (Am_X1) - left;
    int y1 = (int)in_obj.Get (Am_Y1) - top;
    int x2 = (int)in_obj.Get (Am_X2) - left;
    int y2 = (int)in_obj.Get (Am_Y2) - top;

    int hit_threshold = in_obj.Get (Am_HIT_THRESHOLD);
    int thickness = get_line_thickness (in_obj);
    int threshold = imax (hit_threshold, (thickness+1)/2);
    // *** HACK; why is this necessary??  Otherwise thickness 1 and 2 lines
    // *** are not selectable 
    if (threshold == 1) threshold = 2;

    // do simple bounding box test first
    if (x < imin (x1, x2) - threshold || x > imax (x1, x2) + threshold ||
	y < imin (y1, y2) - threshold || y > imax (y1, y2) + threshold) {
      return Am_No_Object;
    }

    // The old line hit formula.  Seems to work okay.
    
    //     // equation for line is ax + by + c = 0
    //     // d/sqrt(a^2+b^2) is the distance between line and point <x,y>
    //     long a = y1 - y2; //long type needed for Windows
    //     long b = x2 - x1;
    //     float c = long(x1)*y2 - long(x2)*y1;
    //     float d = ((float)a)*x + ((float)b)*y + c;
    // 
    //     if (d*d <= long(threshold) * threshold * (a*a + b*b) ) {
    //       return in_obj;
    //     }
    //     else {
    //       return Am_No_Object;
    //     }
    
    // the new line hit formula.  Also seems to work, based on
    // some Maple calculations.  More expensive.
    float numer = (float)(x * y1 - x * y2 + y * x2 - y * x1 + y2 * x1 - y1 * x2);
    float denom = (float)(y1 * y1 - 2 * y1 * y2 + y2 * y2
      + x2 * x2 - 2 * x2 * x1 + x1 * x1);
    float distance2 = numer * numer / denom;
    
    if (distance2 <= float(threshold) * float(threshold))
      return in_obj;
  }

  /* else... */
  return Am_No_Object;
}

/******************************************
 ** Standard exported constraints        **
 ******************************************/

// Put in an Am_HEIGHT slot. Sets the height to the height of the object's 
// owner, minus this object's Am_TOP, minus Am_BOTTOM_OFFSET
Am_Define_Formula (int, Am_Fill_To_Bottom)
{
  Am_Object owner = self.Get_Owner ();
  if (!owner.Valid()) return 0;
  return (int)(owner.Get(Am_HEIGHT)) - (int)(self.Get(Am_TOP)) 
    - (int)(owner.Get(Am_BOTTOM_OFFSET));
}

// Put in an Am_WIDTH slot: see fill_to_bottom
Am_Define_Formula (int, Am_Fill_To_Right)
{
  Am_Object owner = self.Get_Owner ();
  if (!owner.Valid()) return 0;
  return (int)(owner.Get(Am_WIDTH)) - (int)(self.Get(Am_LEFT)) 
    - (int)(owner.Get(Am_RIGHT_OFFSET));
}

Am_Define_Formula (int, Am_Fill_To_Rest_Of_Width)
{
  Am_Object owner = self.Get_Owner ();
  Am_Value_List parts = owner.Get (Am_GRAPHICAL_PARTS);
  int width = 0;
  Am_Object part;
  for (parts.Start (); !parts.Last (); parts.Next ()) {
    part = parts.Get ();
    if (part != self)
      width += (int)part.Get (Am_WIDTH);
  }
  return (int)owner.Get (Am_WIDTH) - (width + (int)owner.Get (Am_LEFT_OFFSET) +
      (int)owner.Get (Am_RIGHT_OFFSET) +
      (int)owner.Get (Am_H_SPACING) * (parts.Length () - 1));
}

Am_Define_Formula (int, Am_Fill_To_Rest_Of_Height)
{
  Am_Object owner = self.Get_Owner ();
  Am_Value_List parts = owner.Get (Am_GRAPHICAL_PARTS);
  int height = 0;
  Am_Object part;
  for (parts.Start (); !parts.Last (); parts.Next ()) {
    part = parts.Get ();
    if (part != self)
      height += (int)part.Get (Am_HEIGHT);
  }
  return (int)owner.Get (Am_HEIGHT) - (height + (int)owner.Get (Am_TOP_OFFSET) +
      (int)owner.Get (Am_BOTTOM_OFFSET) +
      (int)owner.Get (Am_V_SPACING) * (parts.Length () - 1));
}

Am_Define_Formula(int, Am_Width_Of_Parts) {
  int max_x = 0, comp_right;
  Am_Value_List components;
  Am_Object comp;
  components = self.Get(Am_GRAPHICAL_PARTS);
  for (components.Start(); !components.Last(); components.Next()) {
    comp = components.Get ();
    if (comp.Get(Am_VISIBLE).Valid()) {
      // compute how much of the component extends right of the origin
      comp_right = ((int)(comp.Get(Am_LEFT)) + (int)(comp.Get(Am_WIDTH)));
      max_x = imax (max_x, comp_right);
    }
  }
  if (self.Peek (Am_RIGHT_OFFSET, Am_NO_DEPENDENCY).Exists())
    max_x += (int)self.Get (Am_RIGHT_OFFSET);
  return max_x; // always >=0 since it's initialized to 0
}

Am_Define_Formula(int, Am_Height_Of_Parts) {
  int max_y = 0, comp_bottom;
  Am_Value_List components;
  Am_Object comp;
  components = self.Get(Am_GRAPHICAL_PARTS);
  for (components.Start(); !components.Last(); components.Next()) {
    comp = components.Get ();
    if (comp.Get(Am_VISIBLE).Valid()) {
      // compute how much of the component extends below the origin
      comp_bottom = ((int)(comp.Get(Am_TOP)) + (int)(comp.Get(Am_HEIGHT)));
      max_y = imax (max_y, comp_bottom);
    }
  }
  if (self.Peek (Am_BOTTOM_OFFSET, Am_NO_DEPENDENCY).Exists())
    max_y += (int)self.Get (Am_BOTTOM_OFFSET);
  return max_y; // always >=0 since it's initialized to 0
}

Am_Define_Formula(int, Am_Center_X_Is_Center_Of) {
  int my_width = self.Get (Am_WIDTH);
  Am_Object center_x_obj;
  center_x_obj = self.Get(Am_CENTER_X_OBJ);
  if (center_x_obj.Valid()) {
    int center_x_obj_left = center_x_obj.Get (Am_LEFT);
    int center_x_obj_width = center_x_obj.Get (Am_WIDTH);
    return center_x_obj_left + ((center_x_obj_width - my_width) / 2);
  }
  else {
    return 0;
  }
}

Am_Define_Formula(int, Am_Center_Y_Is_Center_Of) {
  int my_height = self.Get (Am_HEIGHT);
  Am_Object center_y_obj;
  center_y_obj = self.Get(Am_CENTER_Y_OBJ);
  if (center_y_obj.Valid()) {
    int center_y_obj_top = center_y_obj.Get (Am_TOP);
    int center_y_obj_height = center_y_obj.Get (Am_HEIGHT);
    return center_y_obj_top + ((center_y_obj_height - my_height) / 2);
  }
  else {
    return 0;
  }
}

Am_Define_Formula(int, Am_Center_X_Is_Center_Of_Owner) {
  int my_width = self.Get (Am_WIDTH);
  Am_Object center_x_obj = self.Get_Owner();
  if (center_x_obj.Valid()) {
    int center_x_obj_width = center_x_obj.Get (Am_WIDTH);
    return (center_x_obj_width - my_width) / 2;
  }
  else {
    return 0;
  }
}

Am_Define_Formula(int, Am_Center_Y_Is_Center_Of_Owner) {
  int my_height = self.Get (Am_HEIGHT);
  Am_Object center_y_obj = self.Get_Owner();
  if (center_y_obj.Valid()) {
    int center_y_obj_height = center_y_obj.Get (Am_HEIGHT);
    return (center_y_obj_height - my_height) / 2;
  }
  else {
    return 0;
  }
}

void get_max_sizes (Am_Value_List& components, 
		    int& max_width, int& max_height)
{
  max_width = 0; max_height = 0;
  Am_Object item;
  for (components.Start (); !components.Last (); components.Next ()) {
    item = components.Get ();
    if ((bool)item.Get (Am_VISIBLE)) {
      int width = item.Get (Am_WIDTH);
      int height = item.Get (Am_HEIGHT);
      if (width > max_width)
	max_width = width;
      if (height > max_height)
	max_height = height;
    }
  }
}

void get_fixed_sizes (Am_Object self, Am_Value_List& components,
		      int& fixed_width, int& fixed_height)
{
  Am_Value value;
  value = self.Get(Am_FIXED_WIDTH);
  // the case where CC gets a bool as an int is taken care of automatically:
  // Am_MAX_FIXED_SIZE == 1 == (int)true; Am_NOT_FIXED_SIZE == 0 == (int)false
  if (value.type == Am_INT)
    fixed_width = value;
  else
    fixed_width = (bool)value ? Am_MAX_FIXED_SIZE : Am_NOT_FIXED_SIZE;
  value = self.Get(Am_FIXED_HEIGHT);
  if (value.type == Am_INT)
    fixed_height = value;
  else
    fixed_height = (bool)value ? Am_MAX_FIXED_SIZE : Am_NOT_FIXED_SIZE;
  if ((fixed_height == Am_MAX_FIXED_SIZE) ||
      (fixed_width == Am_MAX_FIXED_SIZE)) {
    int max_width, max_height;
    get_max_sizes (components, max_width, max_height);
    if (fixed_width == Am_MAX_FIXED_SIZE)
      fixed_width = max_width;
    if (fixed_height == Am_MAX_FIXED_SIZE)
      fixed_height = max_height;
  }
}

void get_max_rank_and_size (Am_Object self, 
			    int& max_rank, int& max_size)
{
  Am_Value value;
  value = self.Peek(Am_MAX_RANK);
  if (value.type == Am_INT)
    max_rank = value;
  else
    max_rank = 0;
  value = self.Peek(Am_MAX_SIZE);
  if (value.type == Am_INT)
    max_size = value;
  else
    max_size = 0;
}

void find_line_size_and_rank (Am_Value_List& components, int indent,
			      int fixed_primary, Am_Slot_Key primary_slot,
			      int primary_spacing, Am_Slot_Key secondary_slot,
			      int max_rank, int max_size,
			      int& rank, int& max_secondary)
{
  Am_Object start_item;
  start_item = components.Get ();
  rank = 0;
  max_secondary = 0;
  int position = indent;
  while (!components.Last ()) {
    Am_Object item;
    item = components.Get ();
    if ((bool)item.Get (Am_VISIBLE)) {
      int primary = item.Get (primary_slot);
      int secondary = item.Get (secondary_slot);
      if (rank &&
	  ((max_rank && (rank == max_rank)) ||
	   (max_size &&
	    ((position + (fixed_primary ? fixed_primary : primary)) >=
	     max_size))))
	break;
      ++rank;
      position += (fixed_primary ? fixed_primary : primary) + primary_spacing;
      if (secondary > max_secondary)
        max_secondary = secondary;
    }
    components.Next ();
  }
  components.Start ();
  components.Member (start_item);
}

Am_Define_Formula (int, Am_Horizontal_Layout)
{
  Am_Value_List components;
  components = self.Get (Am_GRAPHICAL_PARTS);
  Am_Object component;
  for (components.Start(); !components.Last(); components.Next()) {
    component = components.Get();
    if (!(bool)component.Get(Am_VISIBLE)) components.Delete();
  }
  int fixed_width, fixed_height;
  get_fixed_sizes (self, components, fixed_width, fixed_height);
  int max_rank, max_size;
  get_max_rank_and_size (self, max_rank, max_size);
  int left_offset = self.Get (Am_LEFT_OFFSET);
  int left = left_offset;
  int top = self.Get (Am_TOP_OFFSET);
  int h_spacing = self.Get (Am_H_SPACING);
  int v_spacing = self.Get (Am_V_SPACING);
  Am_Alignment h_align = self.Get (Am_H_ALIGN);
  Am_Alignment v_align = self.Get (Am_V_ALIGN);
  int indent = self.Get (Am_INDENT);
  components.Start ();
  while (!components.Last ()) {
    int line_rank;
    int line_height;
    find_line_size_and_rank (components, left, fixed_width, Am_WIDTH,
			     h_spacing, Am_HEIGHT, max_rank, max_size,
			     line_rank, line_height);
    int rank = 0;
    while (rank < line_rank) {
      Am_Object item;
      item = components.Get ();
      if ((bool)item.Get (Am_VISIBLE)) {
        int width = item.Get (Am_WIDTH);
        int height = item.Get (Am_HEIGHT);
	if (fixed_width) {
	  switch (h_align.value) {
	  case Am_LEFT_ALIGN_val:
            item.Set (Am_LEFT, left);
	    break;
	  case Am_RIGHT_ALIGN_val:
            item.Set (Am_LEFT, left + fixed_width - width);
	    break;
	  case Am_CENTER_ALIGN_val:
            item.Set (Am_LEFT, left + (fixed_width - width) / 2);
	    break;
	  default:
	    Am_ERRORO("Bad alignment value " << h_align
		      << " in Am_H_ALIGN of " << self, self, Am_H_ALIGN);
	  }
	}
	else
	  item.Set (Am_LEFT, left);
        switch (v_align.value) {
	case Am_TOP_ALIGN_val:
          item.Set (Am_TOP, top);
	  break;
	case Am_BOTTOM_ALIGN_val:
          item.Set (Am_TOP, top + (fixed_height ? fixed_height : line_height)
		     - height);
	  break;
	case Am_CENTER_ALIGN_val:
          item.Set (Am_TOP, top + ((fixed_height ? fixed_height: line_height)
				    - height) / 2);
	  break;
	default:
	    Am_ERRORO("Bad alignment value " << v_align
		      << " in Am_V_ALIGN of " << self, self, Am_V_ALIGN);
	}
        left += (fixed_width ? fixed_width : width) + h_spacing;
        ++rank;
      }
      components.Next ();
    }
    top += (fixed_height ? fixed_height : line_height) + v_spacing;
    left = left_offset + indent;
  }
  return 0;
}
     
Am_Define_Formula (int, Am_Vertical_Layout)
{
  Am_Value_List components;
  components = self.Get (Am_GRAPHICAL_PARTS);
  Am_Object component;
  for (components.Start(); !components.Last(); components.Next()) {
    component = components.Get();
    if (!(bool)component.Get(Am_VISIBLE)) components.Delete();
  }
  int fixed_width, fixed_height;
  get_fixed_sizes (self, components, fixed_width, fixed_height);
  int max_rank, max_size;
  get_max_rank_and_size (self, max_rank, max_size);
  int top_offset = self.Get (Am_TOP_OFFSET);
  int left = self.Get (Am_LEFT_OFFSET);
  int top = top_offset;
  int h_spacing = self.Get (Am_H_SPACING);
  int v_spacing = self.Get (Am_V_SPACING);
  Am_Alignment h_align = self.Get (Am_H_ALIGN);
  Am_Alignment v_align = self.Get (Am_V_ALIGN);
  int indent = self.Get (Am_INDENT);
  components.Start ();
  while (!components.Last ()) {
    int line_rank;
    int line_width;
    find_line_size_and_rank (components, top, fixed_height, Am_HEIGHT,
			     v_spacing, Am_WIDTH, max_rank, max_size,
			     line_rank, line_width);
    int rank = 0;
    while (rank < line_rank) {
      Am_Object item;
      item = components.Get ();
      if ((bool)item.Get (Am_VISIBLE)) {
        int width = item.Get (Am_WIDTH);
        int height = item.Get (Am_HEIGHT);
	if (fixed_height) {
	  switch (v_align.value) {
	  case Am_TOP_ALIGN_val:
            item.Set (Am_TOP, top);
	    break;
	  case Am_BOTTOM_ALIGN_val:
            item.Set (Am_TOP, top + fixed_height - height);
	    break;
	  case Am_CENTER_ALIGN_val:
            item.Set (Am_TOP, top + (fixed_height - height) / 2);
	    break;
	  default:
	    Am_ERRORO("Bad alignment value " << v_align
		      << " in Am_V_ALIGN of " << self, self, Am_V_ALIGN);
	  }
	}
	else
	  item.Set (Am_TOP, top);
        switch (h_align.value) {
	case Am_LEFT_ALIGN_val:
          item.Set (Am_LEFT, left);
	  break;
	case Am_RIGHT_ALIGN_val:
          item.Set (Am_LEFT, left + (fixed_width ? fixed_width : line_width)
		     - width);
	  break;
	case Am_CENTER_ALIGN_val:
          item.Set (Am_LEFT, left + ((fixed_width ? fixed_width : line_width)
				    - width) / 2);
	  break;
	default:
	    Am_ERRORO("Bad alignment value " << h_align
		      << " in Am_H_ALIGN of " << self, self, Am_H_ALIGN);
	}
        top += (fixed_height ? fixed_height : height) + v_spacing;
        ++rank;
      }
      components.Next ();
    }
    left += (fixed_width ? fixed_width : line_width) + h_spacing;
    top = top_offset + indent;
  }
  return 0;
}
     
Am_Define_Formula (int, Am_Right_Is_Right_Of_Owner)
{
  int my_width = self.Get (Am_WIDTH);
  Am_Object owner = self.Get_Owner ();
  if (owner) {
    int owner_width = owner.Get (Am_WIDTH);
    int right_offset = owner.Get (Am_RIGHT_OFFSET);
    return owner_width - my_width - right_offset;
  }
  else {
    return 0;
  }
}

Am_Define_Formula (int, Am_Bottom_Is_Bottom_Of_Owner)
{
  int my_height = self.Get (Am_HEIGHT);
  Am_Object owner = self.Get_Owner ();
  if (owner) {
    int owner_height = owner.Get (Am_HEIGHT);
    int bottom_offset = owner.Get (Am_BOTTOM_OFFSET);
    return owner_height - my_height - bottom_offset;
  }
  else {
    return 0;
  }
}


class Key_Store_Data : public Am_Wrapper {
  Am_WRAPPER_DATA_DECL (Key_Store)
 public:
  Key_Store_Data (Am_Slot_Key in_key)
  {
    key = in_key;
  }
  Key_Store_Data (Key_Store_Data* proto)
  {
    key = proto->key;
  }
  bool operator== (Key_Store_Data& test)
  { return key == test.key; }

  Am_Slot_Key key;
};

Am_WRAPPER_DATA_IMPL (Key_Store, (this))

static Am_Value same_as_procedure (Am_Object& self)
{
  Am_Value value;
  Key_Store_Data* store =
      (Key_Store_Data*)Am_Object_Advanced::Get_Context ()->Get_Data ();
  value = self.Get (store->key);
  store->Release ();
  return value;
}

Am_Formula Am_Same_As (Am_Slot_Key key)
{
  Am_Formula formula (same_as_procedure, "Am_Same_As");
  formula.Set_Data (new Key_Store_Data (key));
  return formula;
}

static Am_Value from_owner_procedure (Am_Object& self)
{
  Am_Value value;
  Am_Object owner = self.Get_Owner ();
  if (owner.Valid ()) {
    Key_Store_Data* store =
        (Key_Store_Data*)Am_Object_Advanced::Get_Context ()->Get_Data ();
    value = owner.Get (store->key);
    store->Release ();
  }
  else
    value.Set_Value_Type(Am_ZERO); //not there, return a ZERO type
  return value;
}

Am_Formula Am_From_Owner (Am_Slot_Key key)
{
  Am_Formula formula (from_owner_procedure, "Am_From_Owner");
  formula.Set_Data (new Key_Store_Data (key));
  return formula;
}

class Part_Key_Store_Data : public Am_Wrapper {
  Am_WRAPPER_DATA_DECL (Part_Key_Store)
 public:
  Part_Key_Store_Data (Am_Slot_Key in_part, Am_Slot_Key in_key)
  {
    part = in_part;
    key = in_key;
  }
  Part_Key_Store_Data (Part_Key_Store_Data* proto)
  {
    part = proto->part;
    key = proto->key;
  }
  bool operator== (Part_Key_Store_Data& test)
  { return key == test.key && part == test.part; }

  Am_Slot_Key part;
  Am_Slot_Key key;
};

Am_WRAPPER_DATA_IMPL (Part_Key_Store, (this))

static Am_Value from_part_procedure (Am_Object& self)
{
  Am_Value value;
  Part_Key_Store_Data* store =
       (Part_Key_Store_Data*)Am_Object_Advanced::Get_Context ()->Get_Data ();
  value = self.Get_Object (store->part).Get (store->key);
  store->Release ();
  return value;
}

Am_Formula Am_From_Part (Am_Slot_Key part, Am_Slot_Key key)
{
  Am_Formula formula (from_part_procedure, "Am_From_Part");
  formula.Set_Data (new Part_Key_Store_Data (part, key));
  return formula;
}

static Am_Value from_sibling_procedure (Am_Object& self)
{
  Am_Value value;
  Part_Key_Store_Data* store =
       (Part_Key_Store_Data*)Am_Object_Advanced::Get_Context ()->Get_Data  ();
  value = self.Get_Owner ().Get_Object (store->part).Get (store->key);
  store->Release ();
  return value;
}

Am_Formula Am_From_Sibling (Am_Slot_Key part, Am_Slot_Key key)
{
  Am_Formula formula (from_sibling_procedure, "Am_From_Sibling");
  formula.Set_Data (new Part_Key_Store_Data (part, key));
  return formula;
}

class Object_Key_Store_Data : public Am_Wrapper {
  Am_WRAPPER_DATA_DECL (Object_Key_Store)
 public:
  Object_Key_Store_Data (Am_Object in_object, Am_Slot_Key in_key)
  {
    object = in_object;
    key = in_key;
  }
  Object_Key_Store_Data (Object_Key_Store_Data* proto)
  {
    object = proto->object;
    key = proto->key;
  }
  bool operator== (Object_Key_Store_Data& test)
  { return key == test.key && object == test.object; }

  Am_Object object;
  Am_Slot_Key key;
};

Am_WRAPPER_DATA_IMPL (Object_Key_Store, (this))

static Am_Value from_object_procedure (Am_Object& /*self*/)
{
  Am_Value value;
  Object_Key_Store_Data* store =
       (Object_Key_Store_Data*)Am_Object_Advanced::Get_Context ()->Get_Data ();
  value = (store->object).Get (store->key);
  store->Release ();
  return value;
}

Am_Formula Am_From_Object (Am_Object object, Am_Slot_Key key)
{
  Am_Formula formula (from_object_procedure, "Am_From_Object");
  formula.Set_Data (new Object_Key_Store_Data (object, key));
  return formula;
}

class Key_Offset_Store_Data : public Am_Wrapper {
  Am_WRAPPER_DATA_DECL (Key_Offset_Store)
 public:
  Key_Offset_Store_Data (Am_Slot_Key in_key, int in_offset,
			 float in_multiplier)
  {
    key = in_key;
    offset = in_offset;
    multiplier = in_multiplier;
  }
  Key_Offset_Store_Data (Key_Offset_Store_Data* proto)
  {
    key = proto->key;
    offset = proto->offset;
    multiplier = proto->multiplier;
  }
  bool operator== (Key_Offset_Store_Data& test)
  { return key == test.key && offset == test.offset
      && multiplier == test.multiplier; }

  Am_Slot_Key key;
  int offset;
  float multiplier;
};

Am_WRAPPER_DATA_IMPL (Key_Offset_Store, (this))

inline void modify_value (Am_Value& value, int offset, float multiplier)
{
  switch (value.type) {
  case Am_INT:
  case Am_LONG:
    value.value.long_value = (long)(value.value.long_value * multiplier) +
                             offset;
    break;
  case Am_FLOAT:
    value.value.float_value = (value.value.float_value * multiplier) + offset;
    break;
  case Am_DOUBLE: {
    double d_value = value;
    d_value = (d_value * multiplier) + offset;
    value = d_value;
    break;
  }
  default:
    break;
  }
}

static Am_Value offset_same_as_procedure (Am_Object& self)
{
  Am_Value value;
  Key_Offset_Store_Data* store =
      (Key_Offset_Store_Data*)Am_Object_Advanced::Get_Context ()->Get_Data ();
  value = self.Get (store->key);
  store->Release ();
  modify_value (value, store->offset, store->multiplier);
  return value;
}

Am_Formula Am_Same_As (Am_Slot_Key key, int offset, float multiplier)
{
  Am_Formula formula (offset_same_as_procedure, "Am_Same_As");
  formula.Set_Data (new Key_Offset_Store_Data (key, offset, multiplier));
  return formula;
}

			     
static Am_Value offset_from_owner_procedure (Am_Object& self)
{
  Am_Value value;
  Am_Object owner = self.Get_Owner ();
  if (owner.Valid ()) {
    Key_Offset_Store_Data* store =
       (Key_Offset_Store_Data*)Am_Object_Advanced::Get_Context ()->Get_Data ();
    value = owner.Get (store->key);
    store->Release ();
    modify_value (value, store->offset, store->multiplier);
  }
  else
    value.Set_Value_Type(Am_ZERO); //not there, return a ZERO type
  return value;
}

Am_Formula Am_From_Owner (Am_Slot_Key key, int offset, float multiplier)
{
  Am_Formula formula (offset_from_owner_procedure, "Am_From_Owner");
  formula.Set_Data (new Key_Offset_Store_Data (key, offset, multiplier));
  return formula;
}

class Part_Key_Offset_Store_Data : public Am_Wrapper {
  Am_WRAPPER_DATA_DECL (Part_Key_Offset_Store)
 public:
  Part_Key_Offset_Store_Data (Am_Slot_Key in_part, Am_Slot_Key in_key,
			      int in_offset, float in_multiplier)
  {
    part = in_part;
    key = in_key;
    offset = in_offset;
    multiplier = in_multiplier;
  }
  Part_Key_Offset_Store_Data (Part_Key_Offset_Store_Data* proto)
  {
    part = proto->part;
    key = proto->key;
    offset = proto->offset;
    multiplier = proto->multiplier;
  }
  bool operator== (Part_Key_Offset_Store_Data& test)
  { return key == test.key && part == test.part && offset == test.offset
      && multiplier == test.multiplier; }

  Am_Slot_Key part;
  Am_Slot_Key key;
  int offset;
  float multiplier;
};

Am_WRAPPER_DATA_IMPL (Part_Key_Offset_Store, (this))

static Am_Value offset_from_part_procedure (Am_Object& self)
{
  Am_Value value;
  Part_Key_Offset_Store_Data* store = (Part_Key_Offset_Store_Data*)
       Am_Object_Advanced::Get_Context ()->Get_Data ();
  value = self.Get_Object (store->part).Get (store->key);
  store->Release ();
  modify_value (value, store->offset, store->multiplier);
  return value;
}

Am_Formula Am_From_Part (Am_Slot_Key part, Am_Slot_Key key, int offset,
			 float multiplier)
{
  Am_Formula formula (offset_from_part_procedure, "Am_From_Part");
  formula.Set_Data (new Part_Key_Offset_Store_Data (part, key, offset,
						    multiplier));
  return formula;
}

static Am_Value offset_from_sibling_procedure (Am_Object& self)
{
  Am_Value value;
  Part_Key_Offset_Store_Data* store = (Part_Key_Offset_Store_Data*)
       Am_Object_Advanced::Get_Context ()->Get_Data  ();
  value = self.Get_Owner ().Get_Object (store->part).Get (store->key);
  store->Release ();
  modify_value (value, store->offset, store->multiplier);
  return value;
}

Am_Formula Am_From_Sibling (Am_Slot_Key sibling, Am_Slot_Key key, int offset,
			    float multiplier)
{
  Am_Formula formula (offset_from_sibling_procedure, "Am_From_Sibling");
  formula.Set_Data (new Part_Key_Offset_Store_Data (sibling, key, offset,
						    multiplier));
  return formula;
}

class Object_Key_Offset_Store_Data : public Am_Wrapper {
  Am_WRAPPER_DATA_DECL (Object_Key_Offset_Store)
 public:
  Object_Key_Offset_Store_Data (Am_Object in_object, Am_Slot_Key in_key,
				int in_offset, float in_multiplier)
  {
    object = in_object;
    key = in_key;
    offset = in_offset;
    multiplier = in_multiplier;
  }
  Object_Key_Offset_Store_Data (Object_Key_Offset_Store_Data* proto)
  {
    object = proto->object;
    key = proto->key;
    offset = proto->offset;
    multiplier = proto->multiplier;
  }
  bool operator== (Object_Key_Offset_Store_Data& test)
  { return key == test.key && object == test.object && offset == test.offset
      && multiplier == test.multiplier; }

  Am_Object object;
  Am_Slot_Key key;
  int offset;
  float multiplier;
};

Am_WRAPPER_DATA_IMPL (Object_Key_Offset_Store, (this))

static Am_Value offset_from_object_procedure (Am_Object& /*self*/)
{
  Am_Value value;
  Object_Key_Offset_Store_Data* store = (Object_Key_Offset_Store_Data*)
         Am_Object_Advanced::Get_Context ()->Get_Data ();
  value = (store->object).Get (store->key);
  store->Release ();
  modify_value (value, store->offset, store->multiplier);
  return value;
}

Am_Formula Am_From_Object (Am_Object object, Am_Slot_Key key, int offset,
			   float multiplier)
{
  Am_Formula formula (offset_from_object_procedure, "Am_From_Object");
  formula.Set_Data (new Object_Key_Offset_Store_Data (object, key, offset,
						      multiplier));
  return formula;
}

/******************************************
 ** Support for Aggregates and Groups    **
 ******************************************/

void Am_Invalid_Rectangle_Intersect(int left, int top, int width, int height,
				    int my_left, int my_top, int my_width,
				    int my_height,
				    int& final_left, int& final_top,
				    int& final_width, int& final_height) {
  if (left >= 0)
    final_left = my_left + left;
  else
    final_left = my_left;
  if ((left + width) < my_width)
    final_width = my_left + left + width - final_left;
  else
    final_width = my_left + my_width - final_left;
  if (top >= 0)
    final_top = my_top + top;
  else
    final_top = my_top;
  if ((top + height) < my_height)
    final_height = my_top + top + height - final_top;
  else
    final_height = my_top + my_height - final_top;
}  

Am_Define_Method(Am_Invalid_Method, void, aggregate_invalid,
		 (Am_Object group, Am_Object /*which_part*/,
		  int left, int top, int width, int height))
{
  Am_Object owner = group.Get_Owner (Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
  if (owner.Valid ()) {
    int my_left = group.Get (Am_LEFT, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    int my_top = group.Get (Am_TOP, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    int my_width = group.Get (Am_WIDTH, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    int my_height = group.Get (Am_HEIGHT, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    int final_left, final_top, final_width, final_height;
    // clip incoming rectangle to my rectangle
    Am_Invalid_Rectangle_Intersect(left, top, width, height,
				   my_left, my_top, my_width, my_height,
				   final_left, final_top,
				   final_width, final_height);
    if ((final_width > 0) && (final_height > 0))
      Am_Invalidate (owner, group, final_left, final_top, final_width,
		     final_height);
  }
}

Am_Define_Method (Am_Draw_Method, void, aggregate_draw,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  if (width && height) {
    drawonable->Push_Clip (left, top, width, height);
//// DEBUG
//cout << "aggregate " << *self << " clip mask <l" << left << " t" << top
//     << " w" << width << " h" << height << ">" << endl;
    Am_Value_List components = self.Get (Am_GRAPHICAL_PARTS);
    Am_Object item;
    for (components.Start (); !components.Last (); components.Next ()) {
      item = components.Get ();
      Am_State_Store* state =
	Am_State_Store::Narrow (item.Get (Am_PREV_STATE));
      if (state->Visible (drawonable, left, top))
	Am_Draw (item, drawonable, left, top);
    }
    drawonable->Pop_Clip ();
  }
}

Am_Define_Method (Am_Draw_Method, void, aggregate_mask,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  if (width && height) {
    drawonable->Push_Clip (left, top, width, height);
    Am_Value_List components = self.Get (Am_GRAPHICAL_PARTS);
    Am_Object item;
    for (components.Start (); !components.Last (); components.Next ()) {
      item = components.Get ();
      Am_State_Store* state =
	  Am_State_Store::Narrow (item.Get (Am_PREV_STATE));
      if (state->Visible (drawonable, left, top)) {
	Am_Draw_Method draw_method = item.Get (Am_MASK_METHOD);
        draw_method.Call (item, drawonable, left, top);
      }
    }
    drawonable->Pop_Clip ();
  }
}

//demon procedure
void group_create (Am_Object self) {
  Am_Object_Demon* create_demon = ((Am_Object_Advanced&)Am_Aggregate).
      Get_Demons ().Get_Object_Demon (Am_CREATE_OBJ);
  create_demon (self);
  am_generic_renew_components (self);
}

//demon procedure
void group_copy (Am_Object self) {
  Am_Object_Demon* copy_demon = ((Am_Object_Advanced&)Am_Aggregate).
      Get_Demons ().Get_Object_Demon (Am_COPY_OBJ);
  copy_demon (self);
  am_generic_renew_copied_comp (self);
}

void flip_book_create (Am_Object self) {
  Am_Object_Demon* create_demon = ((Am_Object_Advanced&)Am_Aggregate).
      Get_Demons ().Get_Object_Demon (Am_CREATE_OBJ);
  create_demon (self);

  Am_Value_List components;
  components = self.Get (Am_FLIP_BOOK_PARTS);
  Am_Value_List new_components;
  Am_Part_Iterator parts = self;
  int parts_length = parts.Length ();
  // if the object has no inherited graphical parts (all the parts are
  // non-graphical) or no parts, then the new object will still have no
  // graphical parts, so exit quickly.
  if (components.Empty () || (parts_length == 0)) {
    self.Set(Am_FLIP_BOOK_PARTS, Am_No_Value_List);
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
  for (components.Start(); !components.Last (); components.Next ()) {
    current_component = components.Get ();
    for (i = 0; i < parts_length; ++i)
      if (current_component == part_map[i]) {
	new_components.Add (part_map[i+parts_length]);
	break;
      }
  }
  delete [] part_map;
  self.Set (Am_FLIP_BOOK_PARTS, new_components);
}

void flip_book_copy (Am_Object self) {
  Am_Object_Demon* copy_demon = ((Am_Object_Advanced&)Am_Aggregate).
      Get_Demons ().Get_Object_Demon (Am_COPY_OBJ);
  copy_demon (self);

  Am_Value_List components;
  components = self.Get (Am_FLIP_BOOK_PARTS);
  Am_Value_List new_components;
  Am_Part_Iterator parts = self;
  int parts_length = parts.Length ();
  if (components.Empty () || (parts_length == 0)) {
    self.Set(Am_FLIP_BOOK_PARTS, Am_No_Value_List);
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
  for (components.Start(); !components.Last (); components.Next ()) {
    current_component = components.Get ();
    for (i = 0; i < parts_length; ++i)
      if (current_component == part_map[i]) {
	new_components.Add (part_map[i+parts_length]);
	break;
      }
  }
  delete [] part_map;
  self.Set (Am_FLIP_BOOK_PARTS, new_components);
}

void flip_book_add_part (Am_Object owner, Am_Object old_object,
			 Am_Object new_object)
{
  Am_Value_List components;
  if (old_object.Valid () && old_object.Is_Instance_Of (Am_Graphical_Object)) {
    owner.Make_Unique (Am_FLIP_BOOK_PARTS);
    components = owner.Get (Am_FLIP_BOOK_PARTS);
    components.Start ();
    if (components.Member (old_object)) {
      components.Delete (false);
      owner.Note_Changed (Am_FLIP_BOOK_PARTS);
    }
  }
  if (new_object.Valid () && new_object.Is_Instance_Of (Am_Graphical_Object)) {
    owner.Make_Unique (Am_FLIP_BOOK_PARTS);
    components = owner.Get (Am_FLIP_BOOK_PARTS);
    bool was_empty = components.Empty ();
    int value = (int)owner.Get (Am_VALUE, Am_RETURN_ZERO_ON_ERROR) %
                (components.Length () + 1);
    if (was_empty)
      owner.Set (Am_FLIP_BOOK_PARTS, Am_Value_List ().Add (new_object));
    else {
      components.Move_Nth (value);
      components.Insert (new_object, Am_BEFORE, false);
      owner.Note_Changed (Am_FLIP_BOOK_PARTS);
    }
  }
}

Am_Define_Value_List_Formula (flip_book_parts)
{
  Am_Value_List parts_list = self.Get (Am_FLIP_BOOK_PARTS);
  int i;
  parts_list.Start ();
  bool was_unique = self.Is_Unique (Am_GRAPHICAL_PARTS);
  Am_Value_List component = self.Get (Am_GRAPHICAL_PARTS, Am_NO_DEPENDENCY);
  if (parts_list.Last ()) {
    component.Start ();
    if (!component.Last ()) {
      component.Delete (!was_unique);
      if (was_unique)
        self.Note_Changed (Am_GRAPHICAL_PARTS);
    }
  } else {
    int position = (int)self.Get (Am_VALUE, Am_RETURN_ZERO_ON_ERROR) %
      parts_list.Length ();
    for (i = 0; i < position; ++i, parts_list.Next ()) ;
    Am_Object current = parts_list.Get ();
    component.Start ();
    if (component.Last ()) {
      was_unique = was_unique && component.Valid ();
      component.Add (current, Am_TAIL, !was_unique);
      current.Set (Am_RANK, 0);
      if (was_unique)
        self.Note_Changed (Am_GRAPHICAL_PARTS);
    } else if (component.Get () != current) {
      component.Set (current, !was_unique);
      current.Set (Am_RANK, 0);
      if (was_unique)
        self.Note_Changed (Am_GRAPHICAL_PARTS);
    }
  }
  return component;
}

static Am_Style clear_halftone (int percent)
{
  if (percent < 0)
    percent = 0;
  else if (percent > 99)
    percent = 99;
  static Am_Style halftone_array [100];
  if (!halftone_array[percent].Valid ())
    halftone_array[percent] = Am_Style::Halftone_Stipple (percent).
        Clone_With_New_Color (Am_Off_Bits);
  return halftone_array[percent];
}

void Am_Get_Effects_Buffers (Am_Object screen, unsigned short buffer,
			     Am_Drawonable*& primary, Am_Drawonable*& mask)
{
  Am_Value primary_value, mask_value;
  primary_value=screen.Peek(Am_DRAW_BUFFER);
  mask_value=screen.Peek(Am_MASK_BUFFER);
  int i;
  if (Am_Value_List::Test (primary_value)) {
    Am_Value_List primary_list = primary_value;
    Am_Value_List mask_list = mask_value;
    primary_list.Start ();
    mask_list.Start ();
    for (i = 0; (i < buffer) && !primary_list.Last (); ++i) {
      primary_list.Next ();
      mask_list.Next ();
    }
    if (primary_list.Last ()) {
      primary = NULL;
      mask = NULL;
    }
    else {
      primary = Am_Drawonable::Narrow (primary_list.Get ());
      mask = Am_Drawonable::Narrow (mask_list.Get ());
    }
  }
  else if (buffer == 0) {
    primary = Am_Drawonable::Narrow (primary_value);
    mask = Am_Drawonable::Narrow (mask_value);
  }
  else {
    primary = NULL;
    mask = NULL;
  }
  if (!primary) {
    Am_Drawonable* screen_drawonable =
          Am_Drawonable::Narrow (screen.Get(Am_DRAWONABLE));
    primary = screen_drawonable->
          Create_Offscreen (100, 100, Am_Off_Bits);
    mask = screen_drawonable->
          Create_Offscreen (100, 100, Am_Off_Bits);
    if (Am_Value_List::Test (primary_value)) {
      Am_Value_List primary_list = primary_value;
      Am_Value_List mask_list = mask_value;
      primary_list.Start ();
      mask_list.Start ();
      for (i = 0; i < buffer; ++i) {
        if (primary_list.Last ()) {
          primary_list.Add ((Am_Ptr)NULL);
          mask_list.Add ((Am_Ptr)NULL);
	}
        else {
          primary_list.Next ();
          mask_list.Next ();
	}
      }
      primary_list.Set ((Am_Ptr)primary);
      mask_list.Set ((Am_Ptr)mask);
      screen.Set (Am_DRAW_BUFFER, primary_list);
      screen.Set (Am_MASK_BUFFER, mask_list);
    }
    else if (buffer == 0) {
      screen.Set (Am_DRAW_BUFFER, (Am_Ptr)primary);
      screen.Set (Am_MASK_BUFFER, (Am_Ptr)mask);
    }
    else {
      Am_Value_List primary_list, mask_list;
      primary_list.Add (primary_value);
      mask_list.Add (mask_value);
      for (i = 1; i < buffer; ++i) {
        primary_list.Add ((Am_Ptr)NULL);
        mask_list.Add ((Am_Ptr)NULL);
      }
      primary_list.Add ((Am_Ptr)primary);
      mask_list.Add ((Am_Ptr)mask);
      screen.Set (Am_DRAW_BUFFER, primary_list);
      screen.Set (Am_MASK_BUFFER, mask_list);
    }
  }
}

Am_Define_Method (Am_Draw_Method, void, fade_group_draw,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  int value = self.Get (Am_VALUE);
  if (width <= 0 || height <= 0 || value >= 99)
    return;
  Am_Value_List components = self.Get (Am_GRAPHICAL_PARTS);
  Am_Object item;
  if (value == 0) {
    drawonable->Push_Clip (left, top, width, height);
    for (components.Start (); !components.Last (); components.Next ()) {
      item = components.Get ();
      Am_State_Store* state =
            Am_State_Store::Narrow (item.Get (Am_PREV_STATE));
      if (state->Visible (drawonable, left, top))
        Am_Draw (item, drawonable, left, top);
    }
    drawonable->Pop_Clip ();
    return;
  }
  Am_Object window = self.Get (Am_WINDOW);
  Am_Object screen;
  for (screen = window.Get_Owner(); !screen.Is_Instance_Of(Am_Screen);
       screen = screen.Get_Owner()) ;
  Am_Drawonable* local_drawonable;
  Am_Drawonable* mask_drawonable;
  int depth = self.Get (Am_FADE_DEPTH);
  Am_Get_Effects_Buffers (screen, depth, local_drawonable, mask_drawonable);
  int draw_width, draw_height;
  local_drawonable->Get_Size (draw_width, draw_height);
  if ((left+width > draw_width) || (top+height > draw_height)) {
    local_drawonable->Set_Size (left+width, top+height);
    mask_drawonable->Set_Size (left+width, top+height);
  }
  local_drawonable->Push_Clip (left, top, width, height);
  local_drawonable->Clear_Area (left, top, width, height);
  mask_drawonable->Clear_Area (left, top, width, height);
  for (components.Start (); !components.Last (); components.Next ()) {
    item = components.Get ();
    Am_State_Store* state =
          Am_State_Store::Narrow (item.Get (Am_PREV_STATE));
    if (state->Visible (local_drawonable, left, top)) {
      Am_Draw (item, local_drawonable, left, top);
      Am_Draw_Method mask_draw = item.Get (Am_MASK_METHOD);
      mask_draw.Call (item, mask_drawonable, left, top);
    }
  }
  local_drawonable->Draw_Rectangle (Am_No_Style, clear_halftone (value),
				    left, top, width, height, Am_DRAW_COPY);
  mask_drawonable->Draw_Rectangle (Am_No_Style, clear_halftone (value),
				   left, top, width, height, Am_DRAW_COPY);
  local_drawonable->Pop_Clip ();
  drawonable->Push_Clip (left, top, width, height);
  drawonable->Bitblt (left, top, width, height, mask_drawonable,
		      left, top, Am_DRAW_GRAPHIC_NIMP);
  drawonable->Bitblt (left, top, width, height, local_drawonable,
		      left, top, Am_DRAW_GRAPHIC_OR);
  drawonable->Pop_Clip ();
}

Am_Define_Method (Am_Draw_Method, void, fade_group_mask,
		  (Am_Object self, Am_Drawonable* drawonable,
		   int x_offset, int y_offset))
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top = (int)self.Get (Am_TOP) + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);
  int value = self.Get (Am_VALUE);
  if (width <= 0 || height <= 0 || value >= 99)
    return;
  Am_Value_List components = self.Get (Am_GRAPHICAL_PARTS);
  Am_Object item;
  if (value == 0) {
    drawonable->Push_Clip (left, top, width, height);
    for (components.Start (); !components.Last (); components.Next ()) {
      item = components.Get ();
      Am_State_Store* state =
            Am_State_Store::Narrow (item.Get (Am_PREV_STATE));
      if (state->Visible (drawonable, left, top)) {
        Am_Draw_Method mask_draw = item.Get (Am_MASK_METHOD);
        mask_draw.Call (item, drawonable, left, top);
      }
    }
    drawonable->Pop_Clip ();
    return;
  }
  Am_Object window = self.Get (Am_WINDOW);
  Am_Object screen;
  for (screen = window.Get_Owner(); !screen.Is_Instance_Of(Am_Screen);
       screen = screen.Get_Owner()) ;
  Am_Drawonable* local_drawonable;
  Am_Drawonable* mask_drawonable;
  int depth = self.Get (Am_FADE_DEPTH);
  Am_Get_Effects_Buffers (screen, depth, local_drawonable, mask_drawonable);
  int draw_width, draw_height;
  local_drawonable->Get_Size (draw_width, draw_height);
  if ((left+width > draw_width) || (top+height > draw_height)) {
    local_drawonable->Set_Size (left+width, top+height);
    mask_drawonable->Set_Size (left+width, top+height);
  }
  local_drawonable->Push_Clip (left, top, width, height);
  local_drawonable->Clear_Area (left, top, width, height);
  for (components.Start (); !components.Last (); components.Next ()) {
    item = components.Get ();
    Am_State_Store* state =
          Am_State_Store::Narrow (item.Get (Am_PREV_STATE));
    if (state->Visible (local_drawonable, left, top)) {
      Am_Draw_Method mask_draw = item.Get (Am_MASK_METHOD);
      mask_draw.Call (item, local_drawonable, left, top);
    }
  }
  local_drawonable->Draw_Rectangle (Am_No_Style, clear_halftone (value),
      left, top, width, height, Am_DRAW_COPY);
  local_drawonable->Pop_Clip ();
  drawonable->Push_Clip (left, top, width, height);
  drawonable->Bitblt (left, top, width, height, local_drawonable,
                      left, top, Am_DRAW_GRAPHIC_OR);
  drawonable->Pop_Clip ();
}

Am_Define_Formula (int, am_fade_depth)
{
  Am_Object owner = self.Get_Owner ();
  while (owner.Valid () && !owner.Is_Instance_Of (Am_Fade_Group))
    owner = owner.Get_Owner ();
  if (owner.Valid ())
    return (int)owner.Get (Am_FADE_DEPTH) + 1;
  else
    return 0;
}

//demon procedure
void resize_group_parts (Am_Slot first_invalidated) {
  Am_Object self;
  self = first_invalidated.Get_Owner ();
  cout << "resize of " << self << endl << flush;
  int old_width, old_height, new_width, new_height;
  float width_ratio, height_ratio;
  old_width = self.Get(Am_OLD_WIDTH);
  old_height = self.Get(Am_OLD_HEIGHT);
  new_width = self.Get(Am_WIDTH);
  new_height = self.Get(Am_HEIGHT);
  if (old_width && old_height) { //otherwise, first time changed
    if (new_width == 0) new_width = 1;
    if (new_height == 0) new_height = 1;
    width_ratio = (float)new_width / (float)old_width;
    height_ratio = (float)new_height / (float)old_height;
    Am_Value_List parts;
    parts = self.Get(Am_GRAPHICAL_PARTS);
    Am_Object part;
    for (parts.Start(); !parts.Last(); parts.Next()) {
      part = parts.Get();
      int left = (int)part.Get(Am_LEFT);
      int top = (int)part.Get(Am_TOP);
      int height = (int)part.Get(Am_HEIGHT);
      int width = (int)part.Get(Am_WIDTH);
      part.Set(Am_LEFT, (int) (left * width_ratio));
      part.Set(Am_WIDTH, (int) (width * width_ratio));
      part.Set(Am_TOP, (int) (top * height_ratio));
      part.Set(Am_HEIGHT, (int) (height * height_ratio));
    }
  }
  self.Set(Am_OLD_HEIGHT, new_height);
  self.Set(Am_OLD_WIDTH, new_width);
}

Am_Define_Value_List_Formula (map_make_components) {
  self.Make_Unique (Am_GRAPHICAL_PARTS);
  Am_Value value;
  value=self.Peek(Am_GRAPHICAL_PARTS, Am_NO_DEPENDENCY);
  Am_Value_List components;
  if (value.Exists ())
    components = value;
  bool was_empty = components.Empty ();
  Am_Item_Method item_method = self.Get (Am_ITEM_METHOD);
  Am_Object item_prototype = self.Get (Am_ITEM_PROTOTYPE, Am_RETURN_ZERO_ON_ERROR);
  value = self.Get (Am_ITEMS);
  Am_Value stored_value = 0;
  int i;
  Am_Object item_instance;
  Am_Object new_item;
  components.Start ();
  if (value.type == Am_INT) {
    int number = value;
    for (i = 0; i < number; ++i) {
      if (components.Last ()) {
	if (item_prototype.Valid ())
          item_instance = item_prototype.Create ();
	else
          item_instance = Am_No_Object;
	if (item_instance.Valid ())
          self.Add_Part (item_instance, false);
        new_item = item_method.Call (i, stored_value, item_instance);
        if (new_item != item_instance) {
          item_instance.Destroy ();
          if (new_item.Valid ())
            self.Add_Part (new_item, false);
	}
        if (new_item.Valid ())
          components.Add (new_item, Am_TAIL, false);
      }
      else {
        item_instance = components.Get ();
        if (item_instance.Get_Owner (Am_NO_DEPENDENCY) != self) {
	  if (item_prototype.Valid ())
            item_instance = item_prototype.Create ();
	  else
            item_instance = Am_No_Object;
	}
        new_item = item_method.Call (i, stored_value, item_instance);
        if (new_item != item_instance)
          item_instance.Destroy ();
        if (new_item.Valid ()) {
          components.Set (new_item, false);
	  if (!new_item.Get_Owner (Am_NO_DEPENDENCY))
            self.Add_Part (new_item, false);
        }
        else
          components.Delete (false);
        components.Next ();
      }
    }
  }
  else if (value.type == Am_VALUE_LIST) {
    Am_Value_List item_list;
    item_list = value;
    if (!item_list.Empty ()) {
      i = 0;
      for (item_list.Start (); !item_list.Last (); item_list.Next ()) {
        stored_value = item_list.Get ();
        if (components.Last ()) {
	  if (item_prototype.Valid ())
            item_instance = item_prototype.Create ();
	  else
            item_instance = Am_No_Object;
          if (item_instance.Valid ())
            self.Add_Part (item_instance, false);
          new_item = item_method.Call (i, stored_value, item_instance);
          if (new_item != item_instance) {
            item_instance.Destroy ();
            if (new_item.Valid ())
              self.Add_Part (new_item, false);
	  }
          if (new_item.Valid ())
            components.Add (new_item, Am_TAIL, false);
        }
        else {
          item_instance = components.Get ();
          if (item_instance.Get_Owner (Am_NO_DEPENDENCY) != self) {
	    if (item_prototype.Valid ())
              item_instance = item_prototype.Create ();
	    else
              item_instance = Am_No_Object;
	  }
          new_item = item_method.Call (i, stored_value, item_instance);
          if (new_item != item_instance)
            item_instance.Destroy ();
          if (new_item.Valid ()) {
            components.Set (new_item, false);
	    if (!new_item.Get_Owner (Am_NO_DEPENDENCY))
              self.Add_Part (new_item, false);
          }
          else
            components.Delete (false);
          components.Next ();
        }
        ++i;
      }
    }
  }
  else if (value.Valid()) {  // any type of 0 is valid: no items.
    cerr << "** Value of Am_ITEMS slot in object, " << self << ", is not an"
      " integer or Am_Value_List." << endl;
    Am_Error ();
  }
  while (!components.Last ()) {
    item_instance = components.Get ();
    if (item_instance.Get_Owner (Am_NO_DEPENDENCY) == self)
      item_instance.Destroy ();
    components.Delete (false);
    components.Next ();
  }
  if (!was_empty && !components.Empty ())
    self.Note_Changed (Am_GRAPHICAL_PARTS);
  if (components.Empty ())
    return Am_Value_List ();
  else
    return components;
}

Am_Define_Method(Am_Item_Method, Am_Object, Am_Standard_Item_Method,
		 (int rank, Am_Value& value, Am_Object item_instance))
{
  if (item_instance.Valid ()) {
    item_instance.Set (Am_RANK, rank, Am_OK_IF_NOT_THERE);
    item_instance.Set (Am_ITEM, value, Am_OK_IF_NOT_THERE);
  }
  return item_instance;
}

///////////////////////////////////
// Default Load and Save Methods //
///////////////////////////////////

// For loading and saving files.
Am_Load_Save_Context Am_Default_Load_Save_Context;

Am_Define_Method (Am_Load_Method, Am_Value, load_none,
		  (istream& is, Am_Load_Save_Context& /* context */))
{
  char ch;
  is.get (ch); // skip eoln
  return Am_Value(); //null Am_Value
}

Am_Define_Method (Am_Save_Method, void, save_none,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& /* value */))
{
  context.Save_Type_Name (os, "Am_NONE");
  os << endl;
}

Am_Define_Method (Am_Load_Method, Am_Value, load_int,
		  (istream& is, Am_Load_Save_Context& /* context */))
{
  int value;
  is >> value;
  char ch;
  is.get (ch); // skip eoln
  return Am_Value (value);
}

Am_Define_Method (Am_Save_Method, void, save_int,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value))
{
  context.Save_Type_Name (os, "Am_INT");
  os << (int)value << endl;
}

Am_Define_Method (Am_Load_Method, Am_Value, load_long,
		  (istream& is, Am_Load_Save_Context& /* context */))
{
  long value;
  is >> value;
  char ch;
  is.get (ch); // skip eoln
  return Am_Value (value);
}

Am_Define_Method (Am_Save_Method, void, save_long,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value))
{
  context.Save_Type_Name (os, "Am_LONG");
  os << (long)value << endl;
}

Am_Define_Method (Am_Load_Method, Am_Value, load_bool,
		  (istream& is, Am_Load_Save_Context& /* context */))
{
  char value[10];
  is >> value;
  char ch;
  is.get (ch); // skip eoln
  return Am_Value ((bool)!strcmp (value, "true"));
}

Am_Define_Method (Am_Save_Method, void, save_bool,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value))
{
  context.Save_Type_Name (os, "Am_BOOL");
  if ((bool)value)
    os << "true" << endl;
  else
    os << "false" << endl;
}

Am_Define_Method (Am_Load_Method, Am_Value, load_float,
		  (istream& is, Am_Load_Save_Context& /* context */))
{
  float value;
  is >> value;
  char ch;
  is.get (ch); // skip eoln
  return Am_Value (value);
}

Am_Define_Method (Am_Save_Method, void, save_float,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value))
{
  context.Save_Type_Name (os, "Am_FLOAT");
  os << (float)value << endl;
}

Am_Define_Method (Am_Load_Method, Am_Value, load_double,
		  (istream& is, Am_Load_Save_Context& /* context */))
{
  double value;
  is >> value;
  char ch;
  is.get (ch); // skip eoln
  return Am_Value (value);
}

Am_Define_Method (Am_Save_Method, void, save_double,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value))
{
  context.Save_Type_Name (os, "Am_DOUBLE");
  os << (double)value << endl;
}

Am_Define_Method (Am_Load_Method, Am_Value, load_char,
		  (istream& is, Am_Load_Save_Context& /* context */))
{
  char value;
  is >> value;
  char ch;
  is.get (ch); // skip eoln
  return Am_Value (value);
}

Am_Define_Method (Am_Save_Method, void, save_char,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value))
{
  context.Save_Type_Name (os, "Am_CHAR");
  os << (char)value << endl;
}

Am_Define_Method (Am_Load_Method, Am_Value, load_string,
		  (istream& is, Am_Load_Save_Context& /* context */))
{
  char ch;
  int length;
  is >> length;
  is.get (ch); // skip eoln
  char* value = new char[length+1];
  int i;
  for (i = 0; i < length; ++i) {
    is.get (value[i]);
  }
  value[length] = '\0';
  is.get (ch); // skip eoln
  Am_String string (value);
  delete value;
  return Am_Value (string);
}

Am_Define_Method (Am_Save_Method, void, save_string,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value))
{
  context.Save_Type_Name (os, "Am_STRING");
  Am_String string = value;
  const char* string_val = string;
  int length = strlen (string_val);
  os << length << endl;
  int i;
  for (i = 0; i < length; ++i) {
    os << string_val[i];
  }
  os << endl;
}

Am_Define_Method (Am_Load_Method, Am_Value, load_list,
		  (istream& is, Am_Load_Save_Context& context))
{
  Am_Value_List list = Am_Value_List::Empty_List ();
  context.Recursive_Load_Ahead (list);
  int length;
  is >> length;
  char ch;
  is.get (ch); // skip eoln
  int i;
  Am_Value v;
  for (i = 0; i < length; ++i) {
    v = context.Load (is);
    //if any errors, return immediately
    if (v == Am_No_Value) return Am_No_Value; 
    list.Add (v, Am_TAIL, false);
  }
  return Am_Value (list);
}

Am_Define_Method (Am_Save_Method, void, save_list,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value))
{
  context.Save_Type_Name (os, "Am_VALUE_LIST");
  Am_Value_List list = value;
  os << list.Length () << endl;
  for (list.Start (); !list.Last (); list.Next ())
    context.Save (os, list.Get ());
}

Am_Define_Method (Am_Load_Method, Am_Value, load_point_list,
		  (istream& is, Am_Load_Save_Context& /*context*/)) {
  int length, i;
  is >> length;
  Am_Point_List list;
  float x, y;
  for (i = 0; i < length; ++i) {
    is >> x;
    is >> y;
    list.Add (x, y, Am_TAIL, false);
  }
  char ch;
  is.get (ch); // skip eoln
  return Am_Value (list);
}

Am_Define_Method (Am_Save_Method, void, save_point_list,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value)) {
  context.Save_Type_Name (os, "Am_POINT_LIST");
  Am_Point_List list = value;
  os << list.Length () << endl;
  float x, y;
  for (list.Start (); !list.Last (); list.Next ()) {
    list.Get(x, y);
    os << x << " " << y << endl;
  }
}

Am_Define_Method (Am_Save_Method, void, save_object,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Value& value))
{
  Am_Object object = value;
  Am_Save_Object_Method method = object.Get (Am_SAVE_OBJECT_METHOD);
  if (!method.Valid())
    Am_ERRORO("Object " << object
      << " does not have a save method in its Am_SAVE_OBJECT_METHOD slot",
	      object, Am_SAVE_OBJECT_METHOD);
  method.Call (os, context, object);
}

void standard_save_internal (ostream& os, Am_Load_Save_Context& context,
			     const Am_Object& object,
			     const char * save_type_name) {
  const char* name;
  //test to see if prototype is registered
  name = context.Is_Registered_Prototype(object);
  Am_Object proto;
  if (name) proto = object;
  else {
    proto = object.Get_Prototype();
    name = context.Is_Registered_Prototype(proto);
    if (!name)
      Am_ERROR("Neither object " << object << " or its prototype " 
	       << proto
	  << " have been registered for save for Am_Standard_Save_Object");
  }
  Am_Value_List slots = proto.Get(Am_SLOTS_TO_SAVE);
  context.Save_Type_Name (os, save_type_name);
  context.Save (os, Am_Value (proto));
  Am_Slot_Key slot;
  for (slots.Start (); !slots.Last (); slots.Next ()) {
    slot = (Am_Slot_Key)(int)slots.Get();
    context.Save (os, object.Get (slot));
  }
}

Am_Define_Method (Am_Save_Object_Method, void, Am_Standard_Save_Object,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Object& object)) {
  standard_save_internal(os, context, object, "Am_STANDARD_SAVE");
}

Am_Define_Method (Am_Save_Object_Method, void, Am_Standard_Save_Group,
		  (ostream& os, Am_Load_Save_Context& context,
		   const Am_Object& object)) {
  standard_save_internal(os, context, object, "Am_STANDARD_SAVE_GROUP");
  Am_Value_List parts = object.Get(Am_GRAPHICAL_PARTS);
  os << parts.Length() << endl;
  Am_Object part;
  for (parts.Start (); !parts.Last (); parts.Next ()) {
    Am_Value v = parts.Get ();
    context.Save (os, v);
  }
}

Am_Object Am_Load_Object_Swap;
    
Am_Define_Method (Am_Load_Method, Am_Value, Am_Standard_Load_Object,
		  (istream& is, Am_Load_Save_Context& context))
{
  Am_Object loaded_obj = context.Load (is);
  if (!loaded_obj.Valid()) return Am_No_Value;
  Am_Object proto;
  if (loaded_obj.Is_Instance_Of (Am_Load_Object_Swap)) {
    proto = loaded_obj.Get (Am_ITEM_PROTOTYPE);
    if (!proto.Valid()) return Am_No_Value;
  }
  else
    proto = loaded_obj;
  Am_Object new_obj = proto.Create ();
  context.Recursive_Load_Ahead (new_obj);
  Am_Value_List slots = loaded_obj.Get (Am_SLOTS_TO_SAVE);
  Am_Slot_Key slot;
  for (slots.Start (); !slots.Last (); slots.Next ()) {
    slot = (Am_Slot_Key)(int)slots.Get();
    new_obj.Set (slot, context.Load (is), Am_OK_IF_NOT_THERE);
  }
  return Am_Value (new_obj);
}

Am_Define_Method (Am_Load_Method, Am_Value, Am_Standard_Load_Group,
		  (istream& is, Am_Load_Save_Context& context)) {
  Am_Value new_obj_val = Am_Standard_Load_Object_proc(is, context);
  if (new_obj_val.Valid()) {
    Am_Object new_obj = new_obj_val;
    int num_parts;
    is >> num_parts;
    Am_Object part;
    int i;
    for (i = 0; i< num_parts; i++) {
      part = context.Load (is);
      new_obj.Add_Part(part);
    }
  }
  return new_obj_val;
}

void set_slot_names ()
{
#ifdef DEBUG
  Am_Register_Slot_Key (Am_NO_SLOT, "NO_SLOT");
  Am_Register_Slot_Key (Am_NO_INHERIT, "NO_INHERIT");
  Am_Register_Slot_Key (Am_OWNER, "OWNER");
  Am_Register_Slot_Key (Am_PROTOTYPE, "PROTOTYPE");
  Am_Register_Slot_Key (Am_SOURCE_OF_COPY, "SOURCE_OF_COPY");
  Am_Register_Slot_Key (Am_LEFT, "LEFT");
  Am_Register_Slot_Key (Am_TOP, "TOP");
  Am_Register_Slot_Key (Am_WIDTH, "WIDTH");
  Am_Register_Slot_Key (Am_HEIGHT, "HEIGHT");
  Am_Register_Slot_Key (Am_WINDOW, "WINDOW");
  Am_Register_Slot_Key (Am_VISIBLE, "VISIBLE");
  Am_Register_Slot_Key (Am_TITLE, "TITLE");
  Am_Register_Slot_Key (Am_ICON_TITLE, "ICON_TITLE");
  Am_Register_Slot_Key (Am_TEXT, "TEXT");
  Am_Register_Slot_Key (Am_CURSOR_INDEX, "CURSOR_INDEX");
  Am_Register_Slot_Key (Am_CURSOR_OFFSET, "CURSOR_OFFSET");
  Am_Register_Slot_Key (Am_INVERT, "INVERT");
  Am_Register_Slot_Key (Am_OFFSCREEN_DRAWONABLE, "~OFFSCREEN_DRAWONABLE~");
  Am_Register_Slot_Key (Am_DOUBLE_BUFFER, "DOUBLE_BUFFER");
  Am_Register_Slot_Key (Am_DESTROY_WINDOW_METHOD, "~DESTROY_WINDOW_METHOD~");
  Am_Register_Slot_Key (Am_POINT_LIST, "POINT_LIST");
  Am_Register_Slot_Key (Am_FILL_STYLE, "FILL_STYLE");
  Am_Register_Slot_Key (Am_LINE_STYLE, "LINE_STYLE");
  Am_Register_Slot_Key (Am_ANGLE1, "ANGLE1");
  Am_Register_Slot_Key (Am_ANGLE2, "ANGLE2");
  Am_Register_Slot_Key (Am_RADIUS, "RADIUS");
  Am_Register_Slot_Key (Am_DRAW_RADIUS, "DRAW_RADIUS");
  Am_Register_Slot_Key (Am_FONT, "FONT");
  Am_Register_Slot_Key (Am_IMAGE, "IMAGE");
  Am_Register_Slot_Key (Am_IS_COLOR, "IS_COLOR");
  Am_Register_Slot_Key (Am_MAX_WIDTH, "MAX_WIDTH");
  Am_Register_Slot_Key (Am_MAX_HEIGHT, "MAX_HEIGHT");
  Am_Register_Slot_Key (Am_MIN_WIDTH, "MIN_WIDTH");
  Am_Register_Slot_Key (Am_MIN_HEIGHT, "MIN_HEIGHT");
  Am_Register_Slot_Key (Am_USE_MAX_WIDTH, "USE_MAX_WIDTH");
  Am_Register_Slot_Key (Am_USE_MAX_HEIGHT, "USE_MAX_HEIGHT");
  Am_Register_Slot_Key (Am_USE_MIN_WIDTH, "USE_MIN_WIDTH");
  Am_Register_Slot_Key (Am_USE_MIN_HEIGHT, "USE_MIN_HEIGHT");
  Am_Register_Slot_Key (Am_ICONIFIED, "ICONIFIED");
  Am_Register_Slot_Key (Am_QUERY_POSITION, "QUERY_POSITION");
  Am_Register_Slot_Key (Am_QUERY_SIZE, "QUERY_SIZE");
  Am_Register_Slot_Key (Am_LEFT_BORDER_WIDTH, "LEFT_BORDER_WIDTH");
  Am_Register_Slot_Key (Am_TOP_BORDER_WIDTH, "TOP_BORDER_WIDTH");
  Am_Register_Slot_Key (Am_RIGHT_BORDER_WIDTH, "RIGHT_BORDER_WIDTH");
  Am_Register_Slot_Key (Am_BOTTOM_BORDER_WIDTH, "BOTTOM_BORDER_WIDTH");
  Am_Register_Slot_Key (Am_CURSOR, "CURSOR");
  Am_Register_Slot_Key (Am_OMIT_TITLE_BAR, "OMIT_TITLE_BAR");
  Am_Register_Slot_Key (Am_SAVE_UNDER, "SAVE_UNDER");
  Am_Register_Slot_Key (Am_CLIP_CHILDREN, "CLIP_CHILDREN");
  Am_Register_Slot_Key (Am_CLIP, "CLIP");
  Am_Register_Slot_Key (Am_GRAPHICAL_PARTS, "GRAPHICAL_PARTS");
  Am_Register_Slot_Key (Am_FLIP_BOOK_PARTS, "FLIP_BOOK_PARTS");
  Am_Register_Slot_Key (Am_PRETEND_TO_BE_LEAF, "PRETEND_TO_BE_LEAF");
  Am_Register_Slot_Key (Am_RANK, "~RANK~");
  Am_Register_Slot_Key (Am_OWNER_DEPTH, "~OWNER_DEPTH~");
  Am_Register_Slot_Key (Am_FADE_DEPTH, "~FADE_DEPTH~");
  Am_Register_Slot_Key (Am_OBJECT_IN_PROGRESS, "~OBJECT_IN_PROGRESS~");
  Am_Register_Slot_Key (Am_X1, "X1");
  Am_Register_Slot_Key (Am_Y1, "Y1");
  Am_Register_Slot_Key (Am_X2, "X2");
  Am_Register_Slot_Key (Am_Y2, "Y2");
  Am_Register_Slot_Key (Am_HEAD_LENGTH, "HEAD_LENGTH");
  Am_Register_Slot_Key (Am_HEAD_WIDTH, "HEAD_WIDTH");
  Am_Register_Slot_Key (Am_TAIL_LENGTH, "TAIL_LENGTH");
  Am_Register_Slot_Key (Am_TAIL_WIDTH, "TAIL_WIDTH");
  Am_Register_Slot_Key (Am_LAYOUT, "LAYOUT");
  Am_Register_Slot_Key (Am_H_SPACING, "H_SPACING");
  Am_Register_Slot_Key (Am_V_SPACING, "V_SPACING");
  Am_Register_Slot_Key (Am_H_ALIGN, "H_ALIGN");
  Am_Register_Slot_Key (Am_V_ALIGN, "V_ALIGN");
  Am_Register_Slot_Key (Am_X_OFFSET, "X_OFFSET");
  Am_Register_Slot_Key (Am_Y_OFFSET, "Y_OFFSET");
  Am_Register_Slot_Key (Am_LEFT_OFFSET, "LEFT_OFFSET");
  Am_Register_Slot_Key (Am_TOP_OFFSET, "TOP_OFFSET");
  Am_Register_Slot_Key (Am_RIGHT_OFFSET, "RIGHT_OFFSET");
  Am_Register_Slot_Key (Am_BOTTOM_OFFSET, "BOTTOM_OFFSET");
  Am_Register_Slot_Key (Am_FIXED_WIDTH, "FIXED_WIDTH");
  Am_Register_Slot_Key (Am_FIXED_HEIGHT, "FIXED_HEIGHT");
  Am_Register_Slot_Key (Am_INDENT, "INDENT");
  Am_Register_Slot_Key (Am_MAX_RANK, "MAX_RANK");
  Am_Register_Slot_Key (Am_MAX_SIZE, "MAX_SIZE");
  Am_Register_Slot_Key (Am_ITEMS, "ITEMS");
  Am_Register_Slot_Key (Am_ITEM, "ITEM");
  Am_Register_Slot_Key (Am_ITEM_METHOD, "ITEM_METHOD");
  Am_Register_Slot_Key (Am_ITEM_PROTOTYPE, "ITEM_PROTOTYPE");
  
  Am_Register_Slot_Key (Am_DRAW_METHOD, "~DRAW_METHOD~");
  Am_Register_Slot_Key (Am_MASK_METHOD, "~MASK_METHOD~");
  Am_Register_Slot_Key (Am_DRAWONABLE, "~DRAWONABLE~");
  Am_Register_Slot_Key (Am_DRAW_BUFFER, "~DRAW_BUFFER~");
  Am_Register_Slot_Key (Am_MASK_BUFFER, "~MASK_BUFFER~");
  Am_Register_Slot_Key (Am_SCREEN, "~SCREEN~");
  Am_Register_Slot_Key (Am_TODO, "~TODO~");
  Am_Register_Slot_Key (Am_INVALID_METHOD, "~INVALID_METHOD~");
  Am_Register_Slot_Key (Am_PREV_STATE, "~PREV_STATE~");
  Am_Register_Slot_Key (Am_POINT_IN_OBJ_METHOD, "~POINT_IN_OBJ_METHOD~");
  Am_Register_Slot_Key (Am_POINT_IN_PART_METHOD, "~POINT_IN_PART_METHOD~");
  Am_Register_Slot_Key (Am_POINT_IN_LEAF_METHOD, "~POINT_IN_LEAF_METHOD~");
  Am_Register_Slot_Key (Am_TRANSLATE_COORDINATES_METHOD,
			"~TRANSLATE_COORDINATES_METHOD~");
  Am_Register_Slot_Key (Am_INIT_WANT_ENTER_LEAVE,
			"~INIT_WANT_ENTER_LEAVE~");
  Am_Register_Slot_Key (Am_INIT_WANT_MULTI_WINDOW,
			"~INIT_WANT_MULTI_WINDOW~");
  Am_Register_Slot_Key (Am_OLD_WIDTH, "~OLD_WIDTH~");
  Am_Register_Slot_Key (Am_OLD_HEIGHT, "~OLD_HEIGHT~");
  Am_Register_Slot_Key (Am_WAITING_FOR_COMPLETION, "~WAITING_FOR_COMPLETION~");
  Am_Register_Slot_Key (Am_COMPLETION_VALUE, "~COMPLETION_VALUE~");
  Am_Register_Slot_Key (Am_SELECT_OUTLINE_ONLY, "SELECT_OUTLINE_ONLY");
  Am_Register_Slot_Key (Am_SELECT_FULL_INTERIOR, "SELECT_FULL_INTERIOR");
  Am_Register_Slot_Key (Am_HIT_THRESHOLD, "HIT_THRESHOLD");

  Am_Register_Slot_Key (Am_SAVE_OBJECT_METHOD, "SAVE_OBJECT_METHOD");
  Am_Register_Slot_Key (Am_SLOTS_TO_SAVE, "SLOTS_TO_SAVE");
  Am_Register_Slot_Key (Am_DRAW_MONOCHROME, "DRAW_MONOCHROME");
#endif
}

void Am_Initialize ()
{
  Ore_Initialize();
  // make sure all styles are initialized first
  // and register their names
  
  Am_Red = Am_Style(1.0f, 0.0f, 0.0f);
  Am_Green = Am_Style(0.0f, 1.0f, 0.0f);
  Am_Blue = Am_Style(0.0f, 0.0f, 1.0f);
  Am_Yellow = Am_Style(1.0f, 1.0f, 0.0f);
  Am_Purple = Am_Style(1.0f, 0.0f, 1.0f);
  Am_Cyan = Am_Style(0.0f, 1.0f, 1.0f);
  Am_Orange = Am_Style(0.75f, 0.25f, 0.0f);
  Am_Black = Am_Style(0.0f, 0.0f, 0.0f);
  Am_White = Am_Style(1.0f, 1.0f, 1.0f);
  Am_Amulet_Purple = Am_Style (1.0f, 0.75, 1.0f);
  // V2: (0.96f, 0.86f, 1.0f);
  // darker purple (too dark) (0.9023f, 0.73828f, 1.0f);


  Am_Motif_Gray = Am_Style(0.83f, 0.83f, 0.83f); // (0xd3d3 / 0xffff)
  Am_Motif_Light_Gray = Am_Style(0.9f, 0.9f, 0.9f);
  Am_Motif_Blue = Am_Style(0.45f, // (0x7272 / 0xffff)
			               0.62f,  // (0x9f9f / 0xffff)
			               1.0f);
  Am_Motif_Light_Blue = Am_Style(0.7217459f, 0.8998047f, 1.0f);
  Am_Motif_Green = Am_Style(0.37f, // (0x5f5f / 0xffff)
                            0.62f, // (0x9e9e / 0xffff)
                            0.63f); //(0xa0a0 / 0xffff))
  Am_Motif_Light_Green = Am_Style(0.62f, 0.87f, 0.7f);
  Am_Motif_Orange = Am_Style(1.0f, 0.6f, 0.4f);
  Am_Motif_Light_Orange = Am_Style(1.0f, 0.91f, 0.72f);
  
  Am_Thin_Line = Am_Black;
  Am_Line_0 = Am_Black;
  Am_Line_1 = Am_Black;
  Am_Line_2 = Am_Style(0.0f, 0.0f, 0.0f, 2);
  Am_Line_4 = Am_Style(0.0f, 0.0f, 0.0f, 4);
  Am_Line_8 = Am_Style(0.0f, 0.0f, 0.0f, 8);
  Am_Dashed_Line = Am_Style(0.0f, 0.0f, 0.0f, 1, Am_CAP_BUTT, Am_JOIN_MITER,
			    Am_LINE_ON_OFF_DASH);

  static const char Am_DOTTED_DASH_LIST[2] = {2, 2};
  Am_Dotted_Line = Am_Style(0.0f, 0.0f, 0.0f, 1, Am_CAP_BUTT, Am_JOIN_MITER,
			    Am_LINE_ON_OFF_DASH, Am_DOTTED_DASH_LIST, 2);
  
  Am_Gray_Stipple = Am_Style::Halftone_Stipple (50);
  Am_Opaque_Gray_Stipple = Am_Style::Halftone_Stipple (50,
				       Am_FILL_OPAQUE_STIPPLED);
  Am_Light_Gray_Stipple = Am_Style::Halftone_Stipple (25);
  Am_Dark_Gray_Stipple = Am_Style::Halftone_Stipple (75);
#if !defined(_WINDOWS)
    Am_Diamond_Stipple = Am_Style (0.0f, 0.0f, 0.0f, 0, Am_CAP_BUTT,
			 Am_JOIN_MITER, Am_LINE_ON_OFF_DASH,
			 Am_DEFAULT_DASH_LIST, Am_DEFAULT_DASH_LIST_LENGTH,
			 Am_FILL_STIPPLED, Am_FILL_POLY_EVEN_ODD,
			 (Am_Image_Array (diamond_bits, 16, 16)));
  Am_Opaque_Diamond_Stipple = Am_Style (0.0f, 0.0f, 0.0f, 0, Am_CAP_BUTT,
			    Am_JOIN_MITER, Am_LINE_ON_OFF_DASH,
			    Am_DEFAULT_DASH_LIST, Am_DEFAULT_DASH_LIST_LENGTH,
			    Am_FILL_OPAQUE_STIPPLED, Am_FILL_POLY_EVEN_ODD,
			    (Am_Image_Array (diamond_bits, 16, 16)));
#else
  Am_Diamond_Stipple = Am_Style (0.0f, 0.0f, 0.0f, 0, Am_CAP_BUTT,
			    Am_JOIN_MITER, Am_LINE_ON_OFF_DASH,
			    Am_DEFAULT_DASH_LIST, Am_DEFAULT_DASH_LIST_LENGTH,
			    Am_FILL_STIPPLED, Am_FILL_POLY_EVEN_ODD,
			    (Am_Image_Array ((char*)diamond_bits, 8, 8)));
  Am_Opaque_Diamond_Stipple = Am_Style (0.0f, 0.0f, 0.0f, 0, Am_CAP_BUTT,
			    Am_JOIN_MITER, Am_LINE_ON_OFF_DASH,
			    Am_DEFAULT_DASH_LIST, Am_DEFAULT_DASH_LIST_LENGTH,
			    Am_FILL_OPAQUE_STIPPLED, Am_FILL_POLY_EVEN_ODD,
			    (Am_Image_Array ((char*)diamond_bits, 8, 8)));
#endif
  
  // Register style names.
  Am_Register_Name (Am_Red, "Am_Red");
  Am_Register_Name (Am_Green, "Am_Green");
  Am_Register_Name (Am_Blue, "Am_Blue");
  Am_Register_Name (Am_Yellow, "Am_Yellow");
  Am_Register_Name (Am_Purple, "Am_Purple");
  Am_Register_Name (Am_Cyan, "Am_Cyan");
  Am_Register_Name (Am_Orange, "Am_Orange");
  Am_Register_Name (Am_White, "Am_White");
  Am_Register_Name (Am_Amulet_Purple, "Am_Amulet_Purple");
  Am_Register_Name (Am_Motif_Gray, "Am_Motif_Gray");
	
  Am_Register_Name (Am_Motif_Light_Gray, "Am_Motif_Light_Gray");
  Am_Register_Name (Am_Motif_Blue, "Am_Motif_Blue");
  Am_Register_Name (Am_Motif_Light_Blue, "Am_Motif_Light_Blue");
  Am_Register_Name (Am_Motif_Green, "Am_Motif_Green");
  Am_Register_Name (Am_Motif_Light_Green, "Am_Motif_Light_Green");
  Am_Register_Name (Am_Motif_Orange, "Am_Motif_Orange");
  Am_Register_Name (Am_Motif_Light_Orange, "Am_Motif_Light_Orange");

  Am_Register_Name (Am_Line_2, "Am_Line_2");
  Am_Register_Name (Am_Line_4, "Am_Line_4");
  Am_Register_Name (Am_Line_8, "Am_Line_8");
  Am_Register_Name (Am_Dashed_Line, "Am_Dashed_Line");
  Am_Register_Name (Am_Dotted_Line, "Am_Dotted_Line");
  Am_Register_Name (Am_Gray_Stipple, "Am_Gray_Stipple");
  Am_Register_Name (Am_Opaque_Gray_Stipple, "Am_Opaque_Gray_Stipple");
  Am_Register_Name (Am_Light_Gray_Stipple, "Am_Light_Gray_Stipple");
  Am_Register_Name (Am_Dark_Gray_Stipple, "Am_Dark_Gray_Stipple");
  Am_Register_Name (Am_Diamond_Stipple, "  Am_Diamond_Stipple");
  Am_Register_Name (Am_Opaque_Diamond_Stipple, "Am_Opaque_Diamond_Stipple");
  
  // these all conflict with Am_Black.  When we print the name out,
  // it will be "Am_Black," but we register them all so you can specify any
  // of them in the inspector.
  Am_Register_Name (Am_Thin_Line, "Am_Thin_Line");
  Am_Register_Name (Am_Line_0, "Am_Line_0");
  Am_Register_Name (Am_Line_1, "Am_Line_1");
  Am_Register_Name (Am_Black, "Am_Black");

  Am_Register_Name (Am_Default_Font, "Am_Default_Font");

  set_slot_names ();
  
  Am_Object_Advanced temp;
  Am_Demon_Set demons;

  // Initialize formulas for use in graphical objects

  Am_Initialize_Aux ();  // Initialize Am_Window and Am_Graphical_Object
  
  Am_Web xweb (line_x_create, line_x_init, line_x_validate);
  Am_Web yweb (line_y_create, line_y_init, line_y_validate);

  Am_Line = Am_Graphical_Object.Create ("Am_Line")
    .Set (Am_AS_LINE, true)
    .Add (Am_LINE_STYLE, Am_Black)
    .Set (Am_DRAW_METHOD, line_draw)
    .Set (Am_MASK_METHOD, line_mask)
    .Add (Am_X1, 0) // initialize all the values to first
    .Add (Am_Y1, 0)
    .Add (Am_X2, 0)
    .Add (Am_Y2, 0)
    .Set (Am_LEFT, 0)
    .Set (Am_TOP, 0)
    .Set (Am_WIDTH, 1)
    .Set (Am_HEIGHT, 1)
    .Add (Am_HIT_THRESHOLD, 0)
    .Set (Am_POINT_IN_OBJ_METHOD, line_point_in_obj)
    .Set (Am_X1, xweb)
    .Set (Am_Y1, yweb)
    ;

  temp = (Am_Object_Advanced&)Am_Line;
  temp.Get_Slot (Am_LINE_STYLE).Set_Demon_Bits (Am_MOVING_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_X1).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_Y1).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_X2).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_Y2).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);

  Am_Arrow_Line = Am_Line.Create ("Am_Arrow_Line")
    .Add (Am_HEAD_LENGTH, 5)
    .Add (Am_HEAD_WIDTH, 3)
    .Set (Am_DRAW_METHOD, arrow_line_draw)
    .Set (Am_MASK_METHOD, arrow_line_mask)
  ;

  temp = (Am_Object_Advanced&)Am_Arrow_Line;
  temp.Get_Slot (Am_HEAD_WIDTH).Set_Demon_Bits (Am_MOVING_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_HEAD_LENGTH).Set_Demon_Bits (Am_MOVING_REDRAW |
						 Am_EAGER_DEMON);
  temp.Disinherit_Slot (Am_X1); // arrow doesn't work like regular lines
  temp.Disinherit_Slot (Am_Y1); // so we have to break the connection
  temp.Disinherit_Slot (Am_X2); // (would be better to make from scratch)
  temp.Disinherit_Slot (Am_Y2);
  temp.Disinherit_Slot (Am_LEFT);
  temp.Disinherit_Slot (Am_TOP);
  temp.Disinherit_Slot (Am_WIDTH);
  temp.Disinherit_Slot (Am_HEIGHT);

  Am_Web arrow_xweb (line_x_create, arrow_line_x_init, arrow_line_x_validate);
  Am_Web arrow_yweb (line_y_create, arrow_line_y_init, arrow_line_y_validate);

  Am_Arrow_Line
    .Add (Am_X1, 0) // reset positioning slot to correspond to arrow lines
    .Add (Am_Y1, 0)
    .Add (Am_X2, 0)
    .Add (Am_Y2, 0)
    .Add (Am_LEFT, -5)
    .Add (Am_TOP, -5)
    .Add (Am_WIDTH, 5)
    .Add (Am_HEIGHT, 5)
    .Set (Am_X1, arrow_xweb)
    .Set (Am_Y1, arrow_yweb)
  ;

  Am_Rectangle = Am_Graphical_Object.Create ("Am_Rectangle")
    .Add (Am_FILL_STYLE, Am_Black)
    .Add (Am_LINE_STYLE, Am_Black)
    .Set (Am_DRAW_METHOD, rectangle_draw)
    .Set (Am_MASK_METHOD, rectangle_mask);
  temp = (Am_Object_Advanced&)Am_Rectangle;
  temp.Get_Slot (Am_FILL_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_LINE_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);

  Am_Arc = Am_Graphical_Object.Create ("Am_Arc")
    .Add (Am_ANGLE1, 0)
    .Add (Am_ANGLE2, 360)
    .Add (Am_FILL_STYLE, Am_Black)
    .Add (Am_LINE_STYLE, Am_Black)
    .Set (Am_DRAW_METHOD, arc_draw)
    .Set (Am_MASK_METHOD, arc_mask);
  temp = (Am_Object_Advanced&)Am_Arc;
  temp.Get_Slot (Am_ANGLE1).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_ANGLE2).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_FILL_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_LINE_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);

  Am_Roundtangle = Am_Graphical_Object.Create ("Am_Roundtangle")
    .Add (Am_RADIUS, Am_SMALL_RADIUS)
    .Add (Am_DRAW_RADIUS, compute_draw_radius)
    .Add (Am_FILL_STYLE, Am_Black)
    .Add (Am_LINE_STYLE, Am_Black)
    .Set (Am_DRAW_METHOD, roundtangle_draw)
    .Set (Am_MASK_METHOD, roundtangle_mask);
  temp = (Am_Object_Advanced&)Am_Roundtangle;
  // Maybe only need to register one of Am_RADIUS or Am_DRAW_RADIUS with
  // Demon since Am_DRAW_RADIUS formula depends on Am_RADIUS?
//// QUESTION: Does changing the radius potentially change the bounding box?
//// I think not.
  temp.Get_Slot (Am_RADIUS).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_DRAW_RADIUS).Set_Demon_Bits (Am_MOVING_REDRAW |
						 Am_EAGER_DEMON);
  temp.Get_Slot (Am_FILL_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_LINE_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);

  Am_Point_List empty_list;
  
  Am_Web polyweb (polygon_web_create, polygon_web_init, polygon_web_validate);
  Am_Polygon = Am_Graphical_Object.Create ("Am_Polygon")
    .Add (Am_LINE_STYLE, Am_Black)
    .Add (Am_FILL_STYLE, Am_Black)
    .Set (Am_DRAW_METHOD, polygon_draw)
    .Set (Am_MASK_METHOD, polygon_mask)
    .Set (Am_WIDTH, 0)   // initialize webbed slots to 0 first
    .Set (Am_HEIGHT, 0)
    .Set (Am_TOP, 0)
    .Set (Am_LEFT, 0)
    .Add (Am_POINT_LIST, empty_list)
    .Add (Am_HIT_THRESHOLD, 0)
    .Add (Am_SELECT_OUTLINE_ONLY, 0)
    .Add (Am_SELECT_FULL_INTERIOR, 0)
    .Set (Am_POINT_IN_OBJ_METHOD, polygon_point_in_obj)
    .Set (Am_LEFT, polyweb)
    ;

  temp = (Am_Object_Advanced&)Am_Polygon;
  temp.Get_Slot (Am_FILL_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_LINE_STYLE).Set_Demon_Bits (Am_MOVING_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_POINT_LIST).Set_Demon_Bits (Am_MOVING_REDRAW |
						Am_EAGER_DEMON);
  Am_Text = Am_Graphical_Object.Create ("Am_Text")
    .Add (Am_FONT, Am_Default_Font)
    .Add (Am_TEXT, "")
    //width and height can be set bigger or smaller than calculated
    // will scroll if smaller.
    .Set (Am_WIDTH, compute_string_width)
    .Set (Am_HEIGHT, compute_string_height)
    .Add (Am_CURSOR_INDEX, Am_NO_CURSOR)
    // X_offset allows the string to be scrolled left and right.  The
    // default formula makes sure the cursor index is visible.
    .Add (Am_X_OFFSET, 0) //initial value is 0
    .Set (Am_X_OFFSET, compute_string_offset)
    .Add (Am_CURSOR_OFFSET, compute_cursor_offset)
    .Add (Am_LINE_STYLE, Am_Line_2)
    .Add (Am_FILL_STYLE, Am_No_Style)
    .Add (Am_INVERT, false) // invert foreground/ background styles of text?
    .Add (Am_PENDING_DELETE, false)
    .Set (Am_DRAW_METHOD, text_draw)
	.Set (Am_MASK_METHOD, text_mask)
    ;
  temp = (Am_Object_Advanced&)Am_Text;
  temp.Get_Slot (Am_TEXT).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_FONT).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_CURSOR_INDEX).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						  Am_EAGER_DEMON);
  temp.Get_Slot (Am_LINE_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_FILL_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_INVERT).Set_Demon_Bits (Am_STATIONARY_REDRAW |
					    Am_EAGER_DEMON);

  Am_Hidden_Text = Am_Text.Create("Am_Hidden_Text")
    .Add(Am_START_CHAR, '*')
    .Set(Am_CURSOR_OFFSET, compute_cursor_offset_for_hidden)
    .Set(Am_DRAW_METHOD, hidden_text_draw);
  temp = (Am_Object_Advanced&)Am_Hidden_Text;
  temp.Get_Slot (Am_START_CHAR)
    .Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);

  Am_Bitmap = Am_Graphical_Object.Create ("Am_Bitmap")
    .Add (Am_LINE_STYLE, Am_Black)
    .Add (Am_FILL_STYLE, Am_No_Style)
    .Add (Am_DRAW_MONOCHROME, false)
    .Add (Am_IMAGE, Am_No_Image)
    .Set (Am_WIDTH, compute_bitmap_width)
    .Set (Am_HEIGHT, compute_bitmap_height)
    //width and height can be set bigger or smaller than calculated
    // will clip if smaller and Am_H_ALIGN and Am_V_ALIGN if bigger
//    .Set (Am_H_ALIGN, Am_LEFT_ALIGN)
//    .Set (Am_V_ALIGN, Am_TOP_ALIGN)
    .Set (Am_DRAW_METHOD, bitmap_draw)
    .Set (Am_MASK_METHOD, bitmap_mask);
  temp = (Am_Object_Advanced&)Am_Bitmap;
  temp.Get_Slot (Am_IMAGE).Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  temp.Get_Slot (Am_LINE_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_FILL_STYLE).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);
  temp.Get_Slot (Am_DRAW_MONOCHROME).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						Am_EAGER_DEMON);

  Am_Aggregate = Am_Graphical_Object.Create ("Am_Aggregate")
    .Set (Am_DRAW_METHOD, aggregate_draw)
    .Set (Am_MASK_METHOD, aggregate_mask)
    .Set (Am_INVALID_METHOD, aggregate_invalid)
    .Add (Am_GRAPHICAL_PARTS, Am_Value_List ())
    .Set (Am_POINT_IN_PART_METHOD, am_group_point_in_part)
    .Set (Am_POINT_IN_LEAF_METHOD, am_group_point_in_leaf)
    .Add (Am_LAYOUT, 0)
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
    .Add (Am_MAX_SIZE, false);

  Am_Group = Am_Aggregate.Create ("Am_Group");
  temp = (Am_Object_Advanced&)Am_Group;
  demons = temp.Get_Demons ().Copy ();
  demons.Set_Object_Demon (Am_CREATE_OBJ, group_create);
  demons.Set_Object_Demon (Am_COPY_OBJ, group_copy);
  demons.Set_Part_Demon (Am_ADD_PART, am_generic_add_part);
  temp.Set_Demons (demons);

  Am_Fade_Group = Am_Group.Create ("Am_Fade_Group")
    .Add (Am_FADE_DEPTH, am_fade_depth)
    .Add (Am_VALUE, 100)
    .Set (Am_DRAW_METHOD, fade_group_draw)
    .Set (Am_MASK_METHOD, fade_group_mask)
  ;
  ((Am_Object_Advanced&)Am_Fade_Group).Get_Slot (Am_VALUE).
      Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);

  Am_Flip_Book_Group = Am_Aggregate.Create ("Am_Flip_Book_Group")
    .Set (Am_GRAPHICAL_PARTS, Am_No_Value_List)
    .Add (Am_FLIP_BOOK_PARTS, Am_No_Value_List)
    .Add (Am_VALUE, 0)
    .Set (Am_GRAPHICAL_PARTS, flip_book_parts);
  temp = (Am_Object_Advanced&)Am_Flip_Book_Group;
  demons = temp.Get_Demons ().Copy ();
  demons.Set_Object_Demon (Am_CREATE_OBJ, flip_book_create);
  demons.Set_Object_Demon (Am_COPY_OBJ, flip_book_copy);
  demons.Set_Part_Demon (Am_ADD_PART, flip_book_add_part);
  temp.Get_Slot (Am_GRAPHICAL_PARTS).Set_Demon_Bits (Am_STATIONARY_REDRAW |
						     Am_EAGER_DEMON);
  temp.Set_Demons (demons);
  
  Am_Resize_Parts_Group = Am_Group.Create("Am_Resize_Parts_Group")
    .Add(Am_OLD_WIDTH, 0)
    .Add(Am_OLD_HEIGHT, 0)
    .Add (Am_SAVE_OBJECT_METHOD, Am_Standard_Save_Group)
    .Add (Am_SLOTS_TO_SAVE, Am_Value_List()
	  .Add(Am_LEFT).Add(Am_TOP).Add(Am_WIDTH).Add(Am_HEIGHT)
	  .Add(Am_CREATED_GROUP))
    ;
  temp = (Am_Object_Advanced&)Am_Resize_Parts_Group;
  demons = temp.Get_Demons ().Copy ();
  demons.Set_Slot_Demon (Am_GROUP_RESIZE_PARTS, resize_group_parts,
			 Am_DEMON_PER_OBJECT | Am_DEMON_ON_CHANGE);
  temp.Set_Demons (demons);
  Am_Slot slot = temp.Get_Slot (Am_WIDTH);
  unsigned short prev_demon_bits = slot.Get_Demon_Bits();
  slot.Set_Demon_Bits(prev_demon_bits | Am_GROUP_RESIZE_PARTS);
  slot = temp.Get_Slot (Am_HEIGHT);
  prev_demon_bits = slot.Get_Demon_Bits();
  slot.Set_Demon_Bits(prev_demon_bits | Am_GROUP_RESIZE_PARTS);

  Am_Map = Am_Aggregate.Create ("Am_Map")
    .Set (Am_WIDTH, Am_Width_Of_Parts)
    .Set (Am_HEIGHT, Am_Height_Of_Parts)
    .Set (Am_GRAPHICAL_PARTS, map_make_components)
    .Add (Am_ITEMS, 0)
    .Add (Am_ITEM_METHOD, Am_Standard_Item_Method)
    .Add (Am_ITEM_PROTOTYPE, 0)
    ;
  temp = (Am_Object_Advanced&)Am_Map;
  temp.Get_Slot (Am_GRAPHICAL_PARTS).Set_Demon_Bits (Am_MOVING_REDRAW |
						     Am_EAGER_DEMON);

  // Default Load and Save Methods
  Am_Default_Load_Save_Context.Register_Prototype("Am_RESIZE_PARTS_GROUP",
						  Am_Resize_Parts_Group);

  Am_Default_Load_Save_Context.Register_Loader ("Am_NONE", load_none);
  Am_Default_Load_Save_Context.Register_Saver (Am_NONE, save_none);
  Am_Default_Load_Save_Context.Register_Loader ("Am_INT", load_int);
  Am_Default_Load_Save_Context.Register_Saver (Am_INT, save_int);
  Am_Default_Load_Save_Context.Register_Loader ("Am_LONG", load_long);
  Am_Default_Load_Save_Context.Register_Saver (Am_LONG, save_long);
  Am_Default_Load_Save_Context.Register_Loader ("Am_BOOL", load_bool);
  Am_Default_Load_Save_Context.Register_Saver (Am_BOOL, save_bool);
  Am_Default_Load_Save_Context.Register_Loader ("Am_FLOAT", load_float);
  Am_Default_Load_Save_Context.Register_Saver (Am_FLOAT, save_float);
  Am_Default_Load_Save_Context.Register_Loader ("Am_DOUBLE", load_double);
  Am_Default_Load_Save_Context.Register_Saver (Am_DOUBLE, save_double);
  Am_Default_Load_Save_Context.Register_Loader ("Am_CHAR", load_char);
  Am_Default_Load_Save_Context.Register_Saver (Am_CHAR, save_char);
  Am_Default_Load_Save_Context.Register_Loader ("Am_STRING", load_string);
  Am_Default_Load_Save_Context.Register_Saver (Am_STRING, save_string);
  Am_Default_Load_Save_Context.Register_Loader ("Am_VALUE_LIST", load_list);
  Am_Default_Load_Save_Context.Register_Saver (
                               Am_Value_List::Type_ID (), save_list);
  Am_Default_Load_Save_Context.Register_Saver (Am_OBJECT, save_object);
  Am_Default_Load_Save_Context.Register_Loader ("Am_STANDARD_SAVE",
						Am_Standard_Load_Object);
  Am_Default_Load_Save_Context.Register_Loader ("Am_STANDARD_SAVE_GROUP",
						Am_Standard_Load_Group);
  Am_Default_Load_Save_Context.Register_Loader ("Am_POINT_LIST",
						load_point_list);
  Am_Default_Load_Save_Context.Register_Saver (
                               Am_Point_List::Type_ID (), save_point_list);

  Am_Load_Object_Swap = Am_Root_Object.Create ("Am_Load_Object_Swap")
    .Add (Am_ITEM_PROTOTYPE, Am_No_Object)
    .Add (Am_SLOTS_TO_SAVE, Am_No_Value_List)
  ;

  Am_Default_Load_Save_Context.Register_Prototype ("Am_Red", Am_Red);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Green", Am_Green);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Blue", Am_Blue);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Yellow", Am_Yellow);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Purple", Am_Purple);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Cyan", Am_Cyan);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Orange", Am_Orange);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Black", Am_Black);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_White", Am_White);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Amulet_Purple",
						   Am_Amulet_Purple);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Motif_Gray",
						   Am_Motif_Gray);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Motif_Light_Gray",
						   Am_Motif_Light_Gray);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Motif_Blue",
						   Am_Motif_Blue);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Motif_Light_Blue",
						   Am_Motif_Light_Blue);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Motif_Green",
						   Am_Motif_Green);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Motif_Light_Green",
						   Am_Motif_Light_Green);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Motif_Orange",
						   Am_Motif_Orange);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Motif_Light_Orange",
						   Am_Motif_Light_Orange);
  // these all conflict with Am_Black
  //Am_Default_Load_Save_Context.Register_Prototype ("Am_Thin_Line",
  //						   Am_Thin_Line);
  //Am_Default_Load_Save_Context.Register_Prototype ("Am_Line_0", Am_Line_0);
  //Am_Default_Load_Save_Context.Register_Prototype ("Am_Line_1", Am_Line_1);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Line_2", Am_Line_2);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Line_4", Am_Line_4);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Line_8", Am_Line_8);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Dotted_Line",
						   Am_Dotted_Line);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Dashed_Line",
						   Am_Dashed_Line);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Gray_Stipple",
						   Am_Gray_Stipple);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Opaque_Gray_Stipple",
						   Am_Opaque_Gray_Stipple);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Light_Gray_Stipple",
						   Am_Light_Gray_Stipple);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Dark_Gray_Stipple",
						   Am_Dark_Gray_Stipple);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Diamond_Stipple",
						   Am_Diamond_Stipple);
  Am_Default_Load_Save_Context.Register_Prototype ("Am_Opaque_Diamond_Stipple",
						   Am_Opaque_Diamond_Stipple);

  Am_Initialize_Interactors (); // Initialize the Interactors (from inter.h)
  Am_Widgets_Initialize ();
  Am_Gesture_Initialize ();
#ifdef DEBUG
  Am_Initialize_Inspector();
#endif
  Main_Demon_Queue.Invoke();
}

void Am_Cleanup ()
{
  Main_Demon_Queue.Shutdown ();
  Am_State_Store::Shutdown ();
  // Is this a hack?
#ifdef DEBUG
  Am_Global_Slot_Trace_Proc = NULL;
#endif
  //
  Am_Line.Destroy ();
  Am_Rectangle.Destroy ();
  Am_Arc.Destroy ();
  Am_Text.Destroy ();
  Am_Roundtangle.Destroy ();
  Am_Bitmap.Destroy ();
  Am_Group.Destroy ();
  Am_Map.Destroy ();
  Am_Aggregate.Destroy ();
  Am_Window.Destroy ();
  Am_Screen.Destroy ();
//// NDY: Make sure these are released -- in order -- once defined.
//  Am_Polygon.Destroy ();
  Am_Graphical_Object.Destroy ();

  Am_Interactor.Destroy ();
  Am_Command.Destroy ();
  Am_Undo_Handler.Destroy ();

  Am_Cleanup_Animated_Constraints ();
//// NDY: Other objects need to be destroyed?
  Am_Root_Object.Destroy();
}

bool Am_Point_In_All_Owners(const Am_Object& in_obj, int x, int y,
			    const Am_Object& ref_obj)
{
  Am_Object owner, result;
  if (in_obj.Is_Instance_Of(Am_Screen)) return true;
  if (!in_obj.Is_Part_Of (ref_obj)) return false;
     // in_obj not actually part of system
  if (in_obj == ref_obj) return true;
  owner = in_obj;
  //check all owners all the way up to window
  do {
    owner = owner.Get_Owner();
    //if (!(bool)owner.Get(Am_VISIBLE)) return false;
    // checking visible should be handled in Am_Point_In_Obj
    result = Am_Point_In_Obj(owner, x, y, ref_obj);
    if (!result.Valid()) return false;
  }
  while (owner != ref_obj);
  return true;  // owner reached top of hierarchy (ref_obj)
}

Am_Object Am_Point_In_Obj (const Am_Object& in_obj, int x, int y,
			   const Am_Object& ref_obj)
{
  Am_Point_In_Method method;
  method = in_obj.Get(Am_POINT_IN_OBJ_METHOD);
  return method.Call (in_obj, x, y, ref_obj);
}

Am_Object Am_Point_In_Part (const Am_Object& in_obj, int x, int y,
			    const Am_Object& ref_obj, bool want_self,
			    bool want_groups)
{
  Am_Point_In_Or_Self_Method method;
  method = in_obj.Get(Am_POINT_IN_PART_METHOD);
  return method.Call (in_obj, x, y, ref_obj, want_self, want_groups);
}

Am_Object Am_Point_In_Leaf (const Am_Object& in_obj, int x, int y,
			    const Am_Object& ref_obj,
			    bool want_self, bool want_groups)
{ 
  Am_Point_In_Or_Self_Method method;
  method = in_obj.Get(Am_POINT_IN_LEAF_METHOD);
  return method.Call (in_obj, x, y, ref_obj, want_self, want_groups);
}

static Am_Drawonable* get_safe_drawonable(const Am_Object& screen) {
  Am_Object used_screen;
  if (screen.Valid ()) used_screen = screen;
  else used_screen = Am_Screen;

  Am_Drawonable* drawonable =
        Am_Drawonable::Narrow (used_screen.Get (Am_DRAWONABLE, Am_NO_DEPENDENCY));
  if (!drawonable) 
    drawonable = Am_Drawonable::Narrow (Am_Screen.Get (Am_DRAWONABLE, Am_NO_DEPENDENCY));
  return drawonable;
}

void Am_Get_String_Extents (const Am_Object& screen, const Am_Font& font,
			    const char* string, int length,
			    int& width, int& height)
{
  Am_Drawonable* drawonable = get_safe_drawonable(screen);
  int ascent, descent, a, b;
  drawonable->Get_String_Extents (font, string,
				  (length == -1) ? strlen (string) : length,
				  width, ascent, descent, a, b);
  height = ascent + descent;
}

int Am_Get_String_Width (const Am_Object& screen, const Am_Font& font,
			 const char* string, int length)
{
  Am_Drawonable* drawonable = get_safe_drawonable(screen);
  return drawonable->Get_String_Width (font, string,
				  (length == -1) ? strlen (string) : length);
}

void Am_Get_Font_Properties (const Am_Object& screen, const Am_Font& font,
			     int& max_char_width, int& min_char_width,
			     int& max_char_ascent, int& max_char_descent)
{
  Am_Drawonable* drawonable = get_safe_drawonable(screen);
  drawonable->Get_Font_Properties (font, max_char_width, min_char_width,
				   max_char_ascent, max_char_descent);
}

bool Am_Test_Image_File (const Am_Object& screen, const Am_Image_Array& image)
{
  if (!screen.Valid ())
    return false;
  Am_Drawonable* drawonable =
        Am_Drawonable::Narrow (screen.Get (Am_DRAWONABLE));
  return drawonable->Test_Image (image);
}
			      
Am_Object Am_GV_Screen (const Am_Object& self)
{
  Am_Value v;
  Am_Object screen;
  v = self.Peek(Am_WINDOW);
  if (v.Valid()) {
    screen = v;
    for ( ;
	 screen.Valid () && !screen.Is_Instance_Of (Am_Screen);
	 screen = screen.Get_Owner()) ;
  }
  if (!screen.Valid()) return Am_Screen;
  else return screen;
}
