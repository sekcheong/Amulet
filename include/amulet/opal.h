/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifndef OPAL_H
#define OPAL_H

#include <am_inc.h>

#include GDEFS__H
#include FORMULA__H

extern Am_Style Am_Red;
extern Am_Style Am_Green;
extern Am_Style Am_Blue;
extern Am_Style Am_Yellow;
extern Am_Style Am_Purple;
extern Am_Style Am_Cyan;
extern Am_Style Am_Orange;
extern Am_Style Am_Black;
extern Am_Style Am_White;
extern Am_Style Am_Amulet_Purple;

extern Am_Style Am_Motif_Gray;
extern Am_Style Am_Motif_Light_Gray;
extern Am_Style Am_Motif_Blue;
extern Am_Style Am_Motif_Light_Blue;
extern Am_Style Am_Motif_Green;
extern Am_Style Am_Motif_Light_Green;
extern Am_Style Am_Motif_Orange;
extern Am_Style Am_Motif_Light_Orange;

extern Am_Style Am_Thin_Line;
extern Am_Style Am_Line_0;
extern Am_Style Am_Line_1;
extern Am_Style Am_Line_2;
extern Am_Style Am_Line_4;
extern Am_Style Am_Line_8;
extern Am_Style Am_Dotted_Line;
extern Am_Style Am_Dashed_Line;

extern Am_Style Am_Gray_Stipple;
extern Am_Style Am_Opaque_Gray_Stipple;
extern Am_Style Am_Light_Gray_Stipple;
extern Am_Style Am_Dark_Gray_Stipple;
extern Am_Style Am_Diamond_Stipple;
extern Am_Style Am_Opaque_Diamond_Stipple;

// Color conversion routines.  RGB to HSV and HSV to RGB
extern void Am_HSV_To_RGB (float h, float s, float v,
			   float& r, float& g, float& b);
extern void Am_RGB_To_HSV (float r, float g, float b,
			   float& h, float& s, float& v);

extern Am_Font Am_Default_Font;
extern Am_Font Am_Japanese_Font;  // Japanese standard font

extern Am_Object Am_Screen;

extern Am_Object Am_Graphical_Object;
extern Am_Object Am_Window;
extern Am_Object Am_Rectangle;
extern Am_Object Am_Roundtangle;
extern Am_Object Am_Line;
extern Am_Object Am_Arrow_Line;
extern Am_Object Am_Polygon;
extern Am_Object Am_Arc;
extern Am_Object Am_Text;
extern Am_Object Am_Hidden_Text; //for passwords
extern Am_Object Am_Bitmap;

extern Am_Object Am_Group;
extern Am_Object Am_Map;
extern Am_Object Am_Resize_Parts_Group;
extern Am_Object Am_Fade_Group;
extern Am_Object Am_Flip_Book_Group;

// Store in Group or Map's Am_LAYOUT slot for automatic layout.
extern Am_Formula Am_Horizontal_Layout;
extern Am_Formula Am_Vertical_Layout;

// Other useful constraints for laying things out.
extern Am_Formula Am_Fill_To_Bottom;
extern Am_Formula Am_Fill_To_Right;
extern Am_Formula Am_Fill_To_Rest_Of_Width;
extern Am_Formula Am_Fill_To_Rest_Of_Height;
extern Am_Formula Am_Width_Of_Parts;
extern Am_Formula Am_Height_Of_Parts;
extern Am_Formula Am_Center_X_Is_Center_Of;
extern Am_Formula Am_Center_Y_Is_Center_Of;
extern Am_Formula Am_Center_X_Is_Center_Of_Owner;
extern Am_Formula Am_Center_Y_Is_Center_Of_Owner;
extern Am_Formula Am_Right_Is_Right_Of_Owner;
extern Am_Formula Am_Bottom_Is_Bottom_Of_Owner;

extern Am_Formula Am_Same_As (Am_Slot_Key key);
extern Am_Formula Am_From_Owner (Am_Slot_Key key);
extern Am_Formula Am_From_Part (Am_Slot_Key part, Am_Slot_Key key);
extern Am_Formula Am_From_Sibling (Am_Slot_Key sibling, Am_Slot_Key key);
extern Am_Formula Am_From_Object (Am_Object object, Am_Slot_Key key);

extern Am_Formula Am_Same_As (Am_Slot_Key key, int offset,
			      float multiplier = 1.0);
extern Am_Formula Am_From_Owner (Am_Slot_Key key, int offset,
				 float multiplier = 1.0);
extern Am_Formula Am_From_Part (Am_Slot_Key part, Am_Slot_Key key, int offset,
				float multiplier = 1.0);
extern Am_Formula Am_From_Sibling (Am_Slot_Key sibling, Am_Slot_Key key,
				   int offset, float multiplier = 1.0);
extern Am_Formula Am_From_Object (Am_Object object, Am_Slot_Key key,
				  int offset, float multiplier = 1.0);

//This enum is used internally; users should use the Am_Alignment
// values instead.
enum Am_Alignment_vals
{ Am_CENTER_ALIGN_val, Am_TOP_ALIGN_val, Am_BOTTOM_ALIGN_val,
  Am_LEFT_ALIGN_val, Am_RIGHT_ALIGN_val };

// For Am_H_ALIGN and Am_V_ALIGN slots.
Am_Define_Enum_Type(Am_Alignment, Am_Alignment_vals)
const Am_Alignment Am_CENTER_ALIGN (Am_CENTER_ALIGN_val);
const Am_Alignment Am_TOP_ALIGN (Am_TOP_ALIGN_val);
const Am_Alignment Am_BOTTOM_ALIGN (Am_BOTTOM_ALIGN_val);
const Am_Alignment Am_LEFT_ALIGN (Am_LEFT_ALIGN_val);
const Am_Alignment Am_RIGHT_ALIGN (Am_RIGHT_ALIGN_val);

// For Am_FIXED_WIDTH and Am_FIXED_HEIGHT slots.
#define Am_NOT_FIXED_SIZE 0
#define Am_MAX_FIXED_SIZE 1

// For Am_Text's Am_CURSOR_INDEX
#define Am_NO_CURSOR -1

extern void Am_Initialize ();

extern void Am_Cleanup ();

extern void Am_Beep (Am_Object window = Am_No_Object);

extern void Am_Move_Object (Am_Object object, Am_Object ref_object,
                            bool above = true);
extern void Am_To_Top (Am_Object object);
extern void Am_To_Bottom (Am_Object object);

extern Am_Object Am_Create_Screen (const char* display_name);

extern bool Am_Do_Events (bool wait = false);
extern void Am_Wait_For_Event ();
extern void Am_Main_Event_Loop ();

extern void Am_Exit_Main_Event_Loop ();

//Check whether point is inside all the owners of object, up to the
//window.  Also validates that all of the owners are visible.
//  If not, returns false.  Use this to make sure that not pressing
//  outside of an owner since the other operations below do NOT check
//  this.
extern bool Am_Point_In_All_Owners(const Am_Object& in_obj, int x, int y,
				   const Am_Object& ref_obj);

// Check whether the point is inside the object.  Ignores
//   covering (i.e., just checks whether point is inside the
//   object even if the object is covered.  If inside, returns the
//   object, otherwise returns NULL (0)
//  The coordinate system of x and y is defined w.r.t. ref_obj
extern Am_Object Am_Point_In_Obj (const Am_Object& in_obj, int x, int y,
				  const Am_Object& ref_obj);
 
// Find the front-most immediate child object at the specified
//  location.  If none, then if want_self then if inside in_obj,
//  returns in_obj.   If NOT want_self or NOT inside in_obj, returns
//  Am_No_Object. The coordinate system of x and y is defined
//  w.r.t. ref_obj.   If want_groups is true, the finds the
//   leaf-most element even if it is a group.  If want_groups is
//   false, then will not return a group (if x,y is not over a
//   "primitive" object, returns Am_No_Object)
extern Am_Object Am_Point_In_Part (const Am_Object& in_obj, int x, int y,
				   const Am_Object& ref_obj,
				   bool want_self = false,
				   bool want_groups = true);
 
// Find the leaf-most object at the specified location.  If x,y is inside
//   in_obj but not over a leaf, then if want_self returns in_obj,
//   otherwise returns Am_No_Object.  If want_groups is true, the finds the
//   leaf-most element even if it is a group.  If want_groups is
//   false, then will not return a group (if x,y is not over a
//   "primitive" object, returns Am_No_Object)
// The coordinate system of x and y is defined w.r.t. ref_obj
extern Am_Object Am_Point_In_Leaf (const Am_Object& in_obj, int x, int y,
				   const Am_Object& ref_obj,
				   bool want_self = true,
				   bool want_groups = true);

// Converts a point in one object's coordinate system, to that of another
// object.  If the objects are not comparable (like being on different screens
// or not being on a screen at all) then the function will return false.
// Otherwise, it will return true and dest_x and dest_y will contain the
// converted coordinates.  Note that the coordinates are for the
// INSIDE of dest_obj.  This means that if "obj" was at src_x, src_y
// in src_obj and you remove it from src_obj and add it to dest_obj at
// dest_x, dest_y then it will be at the same physical screen position.
extern bool Am_Translate_Coordinates
           (const Am_Object& src_obj, int src_x, int src_y,
            const Am_Object& dest_obj, int& dest_x, int& dest_y);

#define Am_Define_Style_Formula(formula_name) \
     Am_Define_Formula (Am_Wrapper*, formula_name)
#define Am_Define_Value_List_Formula(formula_name) \
     Am_Define_Formula (Am_Wrapper*, formula_name)
#define Am_Define_Font_Formula(formula_name) \
     Am_Define_Formula (Am_Wrapper*, formula_name)
#define Am_Define_Point_List_Formula(formula_name) \
     Am_Define_Formula (Am_Wrapper*, formula_name)
#define Am_Define_Image_Formula(formula_name) \
     Am_Define_Formula (Am_Wrapper*, formula_name)
#define Am_Define_Cursor_Formula(formula_name) \
     Am_Define_Formula (Am_Wrapper*, formula_name)

//methods for the Am_DESTROY_WINDOW_METHOD slot of windows
extern Am_Object_Method Am_Window_Hide_Method; //set visible to false
extern Am_Object_Method Am_Window_Destroy_And_Exit_Method;  //exit MEL
extern Am_Object_Method Am_Default_Window_Destroy_Method; 
         //destroy and exit MEL if no windows are left

//Type of method that the Am_Register_Timer function will call.
//Am_Time defined in gdefs.h
Am_Define_Method_Type(Am_Timer_Method, void,
		      (Am_Object obj, const Am_Time& elapsed_time))

// Find the size of a text string given a screen and a font.  (Font size
// depends on the screen.)  The length parameter is the number of characters
// in the string.  Use -1 to mean the entire string.
extern void Am_Get_String_Extents (const Am_Object& screen,
				   const Am_Font& font,
				   const char* string, int length,
				   int& width, int& height);

// Find the width of a text string given a screen and a font.  (Font size
// depends on the screen.)  The length parameter is the number of characters
// in the string.  Use -1 to mean the entire string.
extern int Am_Get_String_Width (const Am_Object& screen, const Am_Font& font,
				const char* string, int length);

// Find the size properties of a font.  (Font size depends on the screen.)
extern void Am_Get_Font_Properties (const Am_Object& screen,
				    const Am_Font& font,
				    int& max_char_width, int& min_char_width,
				    int& ascent, int& descent);

// Verify if an image can be loaded.  This will test load the image.  If it
// succeeds, the procedure will return true and the image may be used without
// error.  Otherwise, the procedure will return false and the image should not
// be used (reassign value to Am_No_Image_Array or another image).
extern bool Am_Test_Image_File (const Am_Object& screen,
				const Am_Image_Array& image);

// Finds the screen that this object is attached to.  (Mostly used in
// conjunction with Am_Get_String_Extents.
extern Am_Object Am_GV_Screen (const Am_Object& self);

Am_Define_Method_Type (Am_Save_Object_Method, void,
		       (ostream& os, Am_Load_Save_Context& context,
			const Am_Object& object))

extern Am_Load_Save_Context Am_Default_Load_Save_Context;

//Am_Standard_Save_Object can be put in the Am_SAVE_OBJECT_METHOD of an object.
//It expects the object or its immediate prototype to have been
//registered with context.Register_Prototype.  The object or its
//prototype must have a list of slot keys in the Am_SLOTS_TO_SAVE
//slot.  These slots are saved.  The load method for
//Am_Standard_Save_Object is automatically registered with every context.
extern Am_Save_Object_Method Am_Standard_Save_Object;

//Am_Standard_Load_Object is automatically registered for loading the
//results of Am_Standard_Save_Object in the
//Am_Default_Load_Save_Context, but you might need to register it if
//you create a DIFFERENT context from the default.
extern Am_Load_Method Am_Standard_Load_Object;

//A special object for transforming one kind of saved object into another.
//Register an instance of this object as the prototype of an object saved
//with standard save.  Set the slot Am_ITEM_PROTOTYPE with the real prototype
//and the slot Am_SLOTS_TO_SAVE with the slots to load.
extern Am_Object Am_Load_Object_Swap;

//use Am_Standard_Save_Group instead of Am_Standard_Save_Object for
//groups (so the parts will be saved).  For example, you should
//register Am_Resize_Parts_Group if you have grouping cmds in the edit menu.
extern Am_Save_Object_Method Am_Standard_Save_Group;
extern Am_Load_Method Am_Standard_Load_Group;

#endif
