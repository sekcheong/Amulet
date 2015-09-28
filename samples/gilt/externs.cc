/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code has been placed in the public   			          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* Originally written as AmEdit by S.Nebel (Linkworks Ltd Wellington,NZ) 1997.
   Modified and updated by Brad A. Myers of the Amulet project at CMU.
*/

#include "externs.h"

/* ************************************************************************ */

Am_Font fontarray[] = {
  Am_Font(Am_FONT_FIXED,false,false,false,Am_FONT_SMALL),
  Am_Font(Am_FONT_FIXED,true, false,false,Am_FONT_SMALL),
  Am_Font(Am_FONT_FIXED,false, true,false,Am_FONT_SMALL),
  Am_Font(Am_FONT_FIXED,false, false,true,Am_FONT_SMALL),
  
  Am_Font(Am_FONT_FIXED,false,false,false,Am_FONT_MEDIUM),
  Am_Font(Am_FONT_FIXED,true, false,false,Am_FONT_MEDIUM),
  Am_Font(Am_FONT_FIXED,false, true,false,Am_FONT_MEDIUM),
  Am_Font(Am_FONT_FIXED,false, false,true,Am_FONT_MEDIUM),
  
  Am_Font(Am_FONT_FIXED,false,false,false,Am_FONT_LARGE),
  Am_Font(Am_FONT_FIXED,true, false,false,Am_FONT_LARGE),
  Am_Font(Am_FONT_FIXED,false, true,false,Am_FONT_LARGE), 
  Am_Font(Am_FONT_FIXED,false, false,true,Am_FONT_LARGE) 

  };
/* ************************************************************************ */



Am_Style n2s[FILL_STYLE_CNT];
Am_Style n2l[LINE_STYLE_CNT];



Am_Define_Method(Am_Object_Method, void, ok_button_pressed_cmd, (Am_Object cmd))
{
  Am_Object owner = cmd.Get_Owner();
  if (owner.Valid())
    Am_Finish_Pop_Up_Waiting(owner.Get(Am_WINDOW),Am_Value(true));
}

Am_Define_Method(Am_Object_Method, void, cancel_button_pressed_cmd, (Am_Object cmd))
{
  Am_Object owner = cmd.Get_Owner();
  if (owner.Valid()) 
    Am_Finish_Pop_Up_Waiting(owner.Get(Am_WINDOW),Am_Value(false));
}

void init_styles()
{
n2s[0] = Am_No_Style;
n2s[1] = Am_Red;
n2s[2] = Am_Green;
n2s[3] = Am_Blue;
n2s[4] = Am_Yellow;
n2s[5] = Am_Purple;
n2s[6] = Am_Cyan;
n2s[7] = Am_Orange;
n2s[8] = Am_Black;
n2s[9] = Am_White;
n2s[10] = Am_Amulet_Purple;
n2s[11] = Am_Motif_Gray;
n2s[12] = Am_Motif_Light_Gray;
n2s[13] = Am_Motif_Blue;
n2s[14] = Am_Motif_Light_Blue;
n2s[15] = Am_Motif_Green;
n2s[16] = Am_Motif_Light_Green;
n2s[17] = Am_Motif_Orange;
n2s[18] = Am_Motif_Light_Orange;
n2s[19] = Am_Gray_Stipple;
n2s[20] = Am_Opaque_Gray_Stipple;
n2s[21] = Am_Light_Gray_Stipple;
n2s[22] = Am_Dark_Gray_Stipple;
n2s[23] = Am_Diamond_Stipple;


n2l[0]=Am_No_Style;
n2l[1]=Am_Red;
n2l[2]=Am_Green;
n2l[3]=Am_Blue;
n2l[4]=Am_Yellow;
n2l[5]=Am_Purple;
n2l[6]=Am_Cyan;
n2l[7]=Am_Orange;
n2l[8]=Am_Black;
n2l[9]=Am_White;
n2l[10]=Am_Amulet_Purple;
n2l[11]=Am_Motif_Gray;
n2l[12]=Am_Motif_Light_Gray;
n2l[13]=Am_Motif_Blue;
n2l[14]=Am_Motif_Light_Blue;
n2l[15]=Am_Motif_Green;
n2l[16]=Am_Motif_Light_Green;
n2l[17]=Am_Motif_Orange;
n2l[18]=Am_Motif_Light_Orange;
n2l[19]=Am_Gray_Stipple;
n2l[20]=Am_Opaque_Gray_Stipple;
n2l[21]=Am_Light_Gray_Stipple;
n2l[22]=Am_Dark_Gray_Stipple;
n2l[23]=Am_Diamond_Stipple;
n2l[24]=Am_Opaque_Diamond_Stipple;
n2l[25]=Am_Thin_Line;
n2l[26]=Am_Line_0;
n2l[27]=Am_Line_1;
n2l[28]=Am_Line_2;
n2l[29]=Am_Line_4;
n2l[30]=Am_Line_8;
n2l[31]=Am_Dotted_Line;
n2l[32]=Am_Dashed_Line;
}

char *n2sstr[] = {
 "Am_No_Style",
 "Am_Red",
 "Am_Green",
 "Am_Blue",
 "Am_Yellow",
 "Am_Purple",
 "Am_Cyan",
 "Am_Orange",
 "Am_Black",
 "Am_White",
 "Am_Amulet_Purple",
 "Am_Motif_Gray",
 "Am_Motif_Light_Gray",
 "Am_Motif_Blue",
 "Am_Motif_Light_Blue",
 "Am_Motif_Green",
 "Am_Motif_Light_Green",
 "Am_Motif_Orange",
 "Am_Motif_Light_Orange",
 "Am_Gray_Stipple",
 "Am_Opaque_Gray_Stipple",
 "Am_Light_Gray_Stipple",
 "Am_Dark_Gray_Stipple",
 "Am_Diamond_Stipple"
};

char *n2lstr[] = {
"Am_No_Style",
"Am_Red",
"Am_Green",
"Am_Blue",
"Am_Yellow",
"Am_Purple",
"Am_Cyan",
"Am_Orange",
"Am_Black",
"Am_White",
"Am_Amulet_Purple",
"Am_Motif_Gray",
"Am_Motif_Light_Gray",
"Am_Motif_Blue",
"Am_Motif_Light_Blue",
"Am_Motif_Green",
"Am_Motif_Light_Green",
"Am_Motif_Orange",
"Am_Motif_Light_Orange",
"Am_Gray_Stipple",
"Am_Opaque_Gray_Stipple",
"Am_Light_Gray_Stipple",
"Am_Dark_Gray_Stipple",
"Am_Diamond_Stipple",
"Am_Opaque_Diamond_Stipple",
"Am_Thin_Line",
"Am_Line_0",
"Am_Line_1",
"Am_Line_2",
"Am_Line_4",
"Am_Line_8",
"Am_Dotted_Line",
"Am_Dashed_Line"
};


char * layout[] = { "Am_Vertical_Layout",
                    "Am_Horizontal_Layout"
		  };    







