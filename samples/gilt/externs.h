#ifndef __EXTERNS_H
#define __EXTERNS_H

#include <amulet.h>

extern Am_Font fontarray[];

extern Am_Slot_Key CUSTOMIZE_METHOD;
extern Am_Slot_Key SUB_LABEL;
// extern Am_Slot_Key Lw_FORMAT;
extern Am_Slot_Key Lw_NAME;
extern Am_Slot_Key FONT_KEY;
extern Am_Slot_Key LAYOUT_KEY;
extern Am_Slot_Key FILL_STYLE_KEY;
extern Am_Slot_Key LINE_STYLE_KEY;
extern Am_Slot_Key TYPE_NAME;
extern Am_Slot_Key SAVE_COMMAND_ITEMS;
extern Am_Slot_Key NAME_OBJ;
extern Am_Slot_Key LABEL_OBJ;
extern Am_Slot_Key RESTRICTED;
extern Am_Slot_Key MIN_VALUE;
extern Am_Slot_Key MAX_VALUE;
extern Am_Slot_Key LOOKS_SELECTED_OBJ;
extern Am_Slot_Key SCROLL_PROPS_OBJ;

extern Am_Object_Method ok_button_pressed_cmd;
extern Am_Object_Method cancel_button_pressed_cmd;


extern void init_styles();

#define FILL_STYLE_CNT 24
#define LINE_STYLE_CNT 33
#define FONT_ITEMS 12



extern Am_Style n2s[];
extern Am_Style n2l[];

extern char * n2sstr[];
extern char * n2lstr[];
extern char *layout[];

extern void output_cc_with_header(ostream &os_h, ostream &os_cc,
				  bool is_window,
				  Am_Value_List &top_level_objs_list,
				  Am_String wingroup_name,
				  int fill_key, Am_String title,
				  bool explicit_wh, int width, int height,
				  bool win_fixed_size);
extern void output_cc_no_header(ostream &os_cc, bool is_window,
				Am_Value_List &top_level_objs_list,
				Am_String wingroup_name,
				int fill_key, Am_String title,
				bool explicit_wh, int width, int height,
				bool win_fixed_size);

extern Am_String Add_Extension(Am_String in_filename, const char * ext);

extern Am_Object Name_Only_Window;
extern Am_Object Name_Only_Window_Initialize ();

extern Am_Object Number_Input_Window;
extern Am_Object Number_Input_Window_Initialize ();

extern Am_Object Border_Rect_Window;
extern Am_Object Border_Rect_Window_Initialize ();

extern Am_Object About_Gilt_Window;
extern Am_Object About_Gilt_Window_Initialize ();

extern Am_Object Name_And_Label_Window;
extern Am_Object Name_And_Label_Window_Initialize ();

extern Am_Object Scroll_Group_Window;
extern Am_Object Scroll_Group_Window_Initialize ();

#endif
