/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains utility functions shared by many of the Gem test
   programs, such as testcolor and testwinprops.
*/

#include <am_inc.h>

#include GEM__H

extern Am_Style red, green, blue, white, orange, orchid, purple,
 grey, plum, yellow;
extern Am_Style black, black4, gray_stipple;
extern Am_Style dash15, dash8, dash20;

extern Am_Style miter_ls, round_ls, bevel_ls, not_last, butt, projecting,
         no_dash, dash1, dash2, dash3, dash4;

extern Am_Font default_font, font1, font2, font3, font_large, font_vgi;

// Instances of this class are used to pass around sets of drawonables
// for multiple-screen functions to work on
//
class wins {
public:
  Am_Drawonable *root, *font_win, *color_win, *d1, *d2, *d3, *d4, *d5;
};

// Create wins_array and return the number of elements in the array
//
extern wins* create_wins_array (int argc, char **argv, int &ar_len);

// Called by testdpy
extern void test_bits   (wins *wins_ar, int ar_len);
extern void test_colors (wins *wins_ar, int ar_len);
extern void test_fonts  (wins *wins_ar, int ar_len);
extern void test_win_props  (wins *wins_ar, int ar_len);

// Called by testgem
extern void test_line_props (Am_Drawonable *d);
extern void draw_corners (Am_Drawonable *d,
			  int rect_left, int rect_top, int rect_width,
			  int rect_height);

