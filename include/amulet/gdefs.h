/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains graphic constants and types used by both Opal and Gem.
   Amulet users will need to reference classes defined in this file to
   provide parameters to Amulet functions.
*/

#ifndef GDEFS_H
#define GDEFS_H

#include <am_inc.h>

#include AM_IO__H
#include TYPES__H  // to get the Am_Wrapper object
#include VALUE_LIST__H  //value lists for file names

//for styles: fill tiled not supported
enum Am_Fill_Solid_Flag { Am_FILL_SOLID = 0, Am_FILL_STIPPLED = 2, 
			  Am_FILL_OPAQUE_STIPPLED = 3};

enum Am_Fill_Poly_Flag { Am_FILL_POLY_EVEN_ODD = 0, Am_FILL_POLY_WINDING = 1 };

enum Am_Line_Cap_Style_Flag { Am_CAP_NOT_LAST = 0, Am_CAP_BUTT = 1, 
			      Am_CAP_ROUND = 2, Am_CAP_PROJECTING = 3 };

enum Am_Join_Style_Flag { Am_JOIN_MITER = 0, Am_JOIN_ROUND = 1,
			  Am_JOIN_BEVEL = 2 };

// double dash not supported
enum Am_Line_Solid_Flag { Am_LINE_SOLID = 0, Am_LINE_ON_OFF_DASH = 1 };

#define Am_DEFAULT_DASH_LIST_VALUE {4,4}
#define Am_DEFAULT_DASH_LIST_LENGTH 2
extern const char Am_DEFAULT_DASH_LIST[2];

enum Am_Arc_Style_Flag { Am_ARC_CHORD = 0, Am_ARC_PIE_SLICE = 1 };

enum Am_Radius_Flag { Am_SMALL_RADIUS = -1, Am_MEDIUM_RADIUS = -2, 
		      Am_LARGE_RADIUS = -3};

//for fonts

enum Am_Font_Family_Flag { 
  Am_FONT_FIXED, Am_FONT_SERIF, Am_FONT_SANS_SERIF,  // Standard fonts for one-byte code
  Am_FONT_JFIXED, // Japanese standard font
  Am_FONT_JPROPORTIONAL, // Japanese proportional font, equals Am_FONT_JFIXED on X
  Am_FONT_CFIXED, // Chinese font, equals Am_FONT_JFIXED on Windows 
  Am_FONT_KFIXED // Korean font, equals Am_FONT_JFIXED on Windows
};

enum Am_Font_Size_Flag { Am_FONT_SMALL = 0, Am_FONT_MEDIUM = 1,
			 Am_FONT_LARGE = 2, Am_FONT_VERY_LARGE = 3,
		// the constant definitions for the sizes of Two-byte code are below 
			Am_FONT_TSMALL = 0x10, Am_FONT_TMEDIUM = 0x11,
			Am_FONT_TLARGE = 0x12, Am_FONT_TVERY_LARGE = 0x13 
};

class Am_Font_Data;

class Am_Font {
  Am_WRAPPER_DECL (Am_Font)
public:
    //creators
  Am_Font (const char* the_name);
  Am_Font (Am_Font_Family_Flag f = Am_FONT_FIXED,
	   bool is_bold = false,
	   bool is_italic = false,
	   bool is_underline = false,
	   Am_Font_Size_Flag s = Am_FONT_MEDIUM);

  void Get(Am_String &name,
	   Am_Font_Family_Flag &f,
	   bool &is_bold,
	   bool &is_italic,
	   bool &is_underline,
	   Am_Font_Size_Flag &s);
  bool operator== (const Am_Font& font) const;
  bool operator!= (const Am_Font& font) const;

  static bool Font_Name_Valid (const char* name);
};

class Am_Style;

class Am_Image_Array_Data;

// This is used to hold the data for pixmaps and bitmaps.
// I assume most users will only want to read from files, so this object 
// is not normally available to users.  (otherwise it should be moved
// to gconstants and be made a subclass of Am_Value)

enum Am_Image_File_Format { XBM_FILE_FORMAT, XPM_FILE_FORMAT, 
			    GIF_FILE_FORMAT };

class Am_Image_Array
{
  Am_WRAPPER_DECL (Am_Image_Array)
public:
  // creators
  // create by read from file
  Am_Image_Array ();
  Am_Image_Array (const char* file_name);
  Am_Image_Array (unsigned int width, 
		  unsigned int height, int depth,
		  Am_Style intial_color);
  Am_Image_Array (int percent);
  Am_Image_Array (char *bit_data, int height, int width);

  Am_Image_Array make_diamond();

  // other functions
  int Get_Bit(int x, int y); // NDY
  void Set_Bit(int x, int y, int val);  //NDY

  // Returns in storage the RGB values for each pixel in the image
  // using 1 byte each for R, for G and for B.  So ...
  // storage must be large enough to accomodate 3*width*height bytes.
  // If top_first return RGB values for top row first
  // else return RGB for bottom row first.
  // Returns true if successful (i.e. implemented).
  bool Get_RGB_Image(unsigned char * storage, bool top_first = true);

  // Size will be zero until the image is drawn.  Get the size of an
  // image through Am_Drawonable::Get_Image_Size.
  void Get_Size (int& width, int& height) const;
  
  int Write_To_File (const char* file_name,
		     Am_Image_File_Format form);
				//returns 0 or error code.  NDY
};

extern Am_Image_Array Am_No_Image;

class Am_Cursor_Data;

class Am_Cursor 
{
  Am_WRAPPER_DECL(Am_Cursor)

public:
  // creators
  Am_Cursor();
  Am_Cursor(Am_Image_Array image, Am_Image_Array mask,
	    Am_Style fg_color, Am_Style bg_color);
  
  // sets the cursor in the proper window
  // do we want the function for this to be in cursor or in drawonable?
  void Set_Hot_Spot (int x, int y);
  void Get_Hot_Spot (int& x, int& y) const;

  // Size will be zero until the image is drawn.  Get the size of an
  // image through Am_Drawonable::Get_Image_Size.
  void Get_Size (int& width, int& height);
};

extern Am_Cursor Am_Default_Cursor;

// Styles include all the drawing properties, including the color and
// various line and fill control styles.
// The color part uses red, green, blue that go from 0.0 to 1.0, where
// 1.0 is full on.

class Am_Style_Data;

class Am_Style
 {
  Am_WRAPPER_DECL (Am_Style)
public:
    //creators
  Am_Style ();
  
  //full properties create
   Am_Style (float r, float g, float b,  //color part
	     short thickness = 0,
	     Am_Line_Cap_Style_Flag cap = Am_CAP_BUTT,
	     Am_Join_Style_Flag join = Am_JOIN_MITER,
	     Am_Line_Solid_Flag line_flag = Am_LINE_SOLID,
	     const char* dash_l = Am_DEFAULT_DASH_LIST,
	     int dash_l_length = Am_DEFAULT_DASH_LIST_LENGTH,
	     Am_Fill_Solid_Flag fill_flag = Am_FILL_SOLID,
	     Am_Fill_Poly_Flag poly = Am_FILL_POLY_EVEN_ODD,
	     Am_Image_Array stipple = Am_No_Image);
  
  Am_Style (const char* color_name,
	    short thickness = 0,
	    Am_Line_Cap_Style_Flag cap = Am_CAP_BUTT,
	    Am_Join_Style_Flag join = Am_JOIN_MITER,
	    Am_Line_Solid_Flag line_flag = Am_LINE_SOLID,
	    const char *dash_l = Am_DEFAULT_DASH_LIST,
	    int dash_l_length = Am_DEFAULT_DASH_LIST_LENGTH,
	    Am_Fill_Solid_Flag fill_flag = Am_FILL_SOLID,
	    Am_Fill_Poly_Flag poly = Am_FILL_POLY_EVEN_ODD,
	    Am_Image_Array stipple = Am_No_Image);

  bool operator== (const Am_Style& style) const;
  bool operator!= (const Am_Style& style) const;

  static Am_Style Thick_Line (unsigned short thickness); 
  static Am_Style Halftone_Stipple (int percent,
				    Am_Fill_Solid_Flag
				    fill_flag = Am_FILL_STIPPLED);
  Am_Style Clone_With_New_Color (Am_Style& foreground) const;

  // accessor
  void Get_Values (float& r, float& g, float& b) const;
  
  void Get_Values (short& thickness,
		   Am_Line_Cap_Style_Flag& cap,
		   Am_Join_Style_Flag& join,
		   Am_Line_Solid_Flag& line_flag,
		   const char*& dash_l, int& dash_l_length,
		   Am_Fill_Solid_Flag& fill_flag,
		   Am_Fill_Poly_Flag& poly,
		   Am_Image_Array& stipple) const;
  
  void Get_Values (float& r, float& g, float& b,
		   short& thickness,
		   Am_Line_Cap_Style_Flag& cap,
		   Am_Join_Style_Flag& join,
		   Am_Line_Solid_Flag& line_flag,
		   const char*& dash_l, int& dash_l_length,
		   Am_Fill_Solid_Flag& fill_flag,
		   Am_Fill_Poly_Flag& poly,
		   Am_Image_Array& stipple) const;

  Am_Fill_Solid_Flag Get_Fill_Flag() const;
  Am_Image_Array Get_Stipple() const;
  Am_Fill_Poly_Flag Get_Fill_Poly_Flag () const;
  
  //Get the properties needed to calculate the line width 
  void Get_Line_Thickness_Values (short& thickness,
				  Am_Line_Cap_Style_Flag& cap) const;

  const char* Get_Color_Name () const;
  //returns a pointer to the string, don't dealloc
  
  void Add_Image (Am_Image_Array image);
};

extern Am_Style Am_No_Style;

class Am_Point_Item;
class Am_Point_List_Data;

class Am_Point_List {
  Am_WRAPPER_DECL (Am_Point_List)
public:
  Am_Point_List ();
//  Am_Point_List (Am_Point_List&);
  Am_Point_List (int *ar, int size);
  Am_Point_List (float *ar, int size);

  bool operator== (const Am_Point_List& test_list) const;
  bool operator!= (const Am_Point_List& test_list) const;

  // Returns the number of points in the list.
  unsigned short Length () const;

  // Returns whether list is empty or not.
  bool Empty () const;

  void Start (); // Make first point be current.
  void End ();   // Make last point be current.

  void Prev ();  // Make previous point be current.
  void Next ();  // Make next point be current.

  // Returns TRUE when current point passes the first point.
  bool First () const;

  // Returns TRUE when current point passes the last point.
  bool Last () const;

  // Retrieve the current point.  Error if no point is
  // current.  (Can be returned in either float or int representation.)
  void Get (int &x, int &y) const;
  void Get (float &x, float &y) const;

#ifdef AMULET2_INSTRUMENT
#undef Set
#undef Add
#endif

  // Add puts the new point at the head or tail of the list.
  Am_Point_List& Add (float x, float y, Am_Add_Position position = Am_TAIL,
                      bool unique = true);

  // Insert puts the new point before or after the current position
  // in the list.  The current position is set by using the Start, End, Next,
  // and Prev methods.
  void Insert (float x, float y, Am_Insert_Position position,
               bool unique = true);

  // Change the current point.  Error if no point is current.
  void Set (float x, float y, bool unique = true);

#ifdef AMULET2_INSTRUMENT
  Am_Instrumentation(Am_Point_List)

#define Set Am_Instrumented(Set)
#define Add Am_Instrumented(Add)
#endif

  // Delete the current point.  Error if no point is current.  The current
  // position is shifted to the point previous to the deleted.
  void Delete (bool unique = true);

  // Delete the entire list.  All points are deleted. The current position
  void Make_Empty ();  // becomes undefined.

  //adds points in other_list to my end. Returns me (this) (so can be cascaded)
  Am_Point_List& Append (Am_Point_List other_list, bool unique = true);

  // returns bounding box of all points
  void Get_Extents (int& min_x, int& min_y, int& max_x, int& max_y) const;
  
  // translates all points to a new origin.
  void Translate (int offset_x, int offset_y,
		  bool unique = true);

  // scales all points.  Scaling is relative to (origin_x, origin_y).
  void Scale (float scale_x, float scale_y,
	      int origin_x = 0, int origin_y = 0,
	      bool unique = true);

 private:
  Am_Point_Item* item;
};

ostream& operator<< (ostream& os, Am_Point_List& list);

class Am_Point_Array_Data;

class Am_Point_Array {
  Am_WRAPPER_DECL(Am_Point_Array)
public:
  Am_Point_Array (Am_Point_List pl);
  Am_Point_Array (Am_Point_List pl, int offset_x, int offset_y);
  Am_Point_Array (int *ar, int num_coords);
  Am_Point_Array (int num_points);

  int Length ();
  void Get (int index, int &x, int &y);
#ifdef AMULET2_INSTRUMENT
#undef Set
#endif
  void Set (int index, int x, int y);
#ifdef AMULET2_INSTRUMENT
  Am_Instrumentation(Am_Point_Array)

#define Set Am_Instrumented(Set)
#define Add Am_Instrumented(Add)
#endif
  void Translate (int offset_x, int offset_y);
};

// Time structure for timing events, etc.
class Am_Time_Data;

class Am_Time 
{
  Am_WRAPPER_DECL (Am_Time)
public:
  Am_Time();  // defaults to 0 delta time
  Am_Time (unsigned long milliseconds); // starting number of msec
  static Am_Time Now();
  bool Is_Future () const;
  bool Is_Past () const;
  bool operator> (const Am_Time& other) const;
  bool operator< (const Am_Time& other) const;
  bool operator>= (const Am_Time& other) const;
  bool operator<= (const Am_Time& other) const;

  unsigned long Milliseconds() const;
  
  Am_Time operator- (unsigned long milliseconds) const;
  Am_Time operator+ (unsigned long milliseconds) const;
  Am_Time operator- (const Am_Time& other) const;
  Am_Time operator+ (const Am_Time& other) const;
  void operator+= (unsigned long milliseconds);
  void operator-= (unsigned long milliseconds);
  void operator+= (const Am_Time& other);
  void operator-= (const Am_Time& other);

  bool Zero() const;  // is the delta time 0?  (uninitialized time test)
};

ostream& operator<< (ostream& os, const Am_Time& time);

//returns the current time and date as a string, like
//  "Fri Jan 17 16:03:55 EST 1997\n".
extern Am_String Am_Get_Time_And_Date(); 

////////////////////////////////////////////////////////////////////////
//Interfaces for getting file data in a machine-independent way
////////////////////////////////////////////////////////////////////////

// **** NOTE: THESE FUNCTIONS ARE NOT IMPLEMENTED YET ****************

// Returns the full filename of the specified name.  Depending on the
// file system, this might add the current directory to the front.
extern Am_String Am_Get_Full_Filename(Am_String partial_filename);

// Pulls all the directory part out of the filename and returns it,
// and then sets file_only_name to the file.  Either one might be
// empty, but won't be NULL.
extern Am_String Am_Get_Directory_Part(Am_String filename,
				       Am_String &file_only_name);

// attaches directory_part before filename (normally, just a string
// concatenation, possibly with a separator like "/")
extern Am_String Am_Concat_Filename (Am_String directory_part,
				     Am_String filename);

// Given a filename that might have directories, takes it apart and
// returns the list of directories. The highest level directory (e.g.:
// "C:" on a PC, "/" on Unix, etc.) is first in the list.  If filename
// does not have any directory separators (either / or \ depending on
// the system) then a value_list with one entry, the passed in
// filename, will be returned.
extern Am_Value_List Am_Parse_Filename(Am_String filename);

//Returns a list of am_strings which are the names of all the files in
//the directory.  The file named "." is not returned, but ".." is.
extern Am_Value_List Am_Get_Directory_Contents(Am_String directory);

//Returns true of the filename is a directory
extern bool Am_File_Is_Directory(Am_String filename);

extern bool Am_File_Exists(Am_String full_filename);

//Returns number of bytes in the file.
extern long Am_Get_File_Size(Am_String filename);

//Under Unix and PC, returns the extension of the file.  If none,
//returns NULL.  Under Mac, returns the file type as a string.  
extern Am_String Am_Get_File_Type(Am_String filename);

//... Will probably need other procedures in the future

#endif
