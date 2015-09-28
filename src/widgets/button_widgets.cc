/* ************************************************************************
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the button-like widgets

   Designed and implemented by Brad Myers
*/

#include <am_inc.h>

#include AM_IO__H

#include WIDGETS_ADVANCED__H
#include STANDARD_SLOTS__H
#include VALUE_LIST__H
#include INTER_ADVANCED__H // for Am_Choice_Command_Set_Value and
                           // Am_Choice_Interactor_Repeat_Same
#include OPAL_ADVANCED__H  // for Am_DRAWONABLE, Am_Window_Coordinate

#include WIDGETS__H
#include GEM__H
#include INTER__H
#include OPAL__H

#ifdef DEBUG
#include REGISTRY__H //used for Am_Get_Type_Name to print a warning
#endif


//generate a string from the label if the command if not a string
//or object
#include STR_STREAM__H

Am_Style am_grey_2; //these are used by the check marks
Am_Style am_white_2; 

/******************************************************************************
 * Buttons
 *   Drawing Am_PUSH_BUTTON, Am_CHECK_BUTTON and Am_RADIO_BUTTON
 *   -----------------------------------------------------------
 * The basic strategy is to divide the drawing into two distinct stages.
 * The first stage involves drawing the _box_. Here is what the box is:
 *  o For push buttons  => the button outline and background.
 *  o For check buttons => the check box
 *  o For radio buttons => either the diamond or circle.
 * The second stage involves drawing the title. A title can either be a string
 * or an object. If both are supplied, the string is drawn.
 *
 * All drawing passes through Am_Draw_Button_Widget.
 *   Am_Draw_Button_Widget first does drawing for key_selected.
 *   Then it calls Am_Draw_*_Button_Box to draw the appropriate box.
 *     The coordinates are sent in the form of a struct rect. These coords
 *     define the rectangle for the box to be drawn. This allows the drawing
 *     routines to clip drawing to that region. Additionally the drawing
 *     routines are not concerned with if the check box is on the left or right.
 *     Am_Draw_Button_Widget determines the appropriate coords.
 *   Once the box is drawn, then either the string or object is drawn, inside
 *   its own clipped rectangle. Since the drawing for the string or the object
 *   tends to be almost look and feel independent, all the drawing is done
 *   directly from Am_Draw_Button_Widget.
 */

/******************************************************************************
 * Button drawing helper stuff.
 *   o struct am_rect - makes passes coordinates easier. Also allows for
 *       Inset_Rect
 *   o Inset_Rect, since much of the drawing involves concentric rectangles
 *       which grow smaller, Inset_Rect simplifies maitaining rectangle coords.
 *   o Am_Draw_Rect_Border - used mainly for the Windows drawing routines.
 *       For drawing 3D like rectangles. Upper_left is the line style used to
 *       draw the top and left sides. Lower_right for the bottom and right.
 */

/******************************************************************************
 * PROTOTYPES
 *****************************************************************************/
int Am_Draw_Push_Button_Box( am_rect r, Am_Drawonable* draw,
  Am_Widget_Look look, bool selected, bool key_selected,
  const Computed_Colors_Record& color_rec );
void Am_Draw_Check_Button_Box( am_rect r, Am_Drawonable* draw,
  Am_Widget_Look look, bool selected, bool interim_selected,
  const Computed_Colors_Record& color_rec, bool mask );
void Am_Draw_Radio_Button_Box( am_rect r, Am_Drawonable* draw,
  Am_Widget_Look look, bool selected, bool interim_selected,
  const Computed_Colors_Record& color_rec, bool mask );
int calculate_button_fringe( Am_Widget_Look look, bool leave_room,
  bool key_sel, bool def );

/******************************************************************************
 * CONSTANTS
 *****************************************************************************/
const int kMotBox = 5; const int kMotKeySel = 2; const int kMotDefault = 5;
const int kWinBox = 5; const int kWinKeySel = 0; const int kWinDefault = 0;
const int kMacBox = 5; const int kMacKeySel = 0; const int kMacDefault = 4;

Am_Style compute_text_style(bool active, bool depressed,
			    const Computed_Colors_Record& color_rec,
			    Am_Widget_Look look,
			    Am_Button_Type type) {
  bool black = true;;
  if ((type == Am_PUSH_BUTTON || type == Am_MENU_BUTTON) &&
      ((look == Am_MACINTOSH_LOOK && depressed) ||
       (look == Am_MOTIF_LOOK && !color_rec.data->light) ||
       (look == Am_WINDOWS_LOOK && !color_rec.data->light &&
	!(depressed && type == Am_MENU_BUTTON))))
    black = false;
  if(active) {
    if (black) return Am_Black;
    else return Am_White;
  }
  else {
    if (black) return Am_Motif_Inactive_Stipple;
    else return Am_Motif_White_Inactive_Stipple;
  }
}

/*******************************************************************************
 * Am_Draw_Button_Widget
 *   Draws a push button
 */

void
Am_Draw_Button_Widget(
  int left, int top, int width, int height,  // dimensions
  const char* string, Am_Object obj,
  bool interim_selected, bool selected,
  bool active, bool key_selected,
  bool is_default, bool fringe,
  Am_Font font,
  const Computed_Colors_Record& color_rec,
  Am_Widget_Look look,
  Am_Button_Type type,
  Am_Drawonable* draw,
  int box_width, int box_height, bool box_on_left,
  Am_Alignment align, int offset_left, bool mask = false )
{
  am_rect box_r, r( left, top, width, height );
  bool depressed = selected || interim_selected;

  switch( type )
  {
    case Am_PUSH_BUTTON:
    {
      switch( look.value )
      {
        case Am_MOTIF_LOOK_val:
          if( key_selected )
            draw->Draw_Rectangle( Am_Key_Border_Line, Am_No_Style,
                                  r.left, r.top, r.width, r.height );
          if( fringe || key_selected )
            Inset_Rect( r, kMotKeySel );

          if( is_default )
            Am_Draw_Motif_Box( r.left, r.top, r.width, r.height, true,
                               color_rec, draw );
          if( fringe || is_default )
            Inset_Rect( r, kMotDefault );
          break;

        case Am_WINDOWS_LOOK_val:
          if( is_default && !depressed )
          {
            draw->Draw_Rectangle( Am_Style( "black", 1 ), Am_No_Style,
                                  r.left, r.top, r.width, r.height );
            Inset_Rect( r, 1 );
          }
          break;

        case Am_MACINTOSH_LOOK_val: {
          static Am_Style black( "black", 3 );
          static Am_Style ltgray( "ltgray", 3 );

          if( is_default )
            draw->Draw_Roundtangle( active ? black : ltgray,
                                    Am_No_Style,
                                    r.left, r.top, r.width, r.height, 8, 8 );
          if( fringe || is_default )
            Inset_Rect( r, kMacDefault );
          break;
	}
        default:
          Am_Error ("Unknown Look parameter");
          break;
      }

      int inset = Am_Draw_Push_Button_Box( r, draw, look, depressed,
                                           key_selected, color_rec );
      Inset_Rect( r, inset );
      break;
    }

    case Am_CHECK_BUTTON:
      box_r.left   = box_on_left ? r.left : r.left + (r.width - box_width);
      box_r.top    = top + (r.height - box_height)/2;
      box_r.width  = box_width;
      box_r.height = box_height;
      Am_Draw_Check_Button_Box( box_r, draw, look, selected, interim_selected,
                                color_rec, mask );

      if( box_on_left )
        r.left  += box_width + offset_left;

      r.width -= box_width + offset_left;
      break;

    case Am_RADIO_BUTTON:
      box_r.left   = box_on_left ? r.left : r.left + (r.width - box_width);
      box_r.top    = top + (r.height - box_height)/2;
      box_r.width  = box_width;
      box_r.height = box_height;
      Am_Draw_Radio_Button_Box( box_r, draw, look, selected, interim_selected,
                                color_rec, mask );

      if( box_on_left )
        r.left += box_width + offset_left;

      r.width -= box_width + offset_left;
      break;
    default: Am_Error ("Switch statement is not complete");
      break;
  }

  draw->Push_Clip( r.left, r.top, r.width, r.height );

  if( depressed && look == Am_WINDOWS_LOOK && type == Am_PUSH_BUTTON )
  {
    r.left += 1;
    r.top  += 1;
  }

  am_rect text_or_obj_rect;

  if( string ) // draw the string if it exists
  {
    int   str_width, ascent, descent, a, b, str_left, str_top;

    Am_Style text_style = compute_text_style(active, depressed, color_rec,
					     look, type);

    draw->Get_String_Extents( font, string, strlen( string ), str_width,
                              ascent, descent, a, b);

    // center the text
    switch( align.value )
    {
      case Am_LEFT_ALIGN_val:
        str_left = r.left + offset_left;
        break;

      case Am_RIGHT_ALIGN_val:
        str_left = r.left + (r.width - str_width) - 2;
        break;

      case Am_CENTER_ALIGN_val:
      default:
        str_left = r.left + (r.width - str_width) / 2;
        break;
    }
    str_top = r.top + (r.height - ascent - descent) / 2;

    draw->Draw_Text( text_style, string, strlen( string ), font, str_left,
                     str_top, mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );

    text_or_obj_rect.left   = str_left;
    text_or_obj_rect.top    = str_top;
    text_or_obj_rect.width  = str_width;
    text_or_obj_rect.height = ascent + descent;
  }
  else if( obj.Valid() ) // draw the object if no string and there is an object
  {
    // center the object in the button
    text_or_obj_rect.width  = obj.Get( Am_WIDTH );
    text_or_obj_rect.height = obj.Get( Am_HEIGHT );
    text_or_obj_rect.left   = (r.width  - text_or_obj_rect.width)  / 2;
    text_or_obj_rect.top    = (r.height - text_or_obj_rect.height) / 2;
    obj.Set(Am_LEFT, text_or_obj_rect.left);
    obj.Set(Am_TOP, text_or_obj_rect.top);

    bool line_changed = false;
    if( depressed && look == Am_MACINTOSH_LOOK )
    {
      if( obj.Peek( Am_LINE_STYLE ).Exists() )
      {
        if( obj.Get( Am_LINE_STYLE ) == Am_Black )
        {
          obj.Set( Am_LINE_STYLE, Am_White );
          line_changed = true;
        }
      }
    }

    Am_Draw( obj, draw, r.left, r.top );

    if( line_changed )
      obj.Set( Am_LINE_STYLE, Am_Black );
  }

  // draw key selected
  if( key_selected && look == Am_WINDOWS_LOOK )
  {
    Am_Style dash = Am_Style::Halftone_Stipple( 50 );
    if( type == Am_PUSH_BUTTON )
    {
      Inset_Rect( r, 2 );
      if( depressed )
      {
        r.width  -= 1;
        r.height -= 1;
      }
      draw->Draw_Rectangle( dash, Am_No_Style, r.left, r.top, r.width,
                            r.height );
      Inset_Rect( r, 1 );
    }
    else if( type == Am_RADIO_BUTTON || type == Am_CHECK_BUTTON )
    {
      r.top    = text_or_obj_rect.top    - 1;
      r.left   = text_or_obj_rect.left   - 2;
      r.width  = text_or_obj_rect.width  + 4;
      r.height = text_or_obj_rect.height + 2;
      draw->Draw_Rectangle( dash, Am_No_Style, r.left, r.top, r.width,
                            r.height, mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
    }
  }
  draw->Pop_Clip();
}

/******************************************************************************
 * PUSH BUTTONS
 *****************************************************************************/

/******************************************************************************
 * button_draw
 */

Am_Define_Method( Am_Draw_Method, void, button_draw,
                ( Am_Object self, Am_Drawonable* drawonable,
                  int x_offset, int y_offset ) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top  = (int)self.Get( Am_TOP )  + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  bool selected = self.Get( Am_SELECTED );
  bool interim_selected = self.Get( Am_INTERIM_SELECTED );
  bool active = self.Get( Am_ACTIVE );
  bool key_selected = self.Get( Am_KEY_SELECTED );
  bool want_final_selected = self.Get( Am_FINAL_FEEDBACK_WANTED );
  bool is_default = self.Get( Am_DEFAULT );
  bool fringe = self.Get( Am_LEAVE_ROOM_FOR_FRINGE );

  Am_Font font( self.Get( Am_FONT ) );
  Am_Alignment align = self.Get( Am_H_ALIGN );

  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  // now find the contents to draw in the button
  Am_String string;
  Am_Object obj;
  Am_Value value;
  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value=self.Peek(Am_REAL_STRING_OR_OBJ);
  if( value.type == Am_STRING )
    string = value;
  else if( value.type == Am_OBJECT )
    obj = value;
  else
    Am_ERRORO("Label of widget " << self
             << " should have string or object value but it is "
             << value, self, Am_REAL_STRING_OR_OBJ);

  // finally ready to draw it
  Am_Draw_Button_Widget( left, top, width, height, string, obj,
                         interim_selected, selected && want_final_selected,
                         active, key_selected, is_default,
                         fringe, font, color_rec, look,
                         Am_PUSH_BUTTON, drawonable, 0, 0, false,
                         align, 0 );
}

/******************************************************************************
 * button_mask
 */

Am_Define_Method( Am_Draw_Method, void, button_mask,
                ( Am_Object self, Am_Drawonable* drawonable,
                  int x_offset, int y_offset ) )
{
  int left = (int)self.Get (Am_LEFT) + x_offset;
  int top  = (int)self.Get (Am_TOP)  + y_offset;
  int width = self.Get (Am_WIDTH);
  int height = self.Get (Am_HEIGHT);

  bool key_selected = self.Get (Am_KEY_SELECTED);
  bool is_default = self.Get (Am_DEFAULT);
  bool fringe = self.Get (Am_LEAVE_ROOM_FOR_FRINGE);

  Am_Font font (self.Get (Am_FONT));

  Am_Widget_Look look = self.Get (Am_WIDGET_LOOK);
  switch (look.value) {
  case Am_MOTIF_LOOK_val:
    if (fringe && !is_default) {
      if (key_selected)
        drawonable->Draw_Rectangle (Am_Key_Border_Line, Am_No_Style, left, top,
                                    width, height, Am_DRAW_MASK_COPY);
      drawonable->Draw_Rectangle (Am_No_Style, Am_On_Bits,
	                          left + kMotDefault + kMotKeySel,
	                          top + kMotDefault + kMotKeySel,
	                          width - 2* (kMotDefault + kMotKeySel),
	                          height - 2* (kMotDefault + kMotKeySel));
    }
    else if (fringe && is_default && !key_selected)
      drawonable->Draw_Rectangle (Am_No_Style, Am_On_Bits,
	                          left + kMotKeySel,
	                          top + kMotKeySel,
	                          width - 2*kMotKeySel,
	                          height - 2*kMotKeySel);
    else
      drawonable->Draw_Rectangle (Am_No_Style, Am_On_Bits, left, top, width,
				  height);
    break;
  
  case Am_WINDOWS_LOOK_val:
    drawonable->Draw_Rectangle (Am_No_Style, Am_On_Bits, left, top, width,
                                height);
    break;

  case Am_MACINTOSH_LOOK_val:
   if (fringe || is_default) {
      if (is_default) {
        Am_Style thick = Am_Style ("black", 3)
                                  .Clone_With_New_Color (Am_On_Bits);
        drawonable->Draw_Roundtangle (thick, Am_No_Style,
                                      left, top, width, height, 8, 8);
      }
      drawonable->Draw_Roundtangle (Am_On_Bits, Am_On_Bits,
                                    left + kMacDefault, top + kMacDefault,
				    width - 2*kMacDefault,
				    height - 2*kMacDefault, 5, 5);
    }
    else
      drawonable->Draw_Roundtangle (Am_On_Bits, Am_On_Bits,
                                    left, top, width, height, 5, 5);
    break;
  
  default:
    Am_Error ("Unknown Look parameter");
    break;
  }
}

/******************************************************************************
 * Am_Draw_Push_Button_Box
 *   Draws a push button box for a particular look and feel
 */

int
Am_Draw_Push_Button_Box(
  am_rect r, // coords in terms of draw
  Am_Drawonable* draw,
  Am_Widget_Look look,
  bool selected, bool /* key_selected */,
  const Computed_Colors_Record& color_rec )
{
  draw->Push_Clip( r.left, r.top, r.width, r.height );
  int inset = 0;
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val: // just use the every useful Am_Draw_Motif_Bo_valx
      Am_Draw_Motif_Box( r.left, r.top, r.width, r.height, selected,
                         color_rec, draw );
      inset = kMotBox;
      break;

    case Am_WINDOWS_LOOK_val:
    {
      // draw the outside rectangle
      Am_Style upper_left  = !selected ? color_rec.data->highlight_style
                                       : color_rec.data->shadow_style;
      Am_Style lower_right = !selected ? color_rec.data->shadow_style
                                       : color_rec.data->shadow_style;
      Am_Draw_Rect_Border( r, upper_left, lower_right, draw );
      Inset_Rect( r, 1 );

      // draw the inside rectangle
      upper_left  = !selected ? color_rec.data->foreground_style
                              : color_rec.data->background_style;
      lower_right = !selected ? color_rec.data->background_style
                              : color_rec.data->background_style;
      Am_Draw_Rect_Border( r, upper_left, lower_right, draw );
      Inset_Rect( r, 1 );

      // draw the fill
      Am_Style fill_style = color_rec.data->foreground_style;
      draw->Draw_Rectangle( Am_No_Style, fill_style, r.left, r.top, r.width,
                            r.height );
      inset = 2; // only inset for the box, not the internal key_selected stuff
      break;
    }

    case Am_MACINTOSH_LOOK_val:
    {
      Am_Style line_style = Am_Style( "black", 1 ); // black and 1 pixel thick
      Am_Style fill_style = !selected ? Am_White : Am_Black;
      draw->Draw_Roundtangle( line_style, fill_style, r.left, r.top, r.width,
                              r.height, 5, 5 );
      inset = kMacBox;
      break;
    }

    default:
      Am_Error ("Unknown Look parameter");
      break;
  } // switch( look )

  draw->Pop_Clip();
  return inset;
}

/******************************************************************************
 * calculate_button_fringe
 *   Calculates the fringe based on the look and if is default or key_selected
 *
 *   The following table specifies how much border is necessary
 *     look     default     key_selected     box
 *     Motif    5           2                5
 *     Win95    0           0                5~
 *     MacOS    4           0                5
 *       ~ 3 pixels are for the key_selected line which is inside the button
 *
 *   By default returns 0
 */

int
calculate_button_fringe(
  Am_Widget_Look look,
  bool leave_room,
  bool key_sel,
  bool def )
{
  if( leave_room )
  {
    key_sel = true;
    def     = true;
  }

  int border = 0;
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      border = (key_sel ? 2*kMotKeySel : 0) + (def ? 2*kMotDefault : 0 );
      break;

    case Am_WINDOWS_LOOK_val:
      border = (key_sel ? 2*kWinKeySel : 0) + (def ? 2*kWinDefault : 0 );
      break;

    case Am_MACINTOSH_LOOK_val:
      border = (key_sel ? 2*kMacKeySel : 0) + (def ? 2*kMacDefault : 0 );
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }

  return border;
}

/******************************************************************************
 * button_width
 *   Calculates and returns the width needed for a button.
 *   1. calculates the height of the string or object.
 *   2. adds 2*box value
 *   3. adds any necessary fringe dimensions (2*fringe)
 *        If Am_LEAVE_ROOM_FOR_FRINGE == true
 *          fringe = default + key_selected
 *        If Am_LEAVE_ROOM_FOR_FRINGE == false
 *          fringe = ( default if Am_DEFAULT == true )
 *                 + ( key_selected if Am_KEY_SELECTED == true )
 *
 *   Then uses the following table to add more border as needed:
 *     look     default     key_selected     box     total
 *     Motif    5           2                5
 *     Win95    0           0                5~
 *     MacOS    4           0                5
 *       ~ 3 pixels are for the key_selected line which is inside the button
 *
 *   By default returns 20
 */

Am_Define_Formula( int, button_width )
{
  Am_String  string;
  Am_Object  obj;
  Am_Value   value;
  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value = self.Peek(Am_REAL_STRING_OR_OBJ);

  if( value.type == Am_STRING )
    string = value;
  else if( value.type == Am_OBJECT )
    obj = value;
  else
    return 20;

  int offset    = self.Get( Am_ITEM_OFFSET );
  bool fringe   = self.Get( Am_LEAVE_ROOM_FOR_FRINGE ),
       key_sel  = self.Get( Am_KEY_SELECTED ),
       def      = self.Get( Am_DEFAULT );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  int border = calculate_button_fringe( look, fringe, key_sel, def );

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      border += 2 * kMotBox;
      break;

    case Am_WINDOWS_LOOK_val:
      border += 2 * kWinBox;
      break;

    case Am_MACINTOSH_LOOK_val:
      border += 2 * kMacBox;
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }

  if( (const char*)string )
  {
    Am_Object  window( self.Get( Am_WINDOW ) );
    Am_Font    font( self.Get( Am_FONT ) );

    Am_Drawonable* draw = GV_a_drawonable( window ); //will work if not valid
    if( draw )
      {
        int str_width, ascent, descent, a, b;
        draw->Get_String_Extents( font, string, strlen( string ),
                                  str_width, ascent, descent, a, b );
        // we get more white vertical than horizontal white space around text
        // so we want to add a few pixels to str_width
        str_width += 3;
        return str_width + border + 2*offset;
      }
  }
  else if( obj.Valid() )
    return (int)obj.Get( Am_WIDTH ) + border + 2*offset;

  return 20;
}

/******************************************************************************
 * button_height
 *   Calculates and returns the height needed for a button.
 *   1. calculates the height of the string or object.
 *   2. adds 2*box value
 *   3. adds any necessary fringe dimensions (2*fringe)
 *        If Am_LEAVE_ROOM_FOR_FRINGE == true
 *          fringe = default + key_selected
 *        If Am_LEAVE_ROOM_FOR_FRINGE == false
 *          fringe = ( default if Am_DEFAULT == true )
 *                 + ( key_selected if Am_KEY_SELECTED == true )
 *
 *   Then uses the following table to add more border as needed:
 *     look     default     key_selected     box     total
 *     Motif    5           2                2       9
 *     Win95    0           0                5~      5
 *     MacOS    4           0                3       7
 *       ~ 3 pixels are for the key_selected line which is inside the button
 *
 *   By default returns 10
 */

Am_Define_Formula( int, button_height )
{
  Am_String string;
  Am_Object obj = 0;
  Am_Value value;
  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value = self.Peek(Am_REAL_STRING_OR_OBJ);
  if( value.type == Am_STRING ) {
    string = value;
    if (strlen( string ) == 0) { //don't use zero length string for height
      string = "W";
    }
  }
  else if( value.type == Am_OBJECT )
    obj = value;
  else
    return 10;

  int offset    = self.Get( Am_ITEM_OFFSET );
  bool fringe   = self.Get( Am_LEAVE_ROOM_FOR_FRINGE ),
       key_sel  = self.Get( Am_KEY_SELECTED ),
       def      = self.Get( Am_DEFAULT );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  int border = calculate_button_fringe( look, fringe, key_sel, def );

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      border += 2 * kMotBox;
      break;

    case Am_WINDOWS_LOOK_val:
      border += 2 * kWinBox;
      break;

    case Am_MACINTOSH_LOOK_val:
      border += 2 * kMacBox;
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }

  if( (const char*)string )
  {
    Am_Object  window( self.Get( Am_WINDOW ) );
    Am_Font    font( self.Get( Am_FONT ) );

    Am_Drawonable* draw = GV_a_drawonable( window ); //will work if not valid
    if( draw )
      {
        int str_width, ascent, descent, a, b;
        draw->Get_String_Extents( font, string, strlen( string ),
                                  str_width, ascent, descent, a, b);
        return ascent + descent + border + 2*offset;
      }
  }
  else if( obj.Valid() )
    return (int)obj.Get( Am_HEIGHT ) + border + 2*offset
           + ((look == Am_WINDOWS_LOOK) ? 6 : 0); // + 6 for key_selection room

  return 10;
}

Am_Define_Method(Am_Object_Method, void,
                 button_abort_method, (Am_Object widget)) {
  Am_Object inter = widget.Get_Object(Am_INTERACTOR);
  Am_Abort_Interactor(inter);
  //now restore the widget's correct value
  //assume the value has already been set, so just invert the current value
  Am_Value v;
  v=widget.Peek(Am_VALUE);
  if (v.Valid()) {
    widget.Set(Am_VALUE, NULL);
    widget.Set(Am_SELECTED, false);
  }
  else {
    v=widget.Peek(Am_LABEL_OR_ID);
    widget.Set(Am_VALUE, v);
    widget.Set(Am_SELECTED, true);
  }
}

/******************************************************************************
 * CHECK BUTTONS
 *****************************************************************************/

/******************************************************************************
 * checkbox_draw
 */

Am_Define_Method(Am_Draw_Method, void, checkbox_draw,
         ( Am_Object self, Am_Drawonable* drawonable,
           int x_offset, int y_offset) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  int box_width = self.Get( Am_BOX_WIDTH );
  int box_height = self.Get( Am_BOX_HEIGHT );
  bool selected = self.Get( Am_SELECTED );
  bool interim_selected = self.Get( Am_INTERIM_SELECTED );
  bool active = self.Get( Am_ACTIVE );
  bool key_selected = self.Get( Am_KEY_SELECTED );
  bool want_final_selected = self.Get( Am_FINAL_FEEDBACK_WANTED );
  bool box_on_left = self.Get( Am_BOX_ON_LEFT );

  Am_Font font(self.Get( Am_FONT ) );
  Am_Alignment align = self.Get( Am_H_ALIGN );

  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  // now find the contents to draw in the button
  Am_String string;
  Am_Object obj = 0;
  Am_Value value;
  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value=self.Peek(Am_REAL_STRING_OR_OBJ);
  if ( value.type == Am_STRING )
    string = value;
  else if( value.type == Am_OBJECT )
    obj = value;
  else
    Am_ERRORO( "String slot of widget " << self
               << " should have string or object type, but value is "
               << value, self, Am_REAL_STRING_OR_OBJ );

  Am_Draw_Button_Widget( left, top, width, height, string, obj,
                         interim_selected, selected && want_final_selected,
                         active, key_selected, false, false, font, color_rec,
                         look, Am_CHECK_BUTTON, drawonable,
                         box_width, box_height, box_on_left, align, 5 );
}

Am_Define_Method(Am_Draw_Method, void, checkbox_mask,
         ( Am_Object self, Am_Drawonable* drawonable,
           int x_offset, int y_offset) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  int box_width = self.Get( Am_BOX_WIDTH );
  int box_height = self.Get( Am_BOX_HEIGHT );
  bool selected = self.Get( Am_SELECTED );
  bool interim_selected = self.Get( Am_INTERIM_SELECTED );
  bool active = self.Get( Am_ACTIVE );
  bool key_selected = self.Get( Am_KEY_SELECTED );
  bool want_final_selected = self.Get( Am_FINAL_FEEDBACK_WANTED );
  bool box_on_left = self.Get( Am_BOX_ON_LEFT );

  Am_Font font(self.Get( Am_FONT ) );
  Am_Alignment align = self.Get( Am_H_ALIGN );

  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  // now find the contents to draw in the button
  Am_String string;
  Am_Object obj = 0;
  Am_Value value;
  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value=self.Peek(Am_REAL_STRING_OR_OBJ);
  if ( value.type == Am_STRING )
    string = value;
  else if( value.type == Am_OBJECT )
    obj = value;
  else
    Am_ERRORO( "String slot of widget " << self
               << " should have string or object type, but value is "
               << value, self, Am_REAL_STRING_OR_OBJ );

  Am_Draw_Button_Widget( left, top, width, height, string, obj,
                         interim_selected, selected && want_final_selected,
                         active, key_selected, false, false, font, color_rec,
                         look, Am_CHECK_BUTTON, drawonable,
                         box_width, box_height, box_on_left, align, 5, true );
}

/******************************************************************************
 * Am_Draw_Check_Button_Box
 *   Draws a check button box for a particular look and feel
 */

void
Am_Draw_Check_Button_Box(
  am_rect r, // coords in terms of draw
  Am_Drawonable* draw,
  Am_Widget_Look look,
  bool selected, bool interim_selected,
  const Computed_Colors_Record& color_rec,
  bool mask = false )
{
  bool depressed = selected || interim_selected;
  draw->Push_Clip( r.left, r.top, r.width, r.height );

  switch( look.value ) {
    case Am_MOTIF_LOOK_val: // just use the every useful Am_Draw_Motif_Bo_valx
	  if (mask)
	    draw->Draw_Rectangle (Am_No_Style, Am_On_Bits, r.left, r.top,
				  r.width, r.height);
	  else
	    Am_Draw_Motif_Box( r.left, r.top, r.width, r.height, depressed,
			       color_rec, draw );
      break;
    case Am_WINDOWS_LOOK_val:
    {
      // draw the outside rectangle
      Am_Style upper_left, lower_right;
      upper_left  = color_rec.data->background_style;
      lower_right = color_rec.data->highlight_style;
	  if (mask)
	    draw->Draw_Rectangle (Am_On_Bits, Am_No_Style, r.left, r.top,
				  r.width, r.height);
	  else
	    Am_Draw_Rect_Border( r, upper_left, lower_right, draw );
      Inset_Rect( r, 1 );

      // draw the inside rectangle
      upper_left  = color_rec.data->shadow_style;
      lower_right = color_rec.data->foreground_style;
	  if (mask)
	    draw->Draw_Rectangle (Am_On_Bits, Am_No_Style, r.left, r.top,
				  r.width, r.height);
	  else
	    Am_Draw_Rect_Border( r, upper_left, lower_right, draw );
      Inset_Rect( r, 1 );

      // draw the fill
      Am_Style fill_style = interim_selected ? color_rec.data->foreground_style
                                             : Am_White;
      draw->Draw_Rectangle( Am_No_Style, fill_style, r.left, r.top, r.width,
                            r.height, mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );

      const float ninth = (float)1/9;
      if( selected ) {
        Am_Style line_style( "black", 1 ); // black and 1 pixel
        float pix      = ninth * r.width,
              left_x   = 2*pix + r.left - 1 + 0.5,
              center_x = 4*pix + r.left - 1 + 0.5,
              right_x  = 8*pix + r.left - 1 + 0.5,
              min_y    = 4*pix + r.top  - 1,
              max_y    = 6*pix + r.top  - 1,
              twopix   = 2*pix;
        for( float y = min_y; y <= max_y; y += 1 )
          draw->Draw_2_Lines( line_style, Am_No_Style,
			      (int) left_x  ,
			      (int) (y + 0.5),
			      (int) center_x,
			      (int) (y + twopix + 0.5),
			      (int) right_x,
			      (int) (y - twopix + 0.5),
			      mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
      }
      break;
    }

    case Am_MACINTOSH_LOOK_val:
    {
      Am_Style line_style( "black", interim_selected ? 2 : 1);
      Am_Style fill_style = Am_White;
      draw->Draw_Rectangle( line_style, fill_style, r.left, r.top, r.width,
                            r.height, mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
      if( selected ) { // draw the x mark
        line_style = Am_Style( "black", 1 );
        draw->Draw_Line( line_style, r.left, r.top, r.left + r.width,
                         r.top + r.height,
			 mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
        draw->Draw_Line( line_style, r.left, r.top + r.height - 1,
                         r.left + r.width - 1, r.top,
			 mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
      }
      break;
    }

    default:
      Am_Error ("Unknown Look parameter");
      break;
  } // switch( look )

  draw->Pop_Clip();
}

/******************************************************************************
 * checkbox_width
 *   calculates the width of the checkbox
 */

Am_Define_Formula( int, checkbox_width )
{
  Am_String  string;
  Am_Object  obj;
  Am_Value   value;
  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  int box_width = self.Get( Am_BOX_WIDTH );
  value = self.Peek(Am_REAL_STRING_OR_OBJ);
  if( value.type == Am_STRING )
    string = value;
  else if( value.type == Am_OBJECT )
    obj = value;
  else
    return 20 + box_width;

  int offset = self.Get( Am_ITEM_OFFSET );

  if( (const char*)string )
  {
    Am_Object window;
    Am_Font font;
    window = self.Get( Am_WINDOW );
    font = self.Get( Am_FONT );
    //GV_a_drawonable will work if not window valid
    Am_Drawonable* draw = GV_a_drawonable( window ); 
    if( draw ) {
        int str_width, ascent, descent, a, b;
        draw->Get_String_Extents( font, string, strlen( string ), str_width,
                                  ascent, descent, a, b);
        return box_width + str_width + 8 + 2*offset;
      }
  }
  else if( obj.Valid() )
  {
    int obj_width = obj.Get( Am_WIDTH );
    return box_width + obj_width + 8 + 2*offset;
  }

  return 20 + box_width;
}

/******************************************************************************
 * checkbox_height
 *   calculates the height of the checkbox
 */

Am_Define_Formula( int, checkbox_height )
{
  Am_String string;
  Am_Object obj = 0;
  Am_Value value;
  int box_height = self.Get( Am_BOX_HEIGHT );
  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value = self.Peek(Am_REAL_STRING_OR_OBJ);
  if( value.type == Am_STRING ) {
    string = value;
    if (strlen( string ) == 0) { //don't use zero length string for height
      string = "W";
    }
  }
  else if(value.type == Am_OBJECT )
    obj = value;
  else
    return box_height + 4;

  int offset = self.Get( Am_ITEM_OFFSET );
  if( (const char*)string )
  {
    Am_Object window;
    Am_Font font;
    window = self.Get( Am_WINDOW );
    font = self.Get( Am_FONT );
    //GV_a_drawonable will work if window not valid
    Am_Drawonable* draw = GV_a_drawonable( window );
    if( draw )
      {
        int str_width, ascent, descent, a, b;
        draw->Get_String_Extents (font, string, strlen( string ), str_width, ascent, descent, a, b);
        int str_height = ascent + descent;
        return (str_height > box_height ? str_height : box_height)
               + 4 + offset + offset;
      }
  }
  else if( obj.Valid() )
  {
    int obj_height = (int)obj.Get(Am_HEIGHT);
    return (obj_height > box_height ? obj_height : box_height)
           + 4 + offset + offset;
  }
  return box_height + 4;
}

/******************************************************************************
 * checkbox_box_width
 *   calculates the width of the checkbox box, which has a dependency upon the
 *   checkbox's look.
 */

Am_Define_Formula (int, checkbox_box_width)
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int width = 0;

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      width = 15;
      break;

    case Am_WINDOWS_LOOK_val:
      width = 13;
      break;

    case Am_MACINTOSH_LOOK_val:
      width = 12;
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }

  return width;
}

/******************************************************************************
 * checkbox_box_height
 *   calculates the height of the checkbox box, which has a dependency upon the
 *   checkbox's look.
 */

Am_Define_Formula( int, checkbox_box_height )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int height = 0;

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      height = 15;
      break;

    case Am_WINDOWS_LOOK_val:
      height = 13;
      break;

    case Am_MACINTOSH_LOOK_val:
      height = 12;
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }

  return height;
}

/******************************************************************************
 * RADIO BUTTONS
 *******************************************************************************/

/******************************************************************************
 * radion_button_draw
 *
 */

Am_Define_Method(Am_Draw_Method, void, radio_button_draw,
         ( Am_Object self, Am_Drawonable* drawonable,
           int x_offset, int y_offset) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  int box_width = self.Get( Am_BOX_WIDTH );
  int box_height = self.Get( Am_BOX_HEIGHT );
  bool selected = self.Get( Am_SELECTED );
  bool interim_selected = self.Get( Am_INTERIM_SELECTED );
  bool active = self.Get( Am_ACTIVE );
  bool key_selected = self.Get( Am_KEY_SELECTED );
  bool want_final_selected = self.Get( Am_FINAL_FEEDBACK_WANTED );
  bool box_on_left = self.Get( Am_BOX_ON_LEFT );
  Am_Font font( self.Get( Am_FONT ) );
  Am_Alignment align = self.Get( Am_H_ALIGN );

  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD);
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  // now find the contents to draw in the button
  Am_String string;
  Am_Object obj;
  Am_Value  value;

  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value=self.Peek(Am_REAL_STRING_OR_OBJ);
  if( value.type == Am_STRING )
    string = value;
  else if( value.type == Am_OBJECT )
    obj = value;
  else
    Am_ERRORO("Label of widget " << self
             << " should have string or object value but it is "
             << value, self, Am_REAL_STRING_OR_OBJ);
  // finally ready to draw it
  Am_Draw_Button_Widget( left, top, width, height, string, obj,
                         interim_selected, selected && want_final_selected,
                         active, key_selected, false, false, font,
			 color_rec, look, Am_RADIO_BUTTON, drawonable,
			 box_width, box_height, box_on_left, align, 5 );
}

Am_Define_Method(Am_Draw_Method, void, radio_button_mask,
         ( Am_Object self, Am_Drawonable* drawonable,
           int x_offset, int y_offset) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  int box_width = self.Get( Am_BOX_WIDTH );
  int box_height = self.Get( Am_BOX_HEIGHT );
  bool selected = self.Get( Am_SELECTED );
  bool interim_selected = self.Get( Am_INTERIM_SELECTED );
  bool active = self.Get( Am_ACTIVE );
  bool key_selected = self.Get( Am_KEY_SELECTED );
  bool want_final_selected = self.Get( Am_FINAL_FEEDBACK_WANTED );
  bool box_on_left = self.Get( Am_BOX_ON_LEFT );
  Am_Font font( self.Get( Am_FONT ) );
  Am_Alignment align = self.Get( Am_H_ALIGN );

  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD);
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  // now find the contents to draw in the button
  Am_String string;
  Am_Object obj;
  Am_Value  value;

  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value=self.Peek(Am_REAL_STRING_OR_OBJ);
  if( value.type == Am_STRING )
    string = value;
  else if( value.type == Am_OBJECT )
    obj = value;
  else
    Am_ERRORO("Label of widget " << self
             << " should have string or object value but it is "
             << value, self, Am_REAL_STRING_OR_OBJ);
  // finally ready to draw it
  Am_Draw_Button_Widget( left, top, width, height, string, obj,
                         interim_selected, selected && want_final_selected,
                         active, key_selected, false, false, font,
			 color_rec, look, Am_RADIO_BUTTON, drawonable,
			 box_width, box_height, box_on_left, align, 5, true );
}

/******************************************************************************
 * Am_Draw_Radio_Button_Box
 *   Draws a radion button box for a particular look and feel
 */

void
Am_Draw_Radio_Button_Box(
  am_rect r, // coords in terms of draw
  Am_Drawonable* draw,
  Am_Widget_Look look,
  bool selected, bool interim_selected,
  const Computed_Colors_Record& color_rec,
  bool mask = false )
{
  bool depressed = selected || interim_selected;
  draw->Push_Clip( r.left, r.top, r.width, r.height );

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
    {
      Am_Style top_fill    = depressed ? color_rec.data->shadow_style
                                       : color_rec.data->highlight_style;
      Am_Style bot_fill    = depressed ? color_rec.data->highlight_style
                                       : color_rec.data->shadow_style;
      Am_Style inside_fill = depressed ? color_rec.data->background_style
                                       : color_rec.data->foreground_style;

      int center_x = r.left + (r.width  + 1)/2 - 1;
      int center_y = r.top  + (r.height + 1)/2 - 1;
      int right    = r.left + r.width  - 1;
      int bottom   = r.top  + r.height - 1;

      // top edges
      draw->Draw_2_Lines( top_fill, Am_No_Style,
                          r.left, center_y,
                          center_x, r.top,
                          right, center_y,
			  mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
      draw->Draw_2_Lines( top_fill, inside_fill,
                          r.left+1, center_y,
                          center_x, r.top+1,
                          right-1, center_y,
			  mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );

      // bottom edges
      draw->Draw_2_Lines( bot_fill, Am_No_Style,
                          r.left, center_y,
                          center_x, bottom,
                          right, center_y,
			  mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
      draw->Draw_2_Lines( bot_fill, inside_fill,
                          r.left+1, center_y,
                          center_x, bottom-1,
                          right-1, center_y,
			  mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
      break;
    }

    case Am_WINDOWS_LOOK_val:
    {
      // draw outer arcs
      draw->Draw_Arc( color_rec.data->background_style, Am_No_Style,
                      r.left, r.top, r.width, r.height, 45, 180,
					  mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
      draw->Draw_Arc( color_rec.data->highlight_style, Am_No_Style,
                      r.left, r.top, r.width, r.height, 225, 180,
					  mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
      Inset_Rect( r, 1 );

      // draw inner arcs - foreground_style if interim_selected, Am_White if
      // not
      Am_Style fill_style = interim_selected ?
                            color_rec.data->foreground_style: Am_White;
      draw->Draw_Arc( color_rec.data->shadow_style, fill_style,
                      r.left, r.top, r.width, r.height, 45, 180,
		      mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );
      draw->Draw_Arc( color_rec.data->foreground_style, fill_style,
                      r.left, r.top, r.width, r.height, 225, 180,
		      mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );

      // draw selected mark
      if( selected ) {
        Inset_Rect( r, 3 ); // 1 pixel for inner circle and 2 pixels of white
        draw->Draw_Arc( Am_No_Style, mask ? Am_On_Bits : Am_Black,
			r.left, r.top, r.width, r.height );
      }
      break;
    }

    case Am_MACINTOSH_LOOK_val:
    {
      // draw the circle - 2 pixels if interim_selected, 1 if not
      Am_Style line_style( "black", interim_selected ? 2 : 1 ); // black
      draw->Draw_Arc( line_style, Am_White, r.left, r.top, r.width,
                      r.height, 0, 360, mask ? Am_DRAW_MASK_COPY : Am_DRAW_COPY );

      // draw selected mark
      if( selected ) {
        Inset_Rect( r, 3 );
        draw->Draw_Arc( Am_No_Style, mask ? Am_On_Bits : Am_Black,
			r.left, r.top, r.width, r.height );
      }
      break;
    }

    default:
      Am_Error ("Unknown Look parameter");
      break;
  } // switch( type )

  draw->Pop_Clip();
}

/******************************************************************************
 * radio_button_diameter
 *   calculates the diameter of a radion button, which has a dependency upon
 *   the radio button's look.
 */

Am_Define_Formula (int, radio_button_diameter)
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int width = 0;

  switch( look.value ) {
    case Am_MOTIF_LOOK_val:
      width = 15;
      break;

    case Am_WINDOWS_LOOK_val:   // Win and Mac diameters are the same
    case Am_MACINTOSH_LOOK_val:
      width = 12;
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }

  return width;
}

/******************************************************************************
 * MENUS
 *******************************************************************************/

/******************************************************************************
 * Menu items by design have the following characteristics

 ---------------------------------------- -
 |                                      |
 |                                      | top_border (above text ascent)
 |                                      | _
 |        About Amulet        ^A        | (ascent+descent)
 |                                      | -
 |                                      | bot_border (below text descent)
 |                                      |
 ---------------------------------------- -
 |        |           |      |  |       |
   left_               accel_     right_
   offset              offset     offset

 * The top_border, bot_border, left_offset, and right_offset in Motif include
 * a two pixel Motif selected box
 *
 * The text "About Amulet" could also be replaced by and object. The offsets
 * and border should remain the same
 *
 * Here are the default values and slot names
 *
 * Slot Name                       Motif     Win95     MacOS
 * Am_MENU_ITEM_TOP_OFFSET         5         3         2
 * Am_MENU_ITEM_BOT_OFFSET         5         5         4
 * Am_MENU_ITEM_LEFT_OFFSET        5         22        15
 * Am_MENU_ITEM_ACCEL_OFFSET       16        9         11
 * Am_MENU_ITEM_RIGHT_OFFSET       5         19        6
 *
 * note that the Motif values include the motif selected box
 *
 *
 * Menus have two special slots
 *
 * Slot Name
 * Am_MENU_BORDER                  2         3         1
 * Am_MENU_LINE_HEIGHT             2         9         16
 * Am_MENU_SELECTED_COLOR - only used with Win95 - defaults to Am_Yellow
 *
 */

/******************************************************************************
 * menu_draw
 */

Am_Define_Method( Am_Draw_Method, void, menu_draw,
         ( Am_Object menu, Am_Drawonable *drawonable,
           int x_offset, int y_offset) )
{
  int left = (int)menu.Get( Am_LEFT ) + x_offset;
  int top = (int)menu.Get( Am_TOP ) + y_offset;
  int width = menu.Get( Am_WIDTH );
  int height = menu.Get( Am_HEIGHT );
  Am_Widget_Look look = menu.Get( Am_WIDGET_LOOK );
  Computed_Colors_Record color_rec = menu.Get( Am_STYLE_RECORD );

  am_rect r( left, top, width, height );

  // first draw the border
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      Am_Draw_Motif_Box( left, top, width, height, false, color_rec,
                         drawonable );
      break;

    case Am_WINDOWS_LOOK_val:
    {
      // draw the outside rectangle
      Am_Style upper_left  = color_rec.data->foreground_style;
      Am_Style lower_right = color_rec.data->shadow_style;
      Am_Draw_Rect_Border( r, upper_left, lower_right, drawonable );
      Inset_Rect( r, 1 );

      // draw the inside rectangle
      upper_left  = color_rec.data->highlight_style;
      lower_right = color_rec.data->background_style;
      Am_Draw_Rect_Border( r, upper_left, lower_right, drawonable );
      Inset_Rect( r, 1 );

      // draw the fill
      Am_Style fill_style = color_rec.data->foreground_style;
      drawonable->Draw_Rectangle( Am_No_Style, fill_style, r.left, r.top,
                                  r.width, r.height );
      break;
    }
    case Am_MACINTOSH_LOOK_val:
      if( menu.Is_Instance_Of( Am_Menu_Bar ) ) // drawing menu bar
        drawonable->Draw_Rectangle( Am_Black, Am_White, r.left-1, r.top-1,
                                    r.width+2, r.height+1 );
      else // drawing a menu
      {
        drawonable->Draw_Rectangle( Am_Black, Am_No_Style, r.left+3, r.top+3,
                                    r.width-3, r.height-3 );
        drawonable->Draw_Rectangle( Am_Black, Am_White, r.left, r.top,
                                    r.width-1, r.height-1 );
      }
      break;
    default:
      Am_Error ("Unknown Look parameter");
      break;
  }

  // now draw the graphical parts of the aggregate, using Am_Aggregate's
  // draw method.
  Am_Draw_Method method;
  method = Am_Aggregate.Get( Am_DRAW_METHOD );
  method.Call( menu, drawonable, x_offset, y_offset );
}

/******************************************************************************
 * menu_mask
 */

Am_Define_Method(Am_Draw_Method, void, menu_mask,
         ( Am_Object menu, Am_Drawonable *drawonable,
           int x_offset, int y_offset) )
{
  int left = (int)menu.Get( Am_LEFT ) + x_offset;
  int top = (int)menu.Get( Am_TOP ) + y_offset;
  int width = menu.Get( Am_WIDTH );
  int height = menu.Get( Am_HEIGHT );
  drawonable->Draw_Rectangle( Am_No_Style, Am_On_Bits, left, top, width,
                              height );
}

/******************************************************************************
 * menu_item_draw
 */

Am_Define_Method( Am_Draw_Method, void, menu_item_draw,
		  ( Am_Object self, Am_Drawonable* draw,
		    int x_offset, int y_offset) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  bool selected = self.Get( Am_SELECTED );
  bool interim_selected = self.Get( Am_INTERIM_SELECTED );
  bool active = self.Get( Am_ACTIVE );
  bool want_final_selected = self.Get( Am_FINAL_FEEDBACK_WANTED );
  Am_Font font( self.Get( Am_FONT ) );
  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  bool draw_selected = selected && want_final_selected;
  bool checked = self.Get(Am_CHECKED_ITEM, Am_RETURN_ZERO_ON_ERROR).Valid();

  // now find the contents to draw in the button
  Am_String string;
  Am_Object obj;
  Am_Value  value;
  bool line = false;

  bool menuBarItem = false;
  if( self.Get_Owner().Is_Instance_Of( Am_Menu_Bar ) )
    menuBarItem = true;

  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value=self.Peek(Am_REAL_STRING_OR_OBJ);

  switch( value.type )
  {
    case Am_STRING:
      string = value;
      break;

    case Am_OBJECT:
      obj = value;
      break;

    case Am_INT: // ok to fall thru ( Am_INT || Am_BOOL )
    case Am_BOOL:
      line = true;
      break;

    default:
      Am_ERRORO("String slot of widget " << self
                << " should have string or object type, but value is " <<
                value, self, Am_REAL_STRING_OR_OBJ);
  }

  Am_String accel_string;
  Am_Value accel_value;
  accel_value=self.Peek(Am_ACCELERATOR_STRING);
  if( accel_value.type == Am_STRING )
    accel_string = accel_value;

  // let's draw it
  if( line ) // if it's just a line, draw it.
  {
    switch( look.value )
    {
      case Am_MOTIF_LOOK_val:
        draw->Draw_Line( color_rec.data->shadow_style, left, top,
                         left+width-1, top );
        draw->Draw_Line( color_rec.data->highlight_style, left, top+1,
                         left+width-1, top+1 );
        break;

      case Am_WINDOWS_LOOK_val:
        draw->Draw_Line( color_rec.data->background_style, left, top+3,
                         left+width-1, top+3 );
        draw->Draw_Line( color_rec.data->highlight_style, left, top+4,
                         left+width-1, top+4 );
        break;

      case Am_MACINTOSH_LOOK_val:
        draw->Draw_Line( Am_Style( "grey", 1 ), left, top+8, left+width-1,
                         top+8 );
        break;

      default:
        Am_Error ("Unknown Look parameter");
        break;
    }
    return;
  }

  if( draw_selected || interim_selected )
    switch( look.value )
    {
      case Am_MOTIF_LOOK_val:
        Am_Draw_Motif_Box( left, top, width, height, false, color_rec, draw );
        break;

      case Am_WINDOWS_LOOK_val:
      {
        Am_Style fill_color = self.Get( Am_MENU_SELECTED_COLOR );
        draw->Draw_Rectangle( Am_No_Style, fill_color, left, top, width,
                              height );
        break;
      }

      case Am_MACINTOSH_LOOK_val:
        draw->Draw_Rectangle( Am_No_Style, Am_Black, left, top, width,
                              height );
        break;

      default:
        Am_Error ("Unknown Look parameter");
        break;
    }

  int leftOffset  = self.Get( Am_MENU_ITEM_LEFT_OFFSET ),
      topOffset   = self.Get( Am_MENU_ITEM_TOP_OFFSET );

  if (checked) {
    draw->Push_Clip( left, top, width, height );
    Am_Style check_style = Am_Line_2;
    if (!active) check_style = am_grey_2;
    else if ((look.value == Am_MACINTOSH_LOOK_val)  &&
	     (interim_selected || (selected && want_final_selected)))
      check_style = am_white_2;
    int check_height = height-4;
    if (check_height > 12) check_height = 12;
    draw->Draw_2_Lines(check_style, Am_No_Style,
		       left+2,  top + (height/2),
		       left+5,  top + ((height+check_height)/2),
		       left+10, top + ((height-check_height)/2));
    draw->Pop_Clip();
  }

  // now draw the string if any
  Am_Style text_style = compute_text_style(active,
					   draw_selected || interim_selected,
					   color_rec, look, Am_MENU_BUTTON);

  int str_width, ascent, descent, a, b, str_left, str_top;


  if( (char*)string )
  {
    draw->Get_String_Extents( font, string, strlen( string ), str_width,
                              ascent, descent, a, b );

    // always left justify the text for now
    str_left = left + leftOffset;
    str_top = top + topOffset;

    // set a clip region in case string bigger than the button
    draw->Push_Clip( left, top, width, height );
          // does not correctly clip top and bottom
    draw->Draw_Text( text_style, string, strlen( string ), font, str_left,
                     str_top );
    draw->Pop_Clip();
  }
  else if( obj.Valid() )
  {
    // left justify the object; since a part of the button, will be offset from
    // buttons' left and top automatically.
    int obj_left = leftOffset;
    int obj_top = topOffset;
    obj.Set( Am_LEFT, obj_left );
    obj.Set( Am_TOP, obj_top );

    // call the object's draw method to draw the component
    bool line_changed = false;
    if( (selected || interim_selected) && look == Am_MACINTOSH_LOOK )
    {
      if( obj.Peek( Am_LINE_STYLE ).Exists() )
      {
        if( obj.Get( Am_LINE_STYLE ) == Am_Black )
        {
          obj.Set( Am_LINE_STYLE, Am_White );
          line_changed = true;
        }
      }
    }

    draw->Push_Clip( left, top, width, height );
    Am_Draw( obj, draw, left, top );
    draw->Pop_Clip();

    if( line_changed )
      obj.Set( Am_LINE_STYLE, Am_Black );
  }

  if( (char*)accel_string )
  {
    // always right justify the accel text
    draw->Get_String_Extents( font, accel_string, strlen( accel_string ),
                              str_width, ascent, descent, a, b );
    str_left = left + width - str_width - 5;
    str_top = top + topOffset;

    // set a clip region in case string bigger than the button
    draw->Push_Clip( left, top, width, height );
    draw->Draw_Text( text_style, accel_string, strlen( accel_string ),
                     font, str_left, str_top );
    draw->Pop_Clip();
  }
}

//Get the window from the widget, or if a popup, get the real widget's
//window.  Normally, would use the Am_SAVED_OLD_OWNER slot, but this
//doesn't work when create an instance of a window with a menubar
//since the Am_SAVED_OLD_OWNER is set *after* the check_accel_string
//constraint is evaluated
Am_Object get_window (Am_Object & widget) {
  Am_Object window = widget.Get(Am_WINDOW);
  if (window.Valid()) {
    if (window.Is_Instance_Of(Am_Pop_Up_Menu_From_Widget_Proto)) {
      Am_Object main_obj = window.Get(Am_FOR_ITEM);
      if (main_obj.Valid())
	window = main_obj.Get(Am_WINDOW);
      else window = Am_No_Object;
    }
  }
  return window;
}
      

/******************************************************************************
 * check_accel_string
 */

Am_Define_String_Formula( check_accel_string )
{
  Am_Value cmd_value;
  Am_Object cmd_obj, old_cmd_obj, old_cmd_window;
  cmd_value=self.Peek(Am_ACCELERATOR_LIST, Am_NO_DEPENDENCY);
  if (cmd_value.Valid()) old_cmd_obj = cmd_value;
  cmd_value=self.Peek(Am_ACCELERATOR_INTER, Am_NO_DEPENDENCY);
  if (cmd_value.Valid()) old_cmd_window = cmd_value;
  cmd_value = self.Peek(Am_COMMAND);
  if( cmd_value.Valid() && cmd_value.type == Am_OBJECT )
  {
    cmd_obj = cmd_value;
    if( cmd_obj.Is_Instance_Of( Am_Command ) )
    {
      Am_Value accel_value;
      Am_Input_Char accel_char;
      accel_value = cmd_obj.Peek(Am_ACCELERATOR);
      if (Am_Input_Char::Test (accel_value))
        accel_char = accel_value;
      else if( accel_value.type == Am_STRING )
      {
        // convert string into right type
        Am_String sval;
        sval = accel_value;
        accel_char = Am_Input_Char ((char*)sval);
        // store it back into the slot so more efficient next time
        // (because no parsing will be needed next time)
        cmd_obj.Set(Am_ACCELERATOR, accel_char);
      }
      //cout << "Widget " << self << " command " << cmd_obj
      //  << " label = " << cmd_obj.Get(Am_LABEL)
      //  << " accel " << accel_value << " char " << accel_char
      //  << endl << flush;
      // now do comparison
      if (accel_char.Valid ()) //then is a legal accelerator character
      {
        char s[Am_LONGEST_CHAR_STRING];
        accel_char.As_Short_String(s);
        Am_Object window = get_window(self);

        if( ( old_cmd_obj != cmd_obj || old_cmd_window != window ) )
        {
          if( old_cmd_obj.Valid() && old_cmd_window.Valid() )
            Am_Remove_Accelerator_Command_From_Window( old_cmd_obj,
                                                       old_cmd_window );
          if( window.Valid() )
            Am_Add_Accelerator_Command_To_Window( cmd_obj, window );
          self.Set( Am_ACCELERATOR_LIST, cmd_obj, Am_OK_IF_NOT_THERE );
          self.Set( Am_ACCELERATOR_INTER, window, Am_OK_IF_NOT_THERE );
        }
	// cout << "Returning str=" << Am_String(s) << endl << flush;
        return Am_String(s);
      }
    }
  }
  if( old_cmd_obj.Valid() && old_cmd_window.Valid() )
    Am_Remove_Accelerator_Command_From_Window( old_cmd_obj, old_cmd_window );

  // no accel string
  return NULL;
}

/******************************************************************************
 * menu_item_height
 */

Am_Define_Formula( int, menu_item_height )
{
  Am_String string;
  Am_Object obj;
  Am_Value  value;

  int topOffset  = self.Get( Am_MENU_ITEM_TOP_OFFSET ),
      botOffset  = self.Get( Am_MENU_ITEM_BOT_OFFSET ),
      lineHeight = self.Get_Owner().Get( Am_MENU_LINE_HEIGHT );

  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value = self.Peek(Am_REAL_STRING_OR_OBJ);
  switch( value.type )
  {
    case Am_STRING:
      string = value;
      break;

    case Am_OBJECT:
      obj = value;
      break;

    case Am_INT:  // ok to fall thru ( Am_INT || Am_BOOL )
    case Am_BOOL:
      return lineHeight;
  }

  if( (char*)string )
  {
    Am_Object window( self.Get( Am_WINDOW ) );
    Am_Font   font( self.Get( Am_FONT ) );

    Am_Drawonable* draw = GV_a_drawonable( window ); //will work if not valid
    if( draw )
      {
        int str_width, ascent, descent, a, b;
        draw->Get_String_Extents( font, string, strlen( string ), str_width,
                                  ascent, descent, a, b );
        return topOffset + ascent + botOffset;
      }
  }
  else if( obj.Valid() )
    return (int)obj.Get( Am_HEIGHT ) + topOffset + botOffset;

  return 10;
}

/******************************************************************************
 * menu_item_width
 */

Am_Define_Formula( int, menu_item_width )
{
  Am_String string;
  Am_Object obj, window;
  Am_Value  value, cmd;
  Am_Font   font;

  window = self.Get( Am_WINDOW );
  font   = self.Get( Am_FONT );
  int str_width, ascent, descent, a, b;
  int accel_width = 0; // includes the offset before the accel string

  int leftOffset  = self.Get( Am_MENU_ITEM_LEFT_OFFSET ),
      accelOffset = self.Get( Am_MENU_ITEM_ACCEL_OFFSET ),
      rightOffset = self.Get( Am_MENU_ITEM_RIGHT_OFFSET );

  Am_Drawonable* draw = GV_a_drawonable( window ); //will work if not valid
  if( draw )
    {
      cmd = self.Peek(Am_COMMAND);
      if( cmd.Valid() && cmd.type == Am_OBJECT )
      {
        Am_Object cmd_obj = (Am_Object)cmd;
        if( cmd_obj.Is_Instance_Of( Am_Menu_Line_Command ) )
          return 0; // don't worry about the width of menu-lines
        else //can only have an accelerator if have a command
        {
          value = self.Peek(Am_ACCELERATOR_STRING);
          if( value.type == Am_STRING )
          {
            Am_String accel_string;
            accel_string = value;
            draw->Get_String_Extents( font, accel_string, strlen( accel_string ),
                                      accel_width, ascent, descent, a, b );
            accel_width += accelOffset;
          }
        }
      }

      // slot contains a formula which gets the real object based on the
      // value of the COMMAND slot
      value = self.Peek(Am_REAL_STRING_OR_OBJ);
      if( value.type == Am_STRING )
        string = value;
      else if( value.type == Am_OBJECT )
        obj = value;
      else
        return 20;
      if( (const char*)string )
      {
        draw->Get_String_Extents( font, string, strlen (string), str_width,
                                  ascent, descent, a, b );
        return leftOffset + str_width + accel_width + rightOffset;
      }
      else if( obj.Valid() )
      {
        int obj_width = obj.Get( Am_WIDTH );
        return leftOffset + obj_width + accel_width + rightOffset;
      }
    }
  //if get here, something wrong, don't have a real size
  return 20;
}

/******************************************************************************
 * menu_border_size
 */

Am_Define_Formula( int, menu_border_size )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      return 2;

    case Am_WINDOWS_LOOK_val:
      return 3;

    case Am_MACINTOSH_LOOK_val:
      return 1;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
  return 0; // default
}

/******************************************************************************
 * menu_item_top_offset
 */

Am_Define_Formula( int, menu_item_top_offset )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      return 5;

    case Am_WINDOWS_LOOK_val:
      return 3;

    case Am_MACINTOSH_LOOK_val:
      return 2;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
  return 0; // we should never get here
}

/******************************************************************************
 * menu_item_bot_offset
 */

Am_Define_Formula( int, menu_item_bot_offset )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      return 5;

    case Am_WINDOWS_LOOK_val:
      return 5;

    case Am_MACINTOSH_LOOK_val:
      return 4;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
  return 0; // we should never get here
}

/******************************************************************************
 * menu_item_left_offset
 */

int menu_item_left_offset_internal(Am_Widget_Look& look, bool menuBarItem) {
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      return menuBarItem ? 5 : 15; //formerly 5

    case Am_WINDOWS_LOOK_val:
      return menuBarItem ? 6 : 22;

    case Am_MACINTOSH_LOOK_val:
      return menuBarItem ? 11 : 15;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
  return 2; // we should never get here
}

Am_Define_Formula( int, menu_item_left_offset )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  bool menuBarItem = false;
  if( self.Get_Owner().Is_Instance_Of( Am_Menu_Bar ) )
                               // is this item in the menu bar
    menuBarItem = true;
  return menu_item_left_offset_internal(look, menuBarItem);
}


/******************************************************************************
 * menu_item_accel_offset
 */

Am_Define_Formula( int, menu_item_accel_offset )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  switch( look.value ) {
    case Am_MOTIF_LOOK_val:
      return 16;

    case Am_WINDOWS_LOOK_val:
      return 9;

    case Am_MACINTOSH_LOOK_val:
      return 11;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
  return 6; // we should never get here
}

/******************************************************************************
 * menu_item_right_offset
 */

Am_Define_Formula( int, menu_item_right_offset )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  bool menuBarItem = false;
  if( self.Get_Owner().Is_Instance_Of( Am_Menu_Bar ) )
                               // is this item in the menu bar
    menuBarItem = true;

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      return 5;

    case Am_WINDOWS_LOOK_val:
      return menuBarItem ? 5 : 19;

    case Am_MACINTOSH_LOOK_val:
      return menuBarItem ? 10 : 6;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
  return 0; // we should never get here
}

/******************************************************************************
 * Am_Active_From_Command
 *   in the active slot of a widget
 */

Am_Define_Formula( bool, Am_Active_From_Command )
{
  bool ret = true;
  Am_Value v;
  v = self.Peek(Am_COMMAND);
  if( v.Valid() && v.type == Am_OBJECT )
  {
    Am_Object cmd = v;
    if( cmd.Is_Instance_Of( Am_Command ) )
      ret = cmd.Get( Am_ACTIVE );
  }
  return ret;
}

/******************************************************************************
 * Am_Active_And_Active2
 *   goes in the active slot of the interactor
 */

Am_Define_Formula( bool, Am_Active_And_Active2 )
{
  bool ret = true;
  Am_Object button = self.Get_Owner(Am_NO_DEPENDENCY);
  if( button.Valid() )
    ret = (bool)button.Get( Am_ACTIVE ) && (bool)button.Get( Am_ACTIVE_2 );
  return ret;
}

Am_Define_Formula( bool, Am_Default_From_Command )
{
  bool ret = false; // assume that we are not default if no command object
  Am_Value v;
  v = self.Peek(Am_COMMAND);
  if( v.Valid() && v.type == Am_OBJECT )
  {
    Am_Object cmd = v;
    if( cmd.Is_Instance_Of( Am_Command ) )
      ret = cmd.Get( Am_DEFAULT );
  }
  return ret;
}

Am_Define_Formula(Am_Value, am_checked_from_command )
{
  Am_Value ret = Am_No_Value; // not checked if no command object
  Am_Value v;
  v = self.Peek(Am_COMMAND);
  if( v.Valid() && v.type == Am_OBJECT ) {
    Am_Object cmd = v;
    if( cmd.Is_Instance_Of( Am_Command ) )
      ret = cmd.Peek( Am_CHECKED_ITEM );
  }
  return ret;
}

/******************************************************************************
 * Am_Get_Owners_Command
 *   goes in the Am_IMPLEMENTATION_PARENT slot of the command in the
 *   interactor to get the Command of the widget
 */

Am_Define_Object_Formula( Am_Get_Owners_Command )
{
  //owner is inter, owner of inter is widget, get command from widget
  Am_Value v;
  Am_Object command;
  v = self.Get_Owner().Get_Owner().Peek ( Am_COMMAND );
  if( v.Valid() && v.type == Am_OBJECT )
  {
    command = v;
    if( !command.Is_Instance_Of( Am_Command ) )
      command = Am_No_Object;
  }
  return command;
}

/******************************************************************************
 * button_sel_from_value
 *   in selected slot of a button widget, for circular constraints so if VALUE
 *   set, the button gets highlighted correctly
 */

Am_Define_Formula( bool, button_sel_from_value )
{
  Am_Value value;
  value = self.Peek(Am_VALUE);
  return value.Valid();
}

/******************************************************************************
 * set_command_from_button
 */

void
set_command_from_button(
  Am_Object parent_command,
  Am_Object button )
{
  Am_Value value = 0;
  if( button.Valid() )
    value=button.Peek(Am_LABEL_OR_ID);
  parent_command.Set( Am_VALUE, value );
}

/******************************************************************************
 * Single button Undo stuff
 *****************************************************************************/

//no repeat on new for button widgets
void
Am_Widget_General_Undo_Redo(
  Am_Object command_obj,
  bool undo,
  bool selective )
{
  Am_Value new_value, old_value;
  Am_Object inter, widget;
  // this command was in the inter in the widget.  The SAVED_OLD_OWNER
  // will be the inter.  Want to actually set the widget.  The
  // old_value in the command object is for the widget, not for the
  // interactor.  The interactor's value is reset by impl_command.
  inter = command_obj.Get(Am_SAVED_OLD_OWNER);
  if (inter.Valid())
    widget = inter.Get_Owner();

  if (selective) { // then get current value from the interactor
    if (widget.Valid()) new_value=widget.Peek(Am_VALUE);
  }
  else // get current value from the command_obj
    new_value=command_obj.Peek(Am_VALUE);

  if (undo) old_value=command_obj.Peek(Am_OLD_VALUE);
  else  // repeat
    old_value=command_obj.Peek(Am_VALUE);

  command_obj.Set(Am_OLD_VALUE, new_value);
  command_obj.Set(Am_VALUE, old_value);
  //also set widget
  if (widget.Valid()) {
    #ifdef DEBUG
    if (Am_Inter_Tracing(Am_INTER_TRACE_SETTING)) {
      cout << "++ ";
      if (selective) cout << "selective ";
      if (undo) cout << "undo ";
      else cout << "repeat ";
      cout << "setting the Am_VALUE of " << widget << " to "
       << old_value << endl << flush;
    }
    #endif
    widget.Set(Am_OLD_VALUE, new_value, Am_OK_IF_NOT_THERE);
    widget.Set(Am_VALUE, old_value);
  }
}

Am_Define_Method(Am_Object_Method, void, Am_Widget_Inter_Command_Undo,
         (Am_Object command_obj)) {
  Am_Widget_General_Undo_Redo(command_obj, true, false);
}
Am_Define_Method(Am_Object_Method, void,
         Am_Widget_Inter_Command_Selective_Undo,
         (Am_Object command_obj)) {
  Am_Widget_General_Undo_Redo(command_obj, true, true);
}
Am_Define_Method(Am_Object_Method, void,
         Am_Widget_Inter_Command_Selective_Repeat,
         (Am_Object command_obj)) {
  Am_Widget_General_Undo_Redo(command_obj, false, true);
}

// Do method for the command object in the interator for the single button.
// Set the value of the widget, me and my parent command
Am_Define_Method(Am_Object_Method, void, button_inter_command_do,
         (Am_Object command_obj)) {
  Am_Value old_value, new_value, inter_value;

  // set the widget's and parent's value.
  Am_Object parent, inter, widget;
  parent = command_obj.Get(Am_IMPLEMENTATION_PARENT);
  inter = command_obj.Get_Owner();
  widget = inter.Get_Owner();
  old_value=widget.Peek(Am_VALUE);
  inter_value=inter.Peek(Am_VALUE);
  if (inter_value.Valid())
    new_value=widget.Peek(Am_LABEL_OR_ID);
  else new_value = NULL;

  Am_INTER_TRACE_PRINT(Am_INTER_TRACE_SETTING,
            "++ DO method setting the Am_VALUE of " << widget << " to "
            << new_value);
  Am_INTER_TRACE_PRINT(Am_INTER_TRACE_SETTING,
            "++ DO method setting the Am_VALUE of " << command_obj << " to "
            << new_value);
  widget.Set(Am_VALUE, new_value);
  command_obj.Set(Am_OLD_VALUE, old_value);
  command_obj.Set(Am_VALUE, new_value);

  if(parent.Valid() ) {
    //set old value to current value
    old_value=parent.Peek(Am_VALUE);
    parent.Set(Am_OLD_VALUE, old_value);
    parent.Set(Am_VALUE, new_value);
  }
}

/******************************************************************************
 * Am_Destroy_Button
 *   demon procedure
 *   see if have an allocated object attached to me, and if so, destroy
 *   it also.
 *   Also removes any accelerators
 */

void
Am_Destroy_Button(
  Am_Object object )
{
  Am_Object attached, command;
  attached = object.Peek_Object (Am_ATTACHED_OBJECT, Am_NO_DEPENDENCY);
  if (attached.Valid() )
    attached.Remove_From_Owner ();

  if( object.Peek( Am_ATTACHED_COMMAND, Am_NO_DEPENDENCY ).Exists())
  {
    Am_Value cmd_v = object.Peek ( Am_COMMAND, Am_NO_DEPENDENCY );
    attached = object.Get( Am_ATTACHED_COMMAND, Am_NO_DEPENDENCY );
    if( cmd_v.type == Am_OBJECT) {
      command = cmd_v;
      if (attached == command)
	command.Remove_From_Owner();
    }
  }
  if( object.Peek( Am_SUB_MENU, Am_NO_DEPENDENCY ).Exists() )
  {
    attached = object.Get( Am_SUB_MENU );
    attached.Destroy();
  }
  Am_Object_Demon* proto_demon =
      ((Am_Object_Advanced&)Am_Aggregate).Get_Demons()
      .Get_Object_Demon (Am_DESTROY_OBJ);
  if( proto_demon )
    proto_demon( object );

  //now handle accelerators, if any
  Am_Value cmd_value;
  Am_Object old_cmd_obj, old_cmd_window;
  cmd_value=object.Peek(Am_ACCELERATOR_LIST, Am_NO_DEPENDENCY);
  if( cmd_value.Valid() )
  {
    old_cmd_obj = cmd_value;
    cmd_value=object.Peek(Am_ACCELERATOR_INTER, Am_NO_DEPENDENCY);
    if( cmd_value.Valid() )
    {
      old_cmd_window = cmd_value;
      Am_Remove_Accelerator_Command_From_Window( old_cmd_obj, old_cmd_window );
    }
  }
}

/******************************************************************************
 * get_button_widget_value
 *  goes in the VALUE slot of the interactor of a single
 *  button, to get the value of the widget.  This is
 *  necessary to set up a circular constraint in case someone sets the value of
 *  the command from the outside.
 */

Am_Define_Object_Formula( get_button_widget_value )
{
  Am_Object widget = self.Get_Owner(); // widget the interactor is in
  if( widget.Valid() )
  {
    Am_Value value;
    value = widget.Peek(Am_VALUE);
    if( value.Valid() )
      return widget;
  }
  return Am_No_Object;
}

/******************************************************************************
 *   put into the Am_LABEL_OR_ID slot of the widget.  If there is a command
 *   and it has an ID field, use that value, otherwise use
 *   Am_REAL_STRING_OR_OBJ value
 */

Am_Define_Formula (Am_Value, Am_Get_Label_Or_ID) {
  Am_Value value;
  //return will be set into value
  value = self.Peek(Am_COMMAND);
  if(value.type == Am_OBJECT) {
    Am_Object cmd = value;
    if( cmd.Is_Instance_Of( Am_Command ) ) {
      value = cmd.Peek(Am_ID);
      if( !value.Valid() )
	value = cmd.Peek(Am_LABEL); //if no ID, then use the label
    }
  }
  //otherwise, value is already set with the correct value to use
  //Note: can't use Am_REAL_STRING_OR_OBJ since it converts values
  //into a string
  return value;
}

/******************************************************************************
 * Button Panels
 *****************************************************************************/

/******************************************************************************
 * set_parts_list_commands_old_owner
 */

int
set_parts_list_commands_old_owner(
  
  Am_Value_List &parts,
  Am_Object &widget )
{
  Am_Value item_value;
  Am_Object part, item;
  int ret = 0; //not used, just for debugging
  for( parts.Start(); !parts.Last(); parts.Next() )
  {
    part = parts.Get();
    item_value = part.Peek(Am_COMMAND);
    if( item_value.type == Am_OBJECT )
    {
      item = (Am_Object)item_value;
      if( item.Is_Instance_Of( Am_Command ) )
      {
        item.Set(Am_SAVED_OLD_OWNER, widget, Am_OK_IF_NOT_THERE);
        ret++;
      }
    }
  }
  return ret;
}

/******************************************************************************
 * Am_Panel_Set_Old_Owner_To_Me
 */

Am_Define_Formula( int, Am_Panel_Set_Old_Owner_To_Me )
{
  int ret = 0;
  Am_Object cmd;
  Am_Value_List parts;
  cmd = self.Get_Object(Am_COMMAND);
  if( cmd.Valid() )
  {
    cmd.Set( Am_SAVED_OLD_OWNER, self, Am_OK_IF_NOT_THERE );
    ret = 1;
  }
  parts = self.Get( Am_GRAPHICAL_PARTS );
  ret = set_parts_list_commands_old_owner( parts, self );

  //ret not used, different values just for debugging
  return ret;
}

//Button in a panel is active if owner is and if specific command object is
Am_Define_Formula(bool, active_from_command_panel) {
  Am_Object owner = self.Get_Owner();
  if (owner.Valid() && !(bool)owner.Get(Am_ACTIVE)) return false;
  //now check my command object, if any
  Am_Value v;
  v = self.Peek(Am_COMMAND);
  if (v.Valid() && v.type == Am_OBJECT) {
    Am_Object cmd = v;
    if (cmd.Is_Instance_Of(Am_Command)) return cmd.Get(Am_ACTIVE);
  }
  return true;
}

//copy look from owner
Am_Define_Formula(Am_Value, look_from_owner )
{
  return self.Get_Owner().Get( Am_WIDGET_LOOK );
}

Am_Define_Font_Formula( Am_Font_From_Owner )
{
  return self.Get_Owner().Get( Am_FONT );
}

Am_Define_Formula( bool, final_feedback_from_owner )
{
  return self.Get_Owner().Get( Am_FINAL_FEEDBACK_WANTED );
}

// goes in the interactor for a button panel
Am_Define_Formula(Am_Value, Am_How_Set_From_Owner )
{
  return self.Get_Owner().Get( Am_HOW_SET );
}
Am_Define_Formula( bool, box_on_left_from_owner )
{
  return self.Get_Owner().Get( Am_BOX_ON_LEFT );
}

Am_Define_Formula( int, box_width_from_owner )
{
  return self.Get_Owner().Get( Am_BOX_WIDTH );
}

Am_Define_Formula( int, box_height_from_owner )
{
  return self.Get_Owner().Get( Am_BOX_HEIGHT );
}

Am_Define_Formula(Am_Value, Am_Align_From_Box_On_Left )
{
  return (bool)self.Get( Am_BOX_ON_LEFT ) ? Am_LEFT_ALIGN : Am_RIGHT_ALIGN;
}

Am_Define_Formula( int, Am_Left_From_Owner )
{
  return self.Get_Owner().Get( Am_LEFT );
}

Am_Define_Formula( int, Am_Top_From_Owner)
{
  return self.Get_Owner().Get( Am_TOP );
}

Am_Define_Formula( int, Am_Width_From_Owner )
{
  return self.Get_Owner().Get( Am_WIDTH );
}

Am_Define_Formula( int, Am_Height_From_Owner )
{
  return self.Get_Owner().Get( Am_HEIGHT );
}

// Panel item (width, height) calculate the (width, height) of an
// item in a button panel or menu by checking its owner's fixed_width slot.
// 0 or false means, use real width.
// 1 or true means use max. width (from owner).
// Otherwise, set it to that int value
Am_Define_Formula(int, panel_item_width) {
  Am_Object owner = self.Get_Owner();
  if (owner.Valid()) {
    Am_Value fw;
    fw = owner.Peek(Am_FIXED_WIDTH);
    if (fw.type == Am_BOOL) {
      if ((bool)fw) return owner.Get(Am_MAX_WIDTH);
      else return self.Get(Am_REAL_WIDTH);
    }
    else // not Am_BOOL
      if (fw.type == Am_INT) {
	int n = fw;
	if (n == 0) return self.Get(Am_REAL_WIDTH);
	if (n == 1) return owner.Get(Am_MAX_WIDTH);
	return self.Get(Am_FIXED_WIDTH);
      }
    //    else if (!fw.Exists())
    //  return 0;
      else { // neither int nor bool: error.
	Am_Error("Am_Panel_Item_Width: wrong type for Am_FIXED_WIDTH slot.",
		 self, Am_FIXED_WIDTH);
	return 0;
      }
  }
  else return self.Get(Am_REAL_WIDTH);
}

Am_Define_Formula( int, panel_item_height )
{
  Am_Object owner = self.Get_Owner();
  if( owner.Valid() )
  {
    Am_Value fh;
    fh = owner.Peek(Am_FIXED_HEIGHT);
    if( fh.type == Am_BOOL )
      if( (bool)fh )
        return owner.Get( Am_MAX_HEIGHT );
      else
      {
        int height = self.Get( Am_REAL_HEIGHT );
        return height;
      }
    else // not Am_BOOL
      if( fh.type == Am_INT )
      {
        int n = fh;
        if( n == 0 )
          return self.Get( Am_REAL_HEIGHT );
        if( n == 1 )
          return owner.Get( Am_MAX_HEIGHT );
        return self.Get( Am_FIXED_HEIGHT );
      }
    //    else if (!fh.Exists())
    // return 0;
      else // neither int nor bool: error.
      {
        Am_Error("Am_Panel_Item_Height: wrong type for Am_FIXED_HEIGHT slot",
		 self, Am_FIXED_HEIGHT);
        return 0;
      }
  }
  else return self.Get(Am_REAL_HEIGHT);
}

//  The height is just the vertical extent of the menu's parts, plus border
//  width.

Am_Define_Formula( int, menu_height )
{
  // based on Am_map_height in opal.cc
  // the border depends upon the widget look;

  Am_Value_List components = self.Get (Am_GRAPHICAL_PARTS);
  int border = self.Get( Am_MENU_BORDER );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  int height = 0;
  Am_Object item;

  for (components.Start (); !components.Last (); components.Next ()) {
    item = components.Get( );
    if( (bool)item.Get (Am_VISIBLE) )
    {
      int item_top    = item.Get (Am_TOP);
      int item_height = item.Get (Am_HEIGHT);
      int item_bottom = item_top + item_height;
      if( item_bottom > height )
        height = item_bottom;
    }
  }

  if( look == Am_MACINTOSH_LOOK )
    border++;
  return height + border; // we are adding the botton border, Am_TOP_OFFSET takes
                          // care of the top border
}

Am_Define_Formula( int, menu_width )
{
  // based on Am_map_width in opal.cc
  // the border depends upon the widget look;

  Am_Value_List components = self.Get( Am_GRAPHICAL_PARTS );
  int border = self.Get( Am_MENU_BORDER );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  int width  = 0;
  Am_Object item;

  for( components.Start (); !components.Last (); components.Next() )
  {
    item = components.Get();
    if( (bool)item.Get( Am_VISIBLE ) )
    {
      int item_left  = item.Get( Am_LEFT );
      int item_width = item.Get( Am_WIDTH );
      int item_right = item_left + item_width;
      if( item_right > width )
        width = item_right;
    }
  }

  if( look == Am_MACINTOSH_LOOK )
    border++;
  return width + border; // we are adding the right border, Am_LEFT_OFFSET takes
                         // care of the left border
}

Am_Define_Formula( int, menu_line_height )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      return 2;

    case Am_WINDOWS_LOOK_val:
      return 9;

    case Am_MACINTOSH_LOOK_val:
      return 16;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
  return 2; // should never get here
}

// Max Item Height simply calculates maximum width of all parts, not maximum
// extents of the objects.
// potential circular constraint: dependancy on Am_WIDTH instead of
// Am_REAL_WIDTH in some cases.

Am_Define_Formula( int, max_item_width )
{
  // based on Am_map_width in opal.cc
  // finds width of widest visible item

  Am_Value_List components;
  components = self.Get( Am_GRAPHICAL_PARTS );
  int width = 0;
  Am_Object item;
  for( components.Start(); !components.Last(); components.Next() )
  {
    item = components.Get();
    if( (bool)item.Get( Am_VISIBLE ) )
    {
      int item_width;
      if( item.Get_Slot_Type( Am_REAL_WIDTH ) == Am_INT )
        item_width = item.Get( Am_REAL_WIDTH );
      else
        item_width = item.Get( Am_WIDTH );
      if( item_width > width )
        width = item_width;
    }
  }
  return width;
}

Am_Define_Formula( int, max_item_height )
{
  // based on Am_map_width in opal.cc
  // finds width of widest visible item

  Am_Value_List components;
  components = self.Get( Am_GRAPHICAL_PARTS );
  int height = 0;
  for( components.Start(); !components.Last(); components.Next() )
  {
    Am_Object item;
    item = components.Get();
    if( (bool)item.Get( Am_VISIBLE ) )
    {
      int item_height;
      if( item.Get_Slot_Type( Am_REAL_HEIGHT ) == Am_INT )
        item_height = item.Get( Am_REAL_HEIGHT );
      else
        item_height = item.Get( Am_HEIGHT );
      if( item_height > height )
        height = item_height;
    }
  }
  return height;
}

//used by Am_Copy_Item_To_Command and Am_Menu_Bar_Copy_Item_To_Command
//  Converts the Am_ITEM slot set by the map into a command part or value
static void copy_item_to_command_proc(Am_Object& panel_item,
				      const Am_Value& value ) {
  if( value.type == Am_OBJECT && Am_Object( value ).Valid() &&
      Am_Object( value ).Is_Instance_Of( Am_Command ) )
  {
    // then is a command
    Am_Object command = value;
    Am_Object owner = command.Get_Owner(Am_NO_DEPENDENCY);
    if( owner == panel_item )
    {
      if( panel_item.Get_Object( Am_COMMAND, Am_NO_DEPENDENCY ) == command )
        return; // already fine
      else
        panel_item.Remove_Part( command ); // part of me in the wrong slot??
    }
    else if( owner.Valid() ) // make new instance, leave command where it is
      command = command.Create();

    if (panel_item.Is_Part_Slot(Am_COMMAND)) {
      // prevent deleting the prior command
      panel_item.Remove_Part( Am_COMMAND );
    }
    //Set_Part removes old value if any
    panel_item.Set_Part(Am_COMMAND, command, Am_OK_IF_NOT_THERE);
    panel_item.Set(Am_ATTACHED_COMMAND, command, Am_OK_IF_NOT_THERE);
  }
  else // not a command object, add to COMMAND slot as a value
  {
    if (panel_item.Is_Part_Slot(Am_COMMAND )) {
      // prevent deleting the prior command
      panel_item.Remove_Part( Am_COMMAND );
    }
    panel_item.Set(Am_COMMAND, value, Am_OK_IF_NOT_THERE);
  }
}

// Get the ITEM value set by the Map.
// If value is a command object, add it as a part otherwise just set
// the Command slot with it
Am_Define_Formula (Am_Value, Am_Copy_Item_To_Command)
{
  Am_Value value;
  value = self.Peek(Am_ITEM);
  //cout << "regular copy_item_to_command for " << self << " value = " << value
  //     << endl << flush;
  copy_item_to_command_proc( self, value );
  return value;
}

// For the where test of interactor: Find a component of owner which is active.
Am_Define_Method(Am_Where_Method, Am_Object, Am_In_Active_Widget_Part,
         (Am_Object /*inter*/, Am_Object object, Am_Object event_window,
          int x, int y))
{
  Am_Object result = Am_No_Object;
  if (Am_Point_In_All_Owners(object, x, y, event_window))
    result = Am_Point_In_Part (object, x, y, event_window);
  if (result.Valid() ) {
    Am_Value value;
    value=result.Peek(Am_ACTIVE);
    // if slot exists and is zero then return 0 (if slot doesn't
    // exist, return result)
    if (value.Exists() && !value.Valid())
      return Am_No_Object;
    else
      return result;
  }
  else return Am_No_Object;
}

bool inter_value_is_or_contains(Am_Object inter, Am_Object new_object)
{
  Am_Value inter_value;
  inter_value=inter.Peek(Am_VALUE);
  if (Am_Value_List::Test(inter_value)) { // is a list
    Am_Value_List inter_list;
    inter_list = inter_value;
    inter_list.Start();
    if (inter_list.Member(new_object)) return true;
    else return false;
  }
  else { //not list
    Am_Object val;
    val = inter_value;
    if (val == new_object) return true;
    else return false;
  }
}

//assign to the panel value based on the interactor's value.
void set_panel_value_from_inter_value(Am_Object inter)
{
  Am_Object panel = inter.Get_Owner();
  if (!(inter.Valid())) return;
  Am_Value inter_value, label_or_id, value;
  inter_value=inter.Peek(Am_VALUE);
  // cout << "** setting from inter " << inter  << " value " << inter_value
  //      << endl << flush;
  if (Am_Value_List::Test(inter_value)) // is a list
  {
    Am_Value_List inter_list, panel_list;
    inter_list = inter_value;
    Am_Object item;
    for (inter_list.Start(); !inter_list.Last(); inter_list.Next())
    {
      item = inter_list.Get();
      label_or_id=item.Peek(Am_LABEL_OR_ID);
      panel_list.Add(label_or_id);
    }
    value = panel_list;
  } //end if value is a list
  else //value is a single value, should be an object or null
  {
    if (inter_value.Valid())
    {
      Am_Object value_obj;
      value_obj = inter_value;
      value=value_obj.Peek(Am_LABEL_OR_ID);
    }
    else
      value = 0;
  }
  Am_INTER_TRACE_PRINT(Am_INTER_TRACE_SETTING,
            "++ Panel DO method setting the Am_VALUE of " << panel
            << " to " << value);
  panel.Set(Am_VALUE, value);
}

// do method for the command in the interactor in the button panel.
// Sets the panel widget's value,
// set the IMPLEMENTATION_PARENT slot of me
// set the Am_VALUE slot of me and of the parent command
Am_Define_Method(Am_Object_Method, void, Am_Inter_For_Panel_Do,
         (Am_Object command_obj))
{
  Am_Object new_object, item, item_command, panel, panel_command,
    parent_command;
  Am_Object inter = command_obj.Get_Owner(); // owner will be interactor
  Am_Value old_value, new_value, cmd_obj_value;
  if (inter.Valid() ) {
    //get old_value
    panel = inter.Get_Owner(); // panel the interactor is in
    old_value=panel.Peek(Am_VALUE);
    panel.Set(Am_OLD_VALUE, old_value);
    set_panel_value_from_inter_value(inter);
    //set command_obj's slots
    new_value=panel.Peek(Am_VALUE);
    command_obj.Set(Am_OLD_VALUE, old_value);
    Am_INTER_TRACE_PRINT(Am_INTER_TRACE_SETTING,
            "++ DO method setting the Am_VALUE of " << command_obj << " to "
            << new_value);
    command_obj.Set(Am_VALUE, new_value);

    new_object = inter.Get(Am_INTERIM_VALUE);
    // new_object is a sub-widget (an individual button)
    if (panel.Valid()) {
      cmd_obj_value=panel.Peek(Am_COMMAND);
      if (cmd_obj_value.type == Am_OBJECT) {
	panel_command = cmd_obj_value;
	if (!(panel_command.Is_Instance_Of(Am_Command)))
	  panel_command = Am_No_Object;
      }
    }

    if (new_object.Valid() ) {
      cmd_obj_value=new_object.Peek(Am_COMMAND);
      if (cmd_obj_value.type == Am_OBJECT) {
	item_command = cmd_obj_value;
	if (item_command.Is_Instance_Of(Am_Command)) {
	  item = new_object;
	  parent_command = item_command;
	  //set the value of the item_command to the item's label_or_id or 0
	  if (inter_value_is_or_contains(inter, new_object))
	    new_value=new_object.Peek(Am_LABEL_OR_ID);
	  else new_value = 0;
	}
      else item_command = Am_No_Object; // wrong type object
      }
    }
    if (!item_command.Valid() ) { //not in the item, try to find a global cmd
      if (panel_command.Valid()) {
	parent_command = panel_command;
	//set the value of the panel_command to be the same as the panel's
	new_value=panel.Peek(Am_VALUE);
      }
    }
    old_value=parent_command.Peek(Am_VALUE);
    parent_command.Set(Am_OLD_VALUE, old_value);
    Am_INTER_TRACE_PRINT(Am_INTER_TRACE_SETTING,
            "++ DO method setting the Am_VALUE of parent "
			 << parent_command << " to " << new_value);
    parent_command.Set(Am_VALUE, new_value);

    // set my parent to either the item's or panel's command object
    command_obj.Set(Am_IMPLEMENTATION_PARENT, parent_command);
  }
}

Am_Define_Method(Am_Object_Method, void,
                 button_panel_abort_method, (Am_Object widget)) {
  Am_Object inter = widget.Get_Object(Am_INTERACTOR);
  Am_Abort_Interactor(inter);
  //now restore the widget's correct value
  Am_Value v;
  v=widget.Peek(Am_OLD_VALUE);
  //cout << "... Button panel " << widget << " aborting; set value to " << v
  //  << endl << flush;
  widget.Set(Am_VALUE, v);
}

//arbitrarily use first part as the start object
Am_Define_Method(Am_Explicit_Widget_Run_Method, void,
         widget_first_member_start_method,
         (Am_Object widget, Am_Value initial_value)) {
  Am_Object inter = widget.Get_Object(Am_INTERACTOR);
  if (initial_value.Valid()) widget.Set(Am_VALUE, initial_value);
  Am_Value_List parts = widget.Get(Am_GRAPHICAL_PARTS);
  parts.Start();
  Am_Object first_obj = parts.Get();
  Am_Start_Interactor(inter, first_obj);
}

///////////////////////////////////////////////////////////////////////////
// Constraints for button panels
///////////////////////////////////////////////////////////////////////////

void get_inter_value_from_panel_value(const Am_Value& panel_value,
                      const Am_Object& panel,
                      
                      Am_Value &value) {
  Am_Value_List parts, panel_value_list, inter_value_list;
  Am_Value v, label_or_id;
  Am_Object inter, item;
  bool is_list = false;
  bool found_it = false;
  int panel_value_found_count = 0;
  v=panel.Peek(Am_GRAPHICAL_PARTS, Am_NO_DEPENDENCY);
  if (!v.Valid()) return;
  parts = v;
  value = NULL; //initialize return value
  if (panel_value.Valid() && Am_Value_List::Test(panel_value)) { // is a list
    panel_value_list = panel_value;
    is_list = true;
  }
  for (parts.Start(); !parts.Last(); parts.Next()) {
    item = parts.Get();
    label_or_id = item.Peek(Am_LABEL_OR_ID);
    if (is_list) {
      panel_value_list.Start();
      if (panel_value_list.Member(label_or_id)) { // then this one should be on
	item.Set(Am_SELECTED, true);
	inter_value_list.Add(item);
	panel_value_found_count++;
      }
      else  // this one should be off
	item.Set(Am_SELECTED, false);
    }
    else { // panel value not a list, should be Am_LABEL_OR_ID
      if (panel_value == label_or_id) { // then this one should be on
	item.Set(Am_SELECTED, true);
	value = item;
	found_it = true;
      }
      else  // this one should be off
	item.Set(Am_SELECTED, false);
    }
  } // end for parts
  if( is_list ) {
    value = inter_value_list;
#if 0  //this testing doesn't work, move to a Set_Type_Check
    int unused = panel_value_list.Length() - panel_value_found_count;
    if (unused != 0) {
      cerr << "** Amulet WARNING: Value List " << panel_value
	   << " which was set into widget "
	   << panel << " seems to contain " << unused
	   << " item(s) that do not match one the values in its "
	   "Am_ITEMS list.\n" << flush;
    }
#endif
  }
  else
  {
#if 0 // this testing doesn't work, move to a Set_Type_Check **
    if( !found_it ) {
      // unfortunately, can't raise an error because panels are often
      // initialized with illegal values temporarily
      if (panel_value.Valid())) {
      cerr << "** Amulet WARNING: Value " << panel_value
	   << " (type=" << Am_Get_Type_Name(panel_value.type)
	   << ") which was set into widget " << panel
	  << " seems to not be one the values in its Am_ITEMS list\n" << flush;
      }
      return;  // don't remove this line!
      // gcc 2.7.0 on HP requires it (otherwise compiler bug causes a crash)
      // -- rcm
    }
#endif
  return;
  }
}

//Calculate inter value from panel's value.  Used when
//the programmer sets the panel's value.  Also sets the SELECTED slot
Am_Define_Formula (Am_Value, inter_value_from_panel_value) {
  Am_Value value;
  Am_Object panel = self.Get_Owner();
  if (!panel.Valid()) {
    return value;
  }
  Am_Value panel_value;
  panel_value = panel.Peek(Am_VALUE);
  //cout << "** computing inter " << self << " value from panel value " <<
  //  panel_value << endl << flush;
  get_inter_value_from_panel_value(panel_value, panel, value);
  return value;
}

///////////////////////////////////////////////////////////////////////////
// Menu_bars
///////////////////////////////////////////////////////////////////////////

// DESIGN:
// The top-level Am_ITEMS slot should contain a list of command
// objects.  Unlike other menus and panels, the first level members of
// the value_list in the Am_ITEMS slot MUST be command objects. The
// Am_LABEL of each of these command objects will be the top-level
// names of the sub-menus.  Each of these command objects should
// contain a Am_ITEMS slot containing the sub-menu items, which can be
// strings, objects or commands like regular menus and panels.
//
// The top level menu_bar is a horizontal menu.  Each item is set with a
// Am_SUB_MENU slot containing the window containing a Am_SUB_MENU part
// containing a vertical menu.  The interactor in the top level menu_bar works
// over all the windows, and its interim_do deals with turning the visibility
// on and off.  The individual menu items know how to take the right part of
// the Am_ITEM list for their own use.

Am_Define_Formula(int, sub_menu_set_old_owner)
{
  Am_Object window, for_item, menu_bar;
  window = self.Get_Owner();
  int ret = 0;
  if( window.Valid() )
  {
    for_item = window.Get( Am_FOR_ITEM );
    if( for_item.Valid() )
    {
      menu_bar = for_item.Get_Owner();
      if( menu_bar.Valid() )
      {
        Am_Value_List parts;
        parts = self.Get( Am_GRAPHICAL_PARTS );
        ret = set_parts_list_commands_old_owner( parts, menu_bar );
      }
    }
  }
  return ret;
}

Am_Define_Formula( int, menu_bar_width )
{
  return self.Get_Owner().Get( Am_WIDTH );
}

/******************************************************************************
 * menu_bar_height
 *   Based on height_of_parts_procedure in opal.cc.
 */

Am_Define_Formula( int, menu_bar_height )
{
  int max_y = 0, comp_bottom = 0;
  int border = self.Get( Am_MENU_BORDER );
  Am_Widget_Look look   = self.Get( Am_WIDGET_LOOK );
  Am_Value_List components;
  Am_Object comp;
  components = self.Get( Am_GRAPHICAL_PARTS );
  for( components.Start(); !components.Last(); components.Next() )
  {
    comp = components.Get();
    // compute how much of the component extends below the origin
    comp_bottom = ((int)comp.Get( Am_TOP ) + (int)comp.Get( Am_HEIGHT ));
    if( comp_bottom > max_y )
      max_y = comp_bottom;
  }

  int inset_border = 0;
  if( look == Am_MOTIF_LOOK )
          inset_border = 3;

  return max_y + border + inset_border;
}

/******************************************************************************
 * menu_bar_h_spacing
 */

Am_Define_Formula( int, menu_bar_h_spacing )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  return ( look == Am_MACINTOSH_LOOK ) ? -4 : 0;
}

/******************************************************************************
 * menu_bar_x_offset
 */

Am_Define_Formula( int, menu_bar_x_offset )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int border = self.Get( Am_MENU_BORDER );
  if( look == Am_MOTIF_LOOK )
    return border + 3;
  else
    return border;
}

/******************************************************************************
 * menu_bar_y_offset
 */

Am_Define_Formula( int, menu_bar_y_offset )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int border = self.Get( Am_MENU_BORDER );
  if( look == Am_MOTIF_LOOK )
    return border + 3;
  else
    return border;
}

/******************************************************************************
 * menu_bar_sub_win_top
 */

Am_Define_Formula( int, menu_bar_sub_win_top )
{
  Am_Object for_item = self.Get(Am_FOR_ITEM);
  Am_Object menu_bar = for_item.Get_Owner();
  Am_Widget_Look look   = menu_bar.Get( Am_WIDGET_LOOK );
  int height = menu_bar.Get( Am_HEIGHT );

  int overlap = 0;
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      overlap = 5;
      break;

    case Am_WINDOWS_LOOK_val:
      overlap = 3;
      break;

    case Am_MACINTOSH_LOOK_val:
      overlap = 1;
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }

  if( for_item.Valid() && menu_bar.Valid() )
  {
    int x = 0; //initializations so no compiler warnings for translate_coords
    int y = 0;
    // get the coordinates of the bottom of the menu item w.r.t. the screen
    Am_Translate_Coordinates( menu_bar, 0, height - overlap, Am_Screen, x, y);
    return y;
  }
  else
    return 0;
}

/******************************************************************************
 * menu_bar_sub_win_left
 */

Am_Define_Formula( int, menu_bar_sub_win_left )
{
  Am_Object for_item = self.Get(Am_FOR_ITEM);
  if( for_item.Valid() )
  {
    int x = 0; //initializations so no compiler warnings for translate_coords
    int y = 0;
    // get the coordinates of the left of the menu item w.r.t. the screen
    Am_Translate_Coordinates(for_item, 0, 0, Am_Screen, x, y);
    return x;
  }
  else
    return 0;
}

Am_Define_Formula( int, popup_sub_win_width )
{
  return self.Get_Object( Am_SUB_MENU ).Get( Am_WIDTH );
}

Am_Define_Formula( int, popup_sub_win_height )
{
  return self.Get_Object( Am_SUB_MENU ).Get( Am_HEIGHT );
}

Am_Define_Object_Formula( popup_sub_win_undo_handler )
{
  Am_Object for_item, main_window, undo_handler;
  for_item = self.Get( Am_FOR_ITEM );
  if( for_item.Valid() )
  {
    Am_Value v;
    v = for_item.Peek(Am_WINDOW);
    if( v.Valid() )
    {
      main_window = v;
      if( main_window.Valid() )
      {
        v = main_window.Peek(Am_UNDO_HANDLER);
        if( v.Valid() )
          undo_handler = v;
      }
    }
  }
  return undo_handler;
}

Am_Object Am_Menu_Bar_Sub_Window_Proto; //defined below

void create_sub_menu_window(
  Am_Object menu_item )
{
  Am_Object new_window = Am_Menu_Bar_Sub_Window_Proto.Create();
  new_window.Set( Am_FOR_ITEM, menu_item );
  Am_Screen.Add_Part( new_window );
  menu_item.Set( Am_SUB_MENU, new_window, Am_OK_IF_NOT_THERE );
}

//Put into the Am_ITEM_TO_COMMAND slot of each top-level item of menu_bar.
// Gets the ITEM value set by the Map. If value is a command object, add it as
// a part.  Also creates the sub-menus
Am_Define_Formula (Am_Value, menu_bar_copy_item_to_command)
{
  Am_Value value;
  value = self.Peek(Am_ITEM);
  //if (!value.Exists()) return 0;
  if( value.type == Am_OBJECT && Am_Object (value).Valid() &&
      Am_Object (value).Is_Instance_Of(Am_Command) )
  {
    //then fine
  }
  else
  {
    Am_ERRORO("In a menu_bar, the top-level items must be\n"
         << "command objects, but for " << self << " item # "
         << (int)self.Get(Am_RANK) + 1 << " is " << value, self, Am_ITEM);
  }
  copy_item_to_command_proc(self, value);

  //now create the sub-menu if necessary
  if (self.Get_Slot_Type (Am_SUB_MENU) != Am_OBJECT)
    create_sub_menu_window(self);
  return value;
}

// formula for the items list of a sub-menu: get the list from the Am_FOR_ITEM
// of my window.
Am_Define_Formula (Am_Value, sub_menu_items) {
  Am_Value value;
  value = 0;
  Am_Object window, for_item;
  window = self.Get(Am_WINDOW);
  if (window.Valid()) {
    for_item = window.Get(Am_FOR_ITEM);
    if (for_item.Valid()) {
      Am_Object cmd;
      cmd = for_item.Get(Am_ITEM);
      // all top-level items must be commands
      if (cmd.Valid()) {
    // then set the return value of this formula with the contents of the
    // Am_ITEMS slot of the command.
    value = cmd.Peek(Am_ITEMS);
    if (!value.Valid() )
          value = 0;
      }
    }
  }
  return value;
}

//main_win required if vis == true; otherwise not needed
// If the main window is a modal window, then the popped up window
// needs to be modal as well so it can be operated.
void set_popup_win_visible(Am_Object &pop_window, bool vis,
			   Am_Object &main_win = Am_No_Object) {
  pop_window.Set(Am_VISIBLE, vis);
  if (vis) {
    Am_To_Top(pop_window);
    Am_Value v = main_win.Get(Am_WAITING_FOR_COMPLETION,
			      Am_RETURN_ZERO_ON_ERROR);
    pop_window.Set(Am_WAITING_FOR_COMPLETION, v, Am_OK_IF_NOT_THERE);
    if (v.Valid() && (int)v == (int)Am_INTER_WAITING_MODAL) {
      Am_Push_Modal_Window(pop_window);
    }
  }
  else {
    Am_Value v = pop_window.Peek(Am_WAITING_FOR_COMPLETION);
    if (v.Valid() && (int)v == (int)Am_INTER_WAITING_MODAL) {
      Am_Remove_Modal_Window(pop_window);
    }
  }
}

//set the sub-window visibility, and also deal with interim selected of the
//associated main item
void set_sub_window_vis(Am_Object &sub_window, bool vis, Am_Object &main_win) {
  set_popup_win_visible(sub_window, vis, main_win);
  Am_Object for_item = sub_window.Get(Am_FOR_ITEM);
  if (for_item.Valid()) for_item.Set(Am_INTERIM_SELECTED, vis);
}

// For the where test of the interactor in the menu bar: Work over all windows
// and items, and pop-up sub-windows when necessary.
Am_Define_Method(Am_Where_Method, Am_Object, in_menu_bar_item,
         (Am_Object inter, Am_Object /* object */,
          Am_Object event_window, int x, int y)) {
  Am_Object menu_bar = inter.Get_Owner ();
  Am_Object result = Am_No_Object;
  Am_Object menu_bar_window, old_window, new_window;
  Am_Value v;
  // get the old sub_window which used to be visible
  v=inter.Peek(Am_SUB_MENU);
  if (v.Valid()) old_window = v;
  menu_bar_window = menu_bar.Get(Am_WINDOW);
  if (menu_bar_window == event_window) {
    // in the top level window,
    result = Am_Point_In_Part (menu_bar, x, y, event_window);
    // now deal with visibility of sub-windows
    if (result.Valid()) {
      v=result.Peek(Am_SUB_MENU);
      if (v.Valid())
        new_window = v;
    }
    if (new_window != old_window && new_window.Valid()) {
      if (old_window.Valid())
	set_sub_window_vis(old_window, false, menu_bar_window);
      if (new_window.Valid()) 
	set_sub_window_vis(new_window, true, menu_bar_window);
      inter.Set(Am_SUB_MENU, new_window, Am_OK_IF_NOT_THERE);
    }
    return result;  //don't test if top-level menu items are active!!
  }
  else { //must be in a sub-menu
    Am_Object sub_menu = event_window.Get_Object(Am_SUB_MENU);
    if (sub_menu.Valid())
      result = Am_Point_In_Part (sub_menu, x, y, event_window);
    // test if active
    if (result.Valid() ) {
      Am_Value value;
      value=result.Peek(Am_ACTIVE);
      // if slot exists and is zero then return 0.
      // If slot does NOT exist, return result.
      if (value.Exists() && !value.Valid())
        return Am_No_Object;
      else
        return result;
    }
  }
  return Am_No_Object;
}

//menu_bar has a list of parts which are Am_Item_In_Menu and each one should
//have a Am_SUB_MENU slot which contains the appropriate sub-menu
Am_Define_Value_List_Formula(menu_bar_window_list) {
  Am_Object menu_bar = self.Get_Owner();
  Am_Object menu_bar_main_win = menu_bar.Get(Am_WINDOW);
  Am_Value_List window_list;
  Am_Value_List components;
  Am_Object comp;
  Am_Value v;
  components = menu_bar.Get(Am_GRAPHICAL_PARTS);
  if (menu_bar_main_win.Valid())
    window_list.Add(menu_bar_main_win); // main window must be there also
  for (components.Start(); !components.Last(); components.Next()) {
    comp = components.Get( );
    v = comp.Peek(Am_SUB_MENU);
    if (v.Valid()) {
      window_list.Add(v);
    }
  }
  return window_list;
}

//A custom destroy demon for menu bar to destroy the menu popup windows.
void Am_Destroy_Menu_Bar (Am_Object object)
{
  Am_Value_List parts;
  Am_Object part, sub_menu;
  parts = object.Get( Am_GRAPHICAL_PARTS);
  for (parts.Start (); !parts.Last (); parts.Next ()) {
    part = parts.Get( );
    sub_menu = part.Get( Am_SUB_MENU);
    sub_menu.Destroy ();
  }
  Am_Object_Demon* proto_demon = ((Am_Object_Advanced&)Am_Menu).Get_Demons ()
      .Get_Object_Demon (Am_DESTROY_OBJ);
  if (proto_demon)
    proto_demon (object);
}

//current object should be already set into Am_INTERIM_VALUE and old
//value in Am_OLD_INTERIM_VALUE.  This will replace the choice_inter's interim
//do method.
Am_Define_Method(Am_Object_Method, void, menu_bar_inter_interim_do,
         (Am_Object inter)) {
  Am_Object old_object, new_object, menu_bar_win, old_object_win,
    new_object_win, main_item;
  menu_bar_win = inter.Get_Owner().Get(Am_WINDOW);
  old_object = inter.Get(Am_OLD_INTERIM_VALUE);
  new_object = inter.Get(Am_INTERIM_VALUE);
  if (new_object.Valid() )
    new_object.Set(Am_INTERIM_SELECTED, true);
  if (old_object.Valid() ) {
    old_object_win = old_object.Get(Am_WINDOW);
    //never clear the Am_INTERIM_SELECTED of top-level items
    if (old_object_win != menu_bar_win)
      old_object.Set(Am_INTERIM_SELECTED, false);
  }
}

// returns main-item, which might be obj if this is a main-item
Am_Object clear_interim_sel(Am_Object obj, Am_Object menu_bar_win) {
  Am_Object obj_win, main_item;
  if (obj.Valid() ) {
    obj.Set(Am_INTERIM_SELECTED, false);
    obj_win = obj.Get(Am_WINDOW);
    if (obj_win.Valid() &&
    obj_win != menu_bar_win) { // then clear old main menu selection
      main_item = obj_win.Get(Am_FOR_ITEM);
      if (main_item.Valid())
    main_item.Set(Am_INTERIM_SELECTED, false);
    }
    else main_item = obj;
  }
  return main_item;
}

Am_Define_Method(Am_Object_Method, void, menu_bar_inter_abort,
         (Am_Object inter)) {
  Am_Object obj, menu_bar_win, sub_window;
  menu_bar_win = inter.Get_Owner().Get(Am_WINDOW);
  obj = inter.Get(Am_OLD_INTERIM_VALUE);
  clear_interim_sel(obj, menu_bar_win);
  obj = inter.Get(Am_INTERIM_VALUE);
  clear_interim_sel(obj, menu_bar_win);
  // now make sub_window go away
  sub_window = inter.Get(Am_SUB_MENU);
  if (sub_window.Valid()) set_sub_window_vis(sub_window, false, menu_bar_win);
  inter.Set(Am_SUB_MENU, Am_No_Object, Am_OK_IF_NOT_THERE);
}

Am_Define_Method(Am_Object_Method, void, menu_bar_inter_do,
         (Am_Object inter)) {
  Am_Object new_object, parent_command, item_command, main_item_command,
    main_item, menu_bar, sub_window, menu_bar_win;
  Am_Value value;
  new_object = inter.Get(Am_INTERIM_VALUE);
  menu_bar = inter.Get_Owner();
  menu_bar_win = menu_bar.Get(Am_WINDOW);

  // make sub_window go away
  sub_window = inter.Get(Am_SUB_MENU);
  if (sub_window.Valid()) set_sub_window_vis(sub_window, false, menu_bar_win);
  inter.Set(Am_SUB_MENU, Am_No_Object, Am_OK_IF_NOT_THERE);

  // if click on top-level item, it might not be valid, so check first
  if (new_object.Valid()) {
    value=new_object.Peek(Am_ACTIVE);
    // if slot exists and is zero then return 0.
    // If slot does NOT exist, return result.
    if (value.Exists() && !value.Valid()) {
      Am_Abort_Interactor(inter);
      return;
    }
    // else continue
  }

  // Now the standard choice interactor stuff.
  //   clear interim selection of sub-item and main item.  Returns
  //   main-item, which might be new_object if it is a main-item
  main_item = clear_interim_sel(new_object, menu_bar_win);

  // sets the interactor's command value and SELECTED of the individual widgets
  Am_Choice_Set_Value(inter, true);
  inter.Set(Am_OBJECT_MODIFIED, new_object);

  // now find and set the right parent command object
  // This code is like Am_Inter_For_Panel_Do
  if (main_item.Valid()) {
    value=main_item.Peek(Am_COMMAND);
    if (value.type == Am_OBJECT) {
      main_item_command = value;
      if (!main_item_command.Is_Instance_Of(Am_Command))
	main_item_command = 0;  //wrong type object
    }
  }

  if (new_object.Valid() ) {
    value=new_object.Peek(Am_COMMAND);
    if (value.type == Am_OBJECT) {
      item_command = value;
      if (item_command.Is_Instance_Of(Am_Command))
	parent_command = item_command;
      else item_command = 0; // wrong type object
    }
  }
  
  if (!item_command.Valid() ) { //not in the item, use main item
    parent_command = main_item_command;
  }

  // set the value field of the parent_command
  set_command_from_button(parent_command, new_object);

  // install the found command into my command object's parent
  value=inter.Peek(Am_COMMAND);
  if (value.type == Am_OBJECT) {
    Am_Object command_obj = value;
    if (command_obj.Is_Instance_Of(Am_Command)) {
      command_obj.Set(Am_IMPLEMENTATION_PARENT, parent_command);
    }
  }
}

//this goes in the window for the sub-menu
Am_Define_Style_Formula (popup_fill_from_for_item) {
  Am_Value v;
  v = self.Get_Object(Am_FOR_ITEM).Peek(Am_FILL_STYLE);
  if (v.Valid()) return v;
  return Am_Motif_Light_Blue;
}

// the next three go into the sub-menu itself.  FOR_ITEM is in my window
Am_Define_Formula (Am_Value, sub_menu_look_from_win_for_item) {
  Am_Value v;
  v = self.Get_Object(Am_WINDOW).Get_Object(Am_FOR_ITEM).Peek(Am_WIDGET_LOOK);
  if (v.Valid()) return v;
  else return Am_MOTIF_LOOK;
}

/******************************************************************************
 * sub_menu_font_from_win_for_item
 */

Am_Define_Font_Formula(sub_menu_font_from_win_for_item)
{
  Am_Value v;
  v = self.Get_Object(Am_WINDOW).Get_Object(Am_FOR_ITEM).Peek(Am_FONT);
  if( v.Valid() )
    return v;
  else
    return Am_Default_Font;
}

/******************************************************************************
 * sub_menu_font_from_win_for_item
 * Button in a menu_bar is active if owner and main-menu is, and if specific
 * command object is
 */

Am_Define_Formula( bool, menu_bar_sub_item_active )
{
  Am_Object owner = self.Get_Owner();
  if( owner.Valid() && !(bool)owner.Get(Am_ACTIVE) )
    return false;
  // now check the main-item I am attached to, if any
  Am_Object win;
  win = self.Get(Am_WINDOW);
  if (win.Valid())
  {
    Am_Object main_item;
    main_item = win.Get(Am_FOR_ITEM);
    if( main_item.Valid() && !(bool)main_item.Get(Am_ACTIVE) )
      return false;
  }
  //now check my command object, if any
//   Am_Object cmd = self.GV_Object(Am_COMMAND);
//   if( cmd.Valid() && cmd.Is_Instance_Of(Am_Command) )
//     return cmd.GV(Am_ACTIVE);
//   else
//     return true;
  Am_Value cmd_value = self.Peek(Am_COMMAND);
  if (cmd_value.type == Am_OBJECT) {
    Am_Object cmd = cmd_value;
    if( cmd.Is_Instance_Of(Am_Command) )
      return cmd.Get(Am_ACTIVE);
  }
  return true;
}

/******************************************************************************
 * Option Button
 *    single button, but pops up a menu of choices
 ******************************************************************************/

/******************************************************************************
 * option_button_items
 *   self is menu, window's for_item is main option button, get its ITEMS
 */

Am_Define_Formula (Am_Value, option_button_items)
{
  Am_Value value;
  value = 0;
  Am_Value v;
  Am_Object o;
  v = self.Peek(Am_WINDOW);
  if( v.Valid() )
  {
    o = v;
    v = o.Peek(Am_FOR_ITEM);
    if( v.Valid() )
    {
      o = v;
      value = o.Peek(Am_ITEMS);
    }
  }
  return value;
}

/******************************************************************************
 * Am_Option_Button_Sub_Window_Proto
 *   a prototype
 */

Am_Object Am_Option_Button_Sub_Window_Proto; // defined below

/******************************************************************************
 * create_sub_menu_proc
 */

int create_sub_menu_proc(Am_Object& self ) {
  Am_Object new_window = self.Get(Am_SUB_MENU, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
  if (new_window.Valid())
    Am_Error("create_sub_menu called but already has a menu");
  new_window = Am_Option_Button_Sub_Window_Proto.Create()
    .Set(Am_FOR_ITEM, self);
  Am_Screen.Add_Part(new_window);
  self.Set(Am_SUB_MENU, new_window, Am_OK_IF_NOT_THERE);
  return -1;
}

/******************************************************************************
 * create_sub_menu
 */

Am_Formula create_sub_menu( create_sub_menu_proc, "create_sub_menu" );

/******************************************************************************
 * option_sub_win_top
 */

Am_Define_Formula( int, option_sub_win_top )
{
  Am_Object for_item = self.Get( Am_FOR_ITEM );
  bool fringe   = for_item.Get( Am_LEAVE_ROOM_FOR_FRINGE ),
       key_sel  = for_item.Get( Am_KEY_SELECTED ),
       def      = for_item.Get( Am_DEFAULT );
  Am_Widget_Look look = for_item.Get( Am_WIDGET_LOOK );

  int border = calculate_button_fringe( look, fringe, key_sel, def ) / 2;
  
  if( for_item.Valid() )
  {
    //find top of item in my menu that is currently selected
    int x = 0;
    int y = 0;
    Am_Value inter_value;
    inter_value = for_item.Peek(Am_COMPUTE_INTER_VALUE);
    if( inter_value.Valid() )
    {
      Am_Object sel_menu_item = inter_value;
      y = (int)sel_menu_item.Get( Am_TOP, Am_NO_DEPENDENCY );
    }
    // get the coordinates of the current item w.r.t. the screen
    Am_Translate_Coordinates( for_item, 0, -y + border + 1, Am_Screen, x, y );
    return y;
  }
  else
    return 0;
}

/******************************************************************************
 * option_sub_win_left
 */

Am_Define_Formula( int, option_sub_win_left )
{
  Am_Object for_item = self.Get( Am_FOR_ITEM );
  bool fringe   = for_item.Get( Am_LEAVE_ROOM_FOR_FRINGE ),
       key_sel  = for_item.Get( Am_KEY_SELECTED ),
       def      = for_item.Get( Am_DEFAULT );
  Am_Widget_Look look = for_item.Get( Am_WIDGET_LOOK );

  int border = calculate_button_fringe( look, fringe, key_sel, def ) / 2;

  if( for_item.Valid() )
  {
    int x = 0; //initializations so no compiler warnings for translate_coords
    int y = 0;
    // get the coordinates of the left of the menu item w.r.t. the screen
    Am_Translate_Coordinates(for_item, border, 0, Am_Screen, x, y);
    return x;
  }
  else
    return 0;
}

/******************************************************************************
 * get_value_to_use_for_value_from
 *  sets value with the value to use and returns true if active or false if not
*/

bool
get_value_to_use_for_value_from(
  Am_Value& value)
{
  if( value.type == Am_OBJECT )
  {
    Am_Object cmd = value;
    if( cmd.Is_Instance_Of( Am_Menu_Line_Command ) )
    {
      value = true; // this takes care of the menu line case
      return false;
    }
    else if( cmd.Is_Instance_Of( Am_Command ) )
    {
      // then get the value out of the command object
      value = cmd.Peek(Am_ID);
      if (!value.Valid())
        value = cmd.Peek(Am_LABEL);
      Am_Value active_value;
      active_value = cmd.Peek(Am_ACTIVE);
      return active_value.Valid();
    }
    else
      return true; //value is set with a non-command object, use it
  }
  else
    return true; //value is not an object, just use value as is
}

/******************************************************************************
 * option_button_value
 *   get the value out of my sub-menu, if any, otherwise use first active item
 */

Am_Define_Formula (Am_Value, option_button_value)
{
  Am_Value value;
  Am_Object sub_menu = self.Get_Object(Am_SUB_MENU).Get_Object(Am_SUB_MENU);
  value = sub_menu.Peek(Am_VALUE);
  if( !value.Valid() )
  {
    Am_Value v;
    v = self.Peek(Am_ITEMS);
    value = Am_No_Value;
    if( v.Valid() && Am_Value_List::Test(v) )
    {
      Am_Value_List items = v;
      Am_Value firstv;
      //search for a valid item
      items.Start();
      firstv = items.Get(); //save first item
      for( ; !items.Last (); items.Next() )
      {
        value = items.Get();
        if (value.Valid()) {
          if (get_value_to_use_for_value_from(value)) return value;
        }
      }
      //if get here, then all items are inactive, just use first one
      value = firstv;
      get_value_to_use_for_value_from(value);
    }
  }
  return value;
}

const int NOTCH_OFFSET = 8; //distance from edge of button to notch
const int NOTCH_WIDTH = 12;
const int NOTCH_HEIGHT = 8;

/******************************************************************************
 * option_button_width
 *   width of menu, plus notch offset plus notch size
 */

Am_Define_Formula( int, option_button_width )
{
  Am_Object sub_menu_win = self.Get( Am_SUB_MENU );
  int menu_width = sub_menu_win.Get( Am_WIDTH );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      return menu_width + NOTCH_OFFSET + NOTCH_WIDTH + NOTCH_OFFSET;

    case Am_WINDOWS_LOOK_val:
      return menu_width + 16;

    case Am_MACINTOSH_LOOK_val:
      return menu_width + 23;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
  return 20;
}


/******************************************************************************
 * option_button_draw
 */

Am_Define_Method( Am_Draw_Method, void, option_button_draw,
                ( Am_Object self, Am_Drawonable* draw,
                  int x_offset, int y_offset ) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  bool active = self.Get( Am_ACTIVE );
  bool key_selected = self.Get( Am_KEY_SELECTED );
  bool fringe = self.Get( Am_LEAVE_ROOM_FOR_FRINGE );
  Am_Font font;
  font = self.Get( Am_FONT );
  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  // now find the contents to draw in the button
  Am_String string;
  Am_Object obj;
  Am_Value value;

  // string slot contains a formula which gets the real object based on the
  // value of the COMMAND slot
  value=self.Peek(Am_REAL_STRING_OR_OBJ);
  if (value.type == Am_STRING)
    string = value;
  else if (value.type == Am_OBJECT)
    obj = value;
  else
     Am_ERRORO("String slot of widget " << self
               << " should have string or object type, but value is " <<
               value, self, 0);

  int offset = self.Get( Am_MENU_ITEM_LEFT_OFFSET );

  // finally ready to draw it
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
    {
      Am_Draw_Button_Widget( left, top, width, height, string, obj,
                             false, false, active, key_selected, false,
                             fringe, font, color_rec,
                             look, Am_PUSH_BUTTON, draw, 0, 0, false,
                             Am_LEFT_ALIGN, offset );

      //now draw notch
      int x = left + width - NOTCH_OFFSET - NOTCH_WIDTH - 4;
      //center in Y
      int y = top + ( height - NOTCH_HEIGHT ) / 2;
      draw->Push_Clip( left, top, width, height );
      Am_Draw_Motif_Box( x, y, NOTCH_WIDTH, NOTCH_HEIGHT, false, color_rec, draw );
      draw->Pop_Clip();
      break;
    }

    case Am_WINDOWS_LOOK_val:
      Am_Draw_Button_Widget( left, top, width, height, string, obj,
                             false, false, active, key_selected, false,
                             fringe, font, color_rec,
                             look, Am_PUSH_BUTTON, draw, 0, 0, false,
                             Am_LEFT_ALIGN, offset );
      draw_down_arrow( left+width-16, top, 16, height,
                       Am_WINDOWS_LOOK, false, true, true,
                       color_rec, draw );
      break;

    case Am_MACINTOSH_LOOK_val:
    {
      if( fringe )
      {
        left   += 4;
        top    += 4;
        width  -= 8;
        height -= 8;
      }

      draw->Draw_Rectangle( Am_Black, Am_No_Style, left+3, top+3, width-3, height-3 );
      draw->Draw_Rectangle( Am_Black, Am_White, left, top, width-1, height-1 );

      left   += 1;
      top    += 1;
      width  -= 3;
      height -= 3;

      Am_Style style = active ? Am_Black : Am_Motif_Inactive_Stipple;

      if( (const char*)string )
      {
        int str_width, ascent, descent, a, b, str_left, str_top;

        draw->Get_String_Extents( font, string, strlen( string ), str_width,
                                  ascent, descent, a, b);

        str_left = offset + left;
        str_top  = top + (height - ascent - descent) / 2;

        draw->Draw_Text( style, string, strlen( string ), font, str_left,
                         str_top );

      }
      else if( obj.Valid() )
      {
        int obj_width  = obj.Get( Am_WIDTH ),
            obj_height = obj.Get( Am_HEIGHT ),
            obj_left   = ( width - 20 - obj_width ) / 2,
            obj_top    = 2 + ( height - obj_height ) / 2;

        obj.Set( Am_LEFT, obj_left );
        obj.Set( Am_TOP,  obj_top  );
        Am_Draw( obj, draw, left, top );
      }

      // draw little down arrow
      int tri_x = left + width - 15,
          tri_y = top  + ( height - 6 ) / 2,
          len   = 10; // len - is really 11, but one extra pixel is drawn to the right
      for( register long i = 1; i <= 6; i++ )
      {
        draw->Draw_Line( style, tri_x, tri_y, tri_x + len, tri_y );
        tri_x++;
        tri_y++;
        len -= 2;
      }
      break;
    }

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
}

/******************************************************************************
 * option_button_start_do
 *   displays sub-menu and starts its interactor
 */

Am_Define_Method( Am_Object_Method, void, option_button_start_do,
                ( Am_Object inter) )
{
  Am_Object option_button = inter.Get_Owner();
  Am_Object sub_menu_window = option_button.Get(Am_SUB_MENU);
  if( sub_menu_window.Valid() )
  {
    Am_Object main_win = option_button.Get(Am_WINDOW);
    set_popup_win_visible(sub_menu_window, true, main_win);
    Am_Object inter = sub_menu_window.Get_Object(Am_SUB_MENU)
      .Get_Object(Am_INTERACTOR);
    Am_Value inter_value;
    inter_value=option_button.Peek(Am_COMPUTE_INTER_VALUE);
    Am_Start_Interactor(inter, inter_value);
  }
}

/******************************************************************************
 * hide_sub_menu
 */

void hide_sub_menu(Am_Object command_obj ) {
  Am_Object inter = command_obj.Get_Owner(); // owner will be interactor
  if (inter.Valid() )
  {
    Am_Object window = inter.Get_Owner().Get(Am_WINDOW);
    if( window.Valid() )
      set_popup_win_visible(window, false);
  }
}

/******************************************************************************
 * option_sub_menu_do
 */

Am_Define_Method( Am_Object_Method, void, option_sub_menu_do,
                ( Am_Object command_obj ) )
{
  //standard stuff
  Am_Inter_For_Panel_Do_proc(command_obj);
  //make my menu disappear
  hide_sub_menu(command_obj);
}

/******************************************************************************
 * option_hide_sub_menu
 */

Am_Define_Method( Am_Object_Method, void, option_hide_sub_menu,
                ( Am_Object command_obj ))
{
  hide_sub_menu(command_obj);
}

/******************************************************************************
 * command_from_for_item
 */

Am_Define_Object_Formula( command_from_for_item )
{
  return self.Get_Owner().Get_Object( Am_FOR_ITEM ).Get( Am_COMMAND );
}

/******************************************************************************
 *   A custom destroy demon for option button to destroy the menu popup window.
 */

void destroy_option_button (Am_Object object)
{
  Am_Object sub_menu = object.Get(Am_SUB_MENU);
  sub_menu.Destroy ();

  //now run the regular button destroy demon, if any
  Am_Object_Demon* proto_demon = ((Am_Object_Advanced&)Am_Button).Get_Demons()
      .Get_Object_Demon (Am_DESTROY_OBJ);
  if( proto_demon )
    proto_demon( object );
}

/******************************************************************************
 *   A custom destroy demon for option button to destroy the menu popup window.
 */

void destroy_popup_interactor (Am_Object object)
{
  Am_Object sub_menu = object.Get(Am_SUB_MENU);
  sub_menu.Destroy ();

  //now run the regular button destroy demon, if any
  Am_Object_Demon* proto_demon =
      ((Am_Object_Advanced&)Am_One_Shot_Interactor).Get_Demons()
      .Get_Object_Demon (Am_DESTROY_OBJ);
  if( proto_demon )
    proto_demon( object );
}

/******************************************************************************
 * compute_inter_value
 */

Am_Define_Formula (Am_Value, compute_inter_value)
{
  Am_Value value;
  Am_Value panel_value, v;
  Am_Object panel = self.Get_Object( Am_SUB_MENU ).Get_Object( Am_SUB_MENU );
  panel_value = self.Peek(Am_VALUE);
  if( !panel.Valid() )
  {
    value = NULL;
    return value;
  }
  get_inter_value_from_panel_value( panel_value, panel, value );
  return value;
}

/******************************************************************************
 * get_real_string_from_inter_val
 */

Am_Define_Formula (Am_Value, get_real_string_from_inter_val)
{
  Am_Value value;
  Am_Value v;
  v = self.Peek(Am_COMPUTE_INTER_VALUE);
  if( v.Valid() )
  {
    Am_Object button = v;
    value = button.Peek(Am_REAL_STRING_OR_OBJ);
  }
  return value;
}

/******************************************************************************
 * option_button_window_list
 */

Am_Define_Value_List_Formula( option_button_window_list )
{
  Am_Value_List window_list;
  Am_Object menu = self.Get_Owner();
  if( menu.Valid() )
  {
    Am_Object menu_win = menu.Get(Am_WINDOW);
    if( menu_win.Valid() )
    {
      Am_Object option_button = menu_win.Get_Object(Am_FOR_ITEM);
      if( option_button.Valid() )
      {
        Am_Object option_button_window = option_button.Get(Am_WINDOW);
        if( option_button_window.Valid() )
          window_list.Add(menu_win)
                     .Add(option_button_window);
      }
    }
  }
  return window_list;
}

/******************************************************************************
 * option_button_abort_method
 */

Am_Define_Method( Am_Object_Method, void, option_button_abort_method,
                ( Am_Object widget ) )
{
  //just abort the sub-menu widget.  My value has a constraint to the sub-menu
  //so sub-menu's abort will make my value be correct
  Am_Object sub_menu_widget = widget.Get_Object(Am_SUB_MENU)
    .Get_Object(Am_SUB_MENU);
  Am_Abort_Widget(sub_menu_widget);
}

////////////////////////////////////////////////////////////////////////
// PopUp Menus
////////////////////////////////////////////////////////////////////////

Am_Object Am_Pop_Up_Menu_Sub_Window_Proto; // defined below

int create_popup_sub_menu_proc(Am_Object& self ) {
  Am_Value v;
  v=self.Peek(Am_SUB_MENU, Am_NO_DEPENDENCY);
  if (v.Valid())
    Am_Error("create_popup_sub_menu called but already has a menu");
  Am_Object new_window = Am_Pop_Up_Menu_Sub_Window_Proto.Create()
    .Set(Am_FOR_ITEM, self);
  Am_Screen.Add_Part(new_window);
  self.Set(Am_SUB_MENU, new_window, Am_OK_IF_NOT_THERE);
  return -1;
}

Am_Formula create_popup_sub_menu(create_popup_sub_menu_proc,
				 "create_popup_sub_menu");

Am_Define_Method(Am_Mouse_Event_Method, void, popup_inter_start_do,
                ( Am_Object inter, int x, int y, Am_Object ref_obj,
		  Am_Input_Char /* ic */) ) {
  Am_Object sub_menu_window = inter.Get(Am_SUB_MENU);
  if( sub_menu_window.Valid() ) {
    Am_Translate_Coordinates(ref_obj, x, y, Am_Screen, x, y);
    sub_menu_window.Set(Am_LEFT, x);
    sub_menu_window.Set(Am_TOP, y);
    Am_Object main_win = inter.Get(Am_WINDOW);
    set_popup_win_visible(sub_menu_window, true, main_win);
    Am_Object sub_inter = sub_menu_window.Get_Object(Am_SUB_MENU)
      .Get_Object(Am_INTERACTOR);
    Am_Start_Interactor(sub_inter);
  }
  Am_Abort_Interactor(inter); //so the do method isn't called on start
}

/******************************************************************************
 * Am_Get_Real_String_Or_Obj
 *   Formula to get the real object or string to use in the widget. It can be
 *   directly in the Am_COMMAND slot or there can be a command object there,
 *   and then the real value is in the command's Am_LABEL slot.
 */

Am_Define_Formula (Am_Value, Am_Get_Real_String_Or_Obj) {
  Am_Value value, v;
  Am_Object obj, command;
  value = self.Peek(Am_COMMAND);
  if (!value.Valid() ) {
    // The Am_COMMAND slot is empty, just return NULL
    value = Am_No_Object;
    return value;
  }
  else if (value.type == Am_OBJECT) {
      obj = value;
      if(obj.Is_Instance_Of( Am_Command )) {
	command = obj;
	obj = Am_No_Object;
	if( command.Is_Instance_Of( Am_Menu_Line_Command ) )
	  value = true; // this takes care of the menu line case
	else {  // get the value out of the command object
	  value = command.Peek(Am_LABEL);
	  if( value.type == Am_STRING ) ; // have a string in value
	  else if( value.type == Am_OBJECT )
	    obj = value;  // have a new object
	  else { //generate a string from the value by printing it
	    char line[100];
	    OSTRSTREAM_CONSTR  (oss,line, 100, ios::out);
	    oss << value << ends;
	    OSTRSTREAM_COPY(oss,line,100);
	    value = line; //now use the string as the value
	  }
	}
      }
      else { //not a command, must be a regular object
	if (!obj.Is_Instance_Of( Am_Graphical_Object ))
	  Am_ERRORO("Object " << obj << " being added as part of widget "
		    << self
		    << " should be a Command or Graphical Object", self,
		    Am_COMMAND);
	value = obj;
      }
    } //else not an object
    else if (value.type == Am_STRING) ; // value is already set correctly
    else { //generate a string from the value by printing it
      char line[100];
      OSTRSTREAM_CONSTR (oss,line, 100, ios::out);
      oss << value << ends;
      OSTRSTREAM_COPY(oss,line,100);
      value = line; //now use the string as the value
    }
  
  // now deal with the components list

  if(!self.Peek(Am_GRAPHICAL_PARTS, Am_NO_DEPENDENCY).Exists())
  { // doesn't have graphical parts    (usually when being destroyed)
    value = Am_No_Object;
    return value;
  }

  Am_Value_List components;
  components = self.Get( Am_GRAPHICAL_PARTS, Am_NO_DEPENDENCY);
  components.Start();
  Am_Object old_object;
  if( !components.Empty() )
  {
    if( components.Length() != 1 ) {
      Am_ERRORO("Components of " << self << " should be length 1",
		self, Am_GRAPHICAL_PARTS);
    }
    else //length == 1
    {
      old_object = components.Get();
    }
  }

  //if old_object is valid, then components list internal pointer is pointing
  //at it so can delete it if necessary .
  if( obj.Valid() )
  {
    Am_Object owner = obj.Get_Owner(Am_NO_DEPENDENCY);
    if( obj == old_object )
    {
      //make sure obj not in another group (or button) already
      if( owner.Valid() && (owner != self) )
      {
        // then obj is already in another object, use an instance of it
        obj = obj.Create();
        value = obj; // the return value from this formula is the new object
        owner = Am_No_Object;
      }
      //else old_object == obj and already part of self, so fine
    }
    else //old_object is different
    {
      if( old_object.Valid() ) //then remove old_object from components
      {
        components.Delete();
        //if old_object was part of me, then remove it
        if( old_object.Get_Owner() == self )
          self.Remove_Part( old_object );
      }
      // Make obj be a component
      components.Add( obj );
      if( !owner.Valid() ) // if owner is valid then obj is already part of me
        self.Add_Part(Am_ATTACHED_OBJECT, obj);
    }
  }
  else // no new obj, make sure no old components
  {
    if( old_object.Valid() )
    {
      components.Delete();
      self.Remove_Part( old_object );
    }
  }
  self.Set (Am_GRAPHICAL_PARTS, components);
  // value is already set with the return value
  return value;
}

/******************************************************************************
 * option_sub_menu_set_old_owner
 */

Am_Define_Formula( int, option_sub_menu_set_old_owner )
{
  Am_Object window, option_widget;
  window = self.Get_Owner();
  int ret = 0;
  if( window.Valid() )
  {
    option_widget = window.Get( Am_FOR_ITEM );
    if( option_widget.Valid() )
    {
      Am_Value_List parts;
      parts = self.Get( Am_GRAPHICAL_PARTS );
      ret = set_parts_list_commands_old_owner( parts, option_widget );
    }
  }
  return ret;
}

/******************************************************************************
 * Initializing
 *****************************************************************************/

// exported objects

Am_Object Am_Button = 0;
Am_Object Am_Button_Panel = 0;
Am_Object Am_Checkbox_Panel = 0;
Am_Object Am_Radio_Button_Panel = 0;
Am_Object Am_Menu = 0;
Am_Object Am_Menu_Line_Command = 0;

// internal objects

Am_Object Am_Checkbox;
Am_Object Am_Menu_Item;
Am_Object Am_Radio_Button;

Am_Object Am_Button_In_Panel;
Am_Object Am_Radio_Button_In_Panel = 0;
Am_Object Am_Checkbox_In_Panel = 0;
Am_Object Am_Item_In_Menu = 0;
Am_Object Am_Menu_Bar;
Am_Object Am_Option_Button;
Am_Object Am_Pop_Up_Menu_From_Widget_Proto; //internal
Am_Object Am_Pop_Up_Menu_Interactor;

void Am_Button_Widgets_Initialize() {
  am_grey_2 = Am_Style(0.5f, 0.5f, 0.5f, 2);
  am_white_2 = Am_Style(1.0f, 1.0f, 1.0f, 2);

  Am_Object inter; // interactor in the widget
  Am_Object command_obj;
  Am_Object_Advanced obj_adv; // to get at advanced features like
                              // local-only and demons.

  //////////// Command Objects ////////////////////////////

  Am_Menu_Line_Command = Am_Command.Create("Menu_Line_Command")
    .Set (Am_LABEL, "Menu_Line_Command")
    .Set (Am_ACTIVE, false) // menu lines aren't active menu members.
    .Set (Am_VALUE, NULL)
    ;

  //////////// button /////////////
  // instance of a group so can have a part (the contents)
  Am_Button = Am_Widget_Aggregate.Create("Button")
    .Add (Am_VALUE, NULL)
    .Add (Am_OLD_VALUE, NULL)
    .Add (Am_SELECTED, false) //set by interactor OR computed from the value
                              //of the command object
    .Set (Am_SELECTED, button_sel_from_value.Multi_Constraint())
    .Add (Am_ITEM_OFFSET, 0) // how far to indent the string or obj
    .Add (Am_INTERIM_SELECTED, false)
    .Add (Am_ACTIVE, Am_Active_From_Command)
    .Add (Am_ACTIVE_2, true) // used by interactive tools
    .Add (Am_KEY_SELECTED, false)
    .Add (Am_DEFAULT, Am_Default_From_Command)
    .Add (Am_LEAVE_ROOM_FOR_FRINGE, true)
    .Add (Am_FONT, Am_Default_Font)
    .Add (Am_FILL_STYLE, Am_Amulet_Purple)
    .Add (Am_STYLE_RECORD, Am_Get_Computed_Colors_Record_Form)
    .Add (Am_FINAL_FEEDBACK_WANTED, false)
    .Set (Am_DRAW_METHOD, button_draw)
    .Set (Am_MASK_METHOD, button_mask)
    .Add (Am_REAL_STRING_OR_OBJ, Am_Get_Real_String_Or_Obj)
    .Set (Am_WIDTH, button_width)
    .Set (Am_HEIGHT, button_height)
    .Set (Am_H_ALIGN, Am_CENTER_ALIGN)
    .Add (Am_LABEL_OR_ID, Am_Get_Label_Or_ID)
    .Add (Am_SET_COMMAND_OLD_OWNER, Am_Set_Old_Owner_To_Me)
    .Add (Am_ACCELERATOR_STRING, check_accel_string)
    .Add (Am_WIDGET_START_METHOD, Am_Standard_Widget_Start_Method)
    .Add (Am_WIDGET_ABORT_METHOD, button_abort_method)
    .Add (Am_WIDGET_STOP_METHOD, Am_Standard_Widget_Stop_Method)
    .Add_Part (Am_COMMAND, Am_Command.Create("Button_Command")
           .Set (Am_LABEL, "Button")
           .Set (Am_VALUE, 0))
    .Add_Part (Am_INTERACTOR,
           inter = Am_Choice_Interactor_Repeat_Same.Create("inter_in_button")
           .Set (Am_HOW_SET, Am_CHOICE_TOGGLE)
           .Set (Am_START_WHEN, Am_Default_Widget_Start_Char)
           .Set (Am_START_WHERE_TEST, Am_Inter_In)
           .Set (Am_ACTIVE, Am_Active_And_Active2)
  // set up a circular constraint between the value in the command of
  // the interactor and the value in the top-level button widget so setting
  // one will cause the other to change also
            .Set (Am_VALUE, get_button_widget_value.Multi_Constraint())
        )
    ;
  inter.Get_Object(Am_COMMAND)
    .Set(Am_IMPLEMENTATION_PARENT, Am_Get_Owners_Command)
    .Set(Am_DO_METHOD, button_inter_command_do)
    .Set(Am_UNDO_METHOD, Am_Widget_Inter_Command_Undo)
    .Set(Am_REDO_METHOD, Am_Widget_Inter_Command_Undo)
    .Set(Am_SELECTIVE_UNDO_METHOD, Am_Widget_Inter_Command_Selective_Undo)
    .Set(Am_SELECTIVE_REPEAT_SAME_METHOD,
     Am_Widget_Inter_Command_Selective_Repeat)
    .Set(Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL)
    ;

  obj_adv = (Am_Object_Advanced&)Am_Button;

  obj_adv.Get_Slot (Am_REAL_STRING_OR_OBJ)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_SELECTED)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_INTERIM_SELECTED)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_KEY_SELECTED)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_ACTIVE)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_WIDGET_LOOK)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_FINAL_FEEDBACK_WANTED)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_DEFAULT)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_FONT)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_FILL_STYLE)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_H_ALIGN)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_LEAVE_ROOM_FOR_FRINGE)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);

  // all the next slots should not be inherited
  obj_adv.Get_Slot(Am_SELECTED).Set_Inherit_Rule(Am_COPY);
  obj_adv.Get_Slot(Am_INTERIM_SELECTED).Set_Inherit_Rule(Am_COPY);
  obj_adv.Get_Slot(Am_KEY_SELECTED).Set_Inherit_Rule(Am_COPY);
  obj_adv.Get_Slot(Am_ACTIVE).Set_Inherit_Rule(Am_COPY);
  obj_adv.Get_Slot(Am_ACTIVE_2).Set_Inherit_Rule(Am_COPY);

  Am_Demon_Set demons = obj_adv.Get_Demons ().Copy ();
  demons.Set_Object_Demon (Am_DESTROY_OBJ, Am_Destroy_Button);
  obj_adv.Set_Demons (demons);

  //////////// Check box ////////////
  // Just a button with a different draw method.
  Am_Checkbox = Am_Button.Create("Checkbox")
    .Add (Am_BOX_ON_LEFT, true)
    .Add (Am_BOX_WIDTH, checkbox_box_width)
    .Add (Am_BOX_HEIGHT, checkbox_box_height)
    .Set (Am_FINAL_FEEDBACK_WANTED, true)
    .Set (Am_WIDTH, checkbox_width)
    .Set (Am_HEIGHT, checkbox_height)
    .Set (Am_H_ALIGN, Am_Align_From_Box_On_Left)
//    .Set (Am_EXTRA_BUTTON_BORDER, 0)
    .Set (Am_DRAW_METHOD, checkbox_draw)
    .Set (Am_MASK_METHOD, checkbox_mask)
    ;

  inter = Am_Checkbox.Get(Am_INTERACTOR);
  inter.Set(Am_FIRST_ONE_ONLY, true);

  obj_adv = (Am_Object_Advanced&)Am_Checkbox;

  obj_adv.Get_Slot (Am_BOX_ON_LEFT)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_BOX_WIDTH)
    .Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_BOX_HEIGHT)
    .Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);

  //////////// Radio Button ////////////
  // Just another button with a different draw method.
  Am_Radio_Button = Am_Button.Create("Radio_Button")
    .Add (Am_BOX_ON_LEFT, true)
    .Add (Am_BOX_WIDTH, radio_button_diameter)
    .Add (Am_BOX_HEIGHT, radio_button_diameter)
    .Set (Am_ITEM_OFFSET, 3)
    .Set (Am_WIDTH, checkbox_width)
    .Set (Am_HEIGHT, checkbox_height)
    .Set (Am_H_ALIGN, Am_Align_From_Box_On_Left)
//    .Set (Am_EXTRA_BUTTON_BORDER, 0)
    .Set (Am_DRAW_METHOD, radio_button_draw)
    .Set (Am_MASK_METHOD, radio_button_mask)
    ;

  inter = Am_Radio_Button.Get(Am_INTERACTOR);
  inter.Set(Am_FIRST_ONE_ONLY, true);

  obj_adv = (Am_Object_Advanced&)Am_Radio_Button;

  obj_adv.Get_Slot (Am_BOX_ON_LEFT)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_BOX_WIDTH)
    .Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_BOX_HEIGHT)
    .Set_Demon_Bits (Am_MOVING_REDRAW | Am_EAGER_DEMON);

  //////////// Menu Item ////////////
  // Just another button with a different draw method.
  Am_Menu_Item = Am_Button.Create("Menu_Item")
    .Set( Am_WIDTH, menu_item_width )
    .Set( Am_DRAW_METHOD, menu_item_draw )
    .Add( Am_MENU_ITEM_LEFT_OFFSET, menu_item_left_offset )
    .Add( Am_MENU_ITEM_ACCEL_OFFSET, menu_item_accel_offset )
    .Add( Am_MENU_ITEM_RIGHT_OFFSET, menu_item_right_offset )
    .Add( Am_MENU_ITEM_TOP_OFFSET, menu_item_top_offset )
    .Add( Am_MENU_ITEM_BOT_OFFSET, menu_item_bot_offset )
    .Add( Am_CHECKED_ITEM, am_checked_from_command )
    .Add( Am_MENU_SELECTED_COLOR, Am_Yellow )
    ;

  obj_adv = (Am_Object_Advanced&)inter;
  obj_adv.Get_Slot (Am_MENU_SELECTED_COLOR)
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);

  //////////// Button Panel /////////////
  // Design: Basically, use the regular button as an Item_Prototype and the
  // standard mapping stuff to copy it as needed.
  // Setting the ITEM slot of a Am_Button_In_Panel will cause the
  // value to be copied into the COMMAND slot either as a value or a part so
  // the regular button functions will work with it.  Other values are also
  // copied down.

  Am_Button_Panel = Am_Widget_Map.Create("Button Panel")
    .Add (Am_VALUE, NULL)
    .Add (Am_OLD_VALUE, 0)
    .Add (Am_ITEM_OFFSET, 0) // how far to indent the string or obj
    // active here is whether whole widget is active.  Use command part if any
    .Set (Am_FIXED_WIDTH, true)
    .Set (Am_FIXED_HEIGHT, false)
    .Add (Am_KEY_SELECTED, false)
    .Add (Am_MAX_WIDTH, max_item_width)
    .Add (Am_MAX_HEIGHT, max_item_height)
    .Set (Am_WIDTH, Am_Width_Of_Parts)
    .Set (Am_HEIGHT,Am_Height_Of_Parts)
    .Add (Am_ACTIVE, Am_Active_From_Command)
    .Add (Am_ACTIVE_2, true) // used by interactive tools
    .Add (Am_FONT, Am_Default_Font)
    .Add (Am_LEAVE_ROOM_FOR_FRINGE, false) // normally no fringe in button panels
    .Add (Am_FILL_STYLE, Am_Amulet_Purple)
    .Add (Am_FINAL_FEEDBACK_WANTED, false)
    .Add (Am_HOW_SET, Am_CHOICE_SET) //toggle is also a good choice
    .Set (Am_LAYOUT, Am_Vertical_Layout) // or horiz
    .Set (Am_H_ALIGN, Am_LEFT_ALIGN)
    .Set (Am_ITEMS, 0)
    .Add (Am_SET_COMMAND_OLD_OWNER, Am_Panel_Set_Old_Owner_To_Me)
    .Add (Am_WIDGET_START_METHOD, widget_first_member_start_method)
    .Add (Am_WIDGET_ABORT_METHOD, button_panel_abort_method)
    .Add (Am_WIDGET_STOP_METHOD, Am_Standard_Widget_Stop_Method)

    // Plus all the slots of a Map: Am_H_ALIGN, Am_V_ALIGN, Am_FIXED_WIDTH,
    //     Am_FIXED_HEIGHT
    .Add_Part (Am_INTERACTOR,
           inter = Am_Choice_Interactor_Repeat_Same.Create("inter_in_button_panel")
           .Set (Am_HOW_SET, Am_How_Set_From_Owner)
           .Set (Am_START_WHEN, Am_Default_Widget_Start_Char)
           .Set (Am_START_WHERE_TEST, Am_In_Active_Widget_Part)
           .Set (Am_ACTIVE, Am_Active_And_Active2)
           .Set (Am_VALUE, inter_value_from_panel_value.Multi_Constraint() )
           )
    .Set_Part (Am_ITEM_PROTOTYPE, Am_Button_In_Panel =
        Am_Button.Create ("Button_In_Panel_Proto")
           .Add (Am_REAL_WIDTH, button_width)
           .Add (Am_REAL_HEIGHT, button_height)
           .Set (Am_WIDTH, panel_item_width)
           .Set (Am_HEIGHT, panel_item_height)
           .Set (Am_ACTIVE, active_from_command_panel)
           .Set (Am_SELECTED, false)
           .Set (Am_LEAVE_ROOM_FOR_FRINGE, Am_From_Owner( Am_LEAVE_ROOM_FOR_FRINGE ) )
           .Set (Am_WIDGET_LOOK, look_from_owner)
           .Set (Am_FONT, Am_Font_From_Owner)
           .Set (Am_ITEM_OFFSET, Am_From_Owner (Am_ITEM_OFFSET))
           .Set (Am_FILL_STYLE, Am_From_Owner (Am_FILL_STYLE))
           .Set (Am_FINAL_FEEDBACK_WANTED, final_feedback_from_owner)
           .Set (Am_SET_COMMAND_OLD_OWNER, NULL)
           )
    .Add_Part (Am_COMMAND, Am_Command.Create("Command_In_Button_Panel")
           .Set (Am_LABEL, "Panel Button"))
    ;

  //this do method is in addition to the impl_command's of the interactor
  inter.Get_Object(Am_COMMAND)
    .Set(Am_DO_METHOD, Am_Inter_For_Panel_Do)
    .Set(Am_UNDO_METHOD, Am_Widget_Inter_Command_Undo)
    .Set(Am_REDO_METHOD, Am_Widget_Inter_Command_Undo)
    .Set(Am_SELECTIVE_UNDO_METHOD, Am_Widget_Inter_Command_Selective_Undo)
    .Set(Am_SELECTIVE_REPEAT_SAME_METHOD,
     Am_Widget_Inter_Command_Selective_Repeat)
    .Set(Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL)
    .Set_Name("Command_In_Button_Inter")
    ;

  obj_adv = (Am_Object_Advanced&)Am_Button_In_Panel;

  obj_adv = (Am_Object_Advanced&)Am_Button_Panel;

  // don't want the individual interactor from the button
  Am_Button_In_Panel.Remove_Part(Am_INTERACTOR);

  // when in a panel, the button's command object is gotten from the item slot
  // which is set automatically by the Map
  command_obj = Am_Button_In_Panel.Get_Object(Am_COMMAND);
  Am_Button_In_Panel.Add(Am_ITEM, command_obj); // default value
  Am_Button_In_Panel.Add(Am_ITEM_TO_COMMAND, Am_Copy_Item_To_Command);

  ///////////////////////////////////////////////////////////////////////////
  // Radio button panel
  ///////////////////////////////////////////////////////////////////////////

  Am_Radio_Button_Panel = Am_Button_Panel.Create("Radio button Panel");
  Am_Radio_Button_Panel
    .Add (Am_BOX_ON_LEFT, true)
    .Add (Am_BOX_HEIGHT, radio_button_diameter)
    .Add (Am_BOX_WIDTH, radio_button_diameter)
    .Set (Am_ITEM_OFFSET, 3)
    .Set (Am_H_ALIGN, Am_Align_From_Box_On_Left)
    .Set (Am_V_SPACING, 0)
    .Set (Am_FIXED_WIDTH, false) // fixed width makes the labels centered
    .Set (Am_FINAL_FEEDBACK_WANTED, true)
    .Set_Part (Am_ITEM_PROTOTYPE, Am_Radio_Button_In_Panel =
       Am_Radio_Button.Create ("Radio_Button_In_Panel_Proto")
           .Add (Am_REAL_WIDTH, checkbox_width)
           .Add (Am_REAL_HEIGHT, checkbox_height)
           .Set (Am_WIDTH, panel_item_width)
           .Set (Am_HEIGHT, panel_item_height)
           .Set (Am_BOX_ON_LEFT, box_on_left_from_owner)
           .Set (Am_BOX_WIDTH, box_width_from_owner)
           .Set (Am_BOX_HEIGHT, box_height_from_owner)
           .Set (Am_ACTIVE, active_from_command_panel)
           .Set (Am_SELECTED, false)
           .Set (Am_WIDGET_LOOK, look_from_owner)
           .Set (Am_FONT, Am_Font_From_Owner)
           .Set (Am_FILL_STYLE, Am_From_Owner (Am_FILL_STYLE))
           .Set (Am_ITEM_OFFSET, Am_From_Owner (Am_ITEM_OFFSET))
           .Set (Am_SET_COMMAND_OLD_OWNER, NULL)
           .Set (Am_FINAL_FEEDBACK_WANTED, final_feedback_from_owner)
           )
    ;

  inter = Am_Radio_Button_Panel.Get( Am_INTERACTOR);
  inter.Set(Am_FIRST_ONE_ONLY, true);

  // don't want the individual interactor from the button
  Am_Radio_Button_In_Panel.Remove_Part(Am_INTERACTOR);

  // when in a panel, the box's command object is gotten from the item slot
  // which is set automatically by the Map
  command_obj = Am_Radio_Button_In_Panel.Get_Object(Am_COMMAND);
  Am_Radio_Button_In_Panel.Add(Am_ITEM, command_obj); // default value
  Am_Radio_Button_In_Panel.Add(Am_ITEM_TO_COMMAND, Am_Copy_Item_To_Command);

  ///////////////////////////////////////////////////////////////////////////
  // Check Boxes
  ///////////////////////////////////////////////////////////////////////////

  Am_Checkbox_Panel = Am_Button_Panel.Create("Checkbox Panel");
  Am_Checkbox_Panel
    .Set (Am_HOW_SET, Am_CHOICE_LIST_TOGGLE)
    .Set (Am_FINAL_FEEDBACK_WANTED, true)
    .Set (Am_FIXED_WIDTH, false) // fixed width makes the labels centered
    .Add (Am_BOX_ON_LEFT, true)
    .Add (Am_BOX_HEIGHT, checkbox_box_height)
    .Add (Am_BOX_WIDTH, checkbox_box_width)
    .Set (Am_ITEM_OFFSET, 3)
    .Set( Am_V_SPACING, 0 )
    .Set (Am_H_ALIGN, Am_Align_From_Box_On_Left)
    .Set_Part (Am_ITEM_PROTOTYPE, Am_Checkbox_In_Panel =
           Am_Checkbox.Create ("Checkbox_In_Panel_Proto")
           .Add (Am_REAL_WIDTH, checkbox_width)
           .Add (Am_REAL_HEIGHT, checkbox_height)
           .Set (Am_WIDTH, panel_item_width)
           .Set (Am_HEIGHT, panel_item_height)
           .Set (Am_BOX_ON_LEFT, box_on_left_from_owner)
           .Set (Am_BOX_WIDTH, box_width_from_owner)
           .Set (Am_BOX_HEIGHT, box_height_from_owner)
           .Set (Am_ACTIVE, active_from_command_panel)
           .Set (Am_SELECTED, false)
           .Set (Am_WIDGET_LOOK, look_from_owner)
           .Set (Am_FONT, Am_Font_From_Owner)
           .Set (Am_FILL_STYLE, Am_From_Owner (Am_FILL_STYLE))
           .Set (Am_ITEM_OFFSET, Am_From_Owner (Am_ITEM_OFFSET))
           .Set (Am_FINAL_FEEDBACK_WANTED, final_feedback_from_owner)
           .Set (Am_SET_COMMAND_OLD_OWNER, NULL)
           )
    ;

  inter = Am_Checkbox_Panel.Get( Am_INTERACTOR);
  inter.Set(Am_FIRST_ONE_ONLY, true);

  // don't want the individual interactor from the button
  Am_Checkbox_In_Panel.Remove_Part(Am_INTERACTOR);
  // when in a panel, the box's command object is gotten from the item slot
  // which is set automatically by the Map
  command_obj = Am_Checkbox_In_Panel.Get_Object(Am_COMMAND);
  Am_Checkbox_In_Panel.Add(Am_ITEM, command_obj); // default value
  Am_Checkbox_In_Panel.Add(Am_ITEM_TO_COMMAND, Am_Copy_Item_To_Command);

  ///////////////////////////////////////////////////////////////////////////
  // Menus
  ///////////////////////////////////////////////////////////////////////////

  // Based on button panel.

  Am_Menu = Am_Button_Panel.Create("Menu")
    .Set( Am_WIDTH, menu_width )
    .Set( Am_HEIGHT, menu_height )
    .Set( Am_HOW_SET, Am_CHOICE_SET )
    .Add( Am_MENU_BORDER, menu_border_size )
    .Set( Am_LEFT_OFFSET, Am_Same_As( Am_MENU_BORDER ) )
    .Set( Am_TOP_OFFSET, Am_Same_As( Am_MENU_BORDER ) )
    .Set( Am_V_SPACING, 0 )
    .Add( Am_MENU_LINE_HEIGHT, menu_line_height )
    .Set( Am_KEY_SELECTED, false )
    .Set( Am_FINAL_FEEDBACK_WANTED, false )
    .Add( Am_STYLE_RECORD, Am_Get_Computed_Colors_Record_Form )
    .Set( Am_DRAW_METHOD, menu_draw )
    .Set( Am_MASK_METHOD, menu_mask )
    ;

  Am_Menu.Set_Part( Am_ITEM_PROTOTYPE, Am_Item_In_Menu =
      Am_Menu_Item.Create ("Item_In_Menu_Proto")
        .Add( Am_REAL_WIDTH, menu_item_width )
        .Add( Am_REAL_HEIGHT, menu_item_height )
        .Set( Am_WIDTH, panel_item_width )
        .Set( Am_HEIGHT, panel_item_height )
        .Set( Am_ACTIVE, active_from_command_panel )
        .Set( Am_SELECTED, false )
        .Set( Am_WIDGET_LOOK, look_from_owner )
        .Set( Am_FONT, Am_Font_From_Owner )
        .Set( Am_ITEM_OFFSET, Am_From_Owner( Am_ITEM_OFFSET ) )
        .Set( Am_FILL_STYLE, Am_From_Owner( Am_FILL_STYLE ) )
        .Set( Am_FINAL_FEEDBACK_WANTED, final_feedback_from_owner )
        .Set( Am_SET_COMMAND_OLD_OWNER, NULL )
        )
        ;

  // don't want the individual interactor from the button
  Am_Item_In_Menu.Remove_Part(Am_INTERACTOR);
  // when in a panel, the box's command object is gotten from the item slot
  // which is set automatically by the Map
  command_obj = Am_Item_In_Menu.Get_Object(Am_COMMAND);
  Am_Item_In_Menu.Add(Am_ITEM, command_obj); // default value
  Am_Item_In_Menu.Add(Am_ITEM_TO_COMMAND, Am_Copy_Item_To_Command);

  ///////////////////////////////////////////////////////////////////////////
  // Menu_Bars
  ///////////////////////////////////////////////////////////////////////////

  // Based on menus.

  // internal: prototype for the sub-menu windows
  Am_Object proto, sub_menu;

  Am_Pop_Up_Menu_From_Widget_Proto =
    Am_Window.Create("Pop_Up_Menu_From_Widget")
      .Set(Am_OMIT_TITLE_BAR, true)
      .Set(Am_SAVE_UNDER, true)
      .Add(Am_FOR_ITEM, Am_No_Object) // for_item is menu button widget
      .Set(Am_LEFT, 0) // usually overridden with a formula
      .Set(Am_TOP, 0)  // usually overridden with a formula
      .Set(Am_WIDTH, popup_sub_win_width.Multi_Constraint())
      .Set(Am_HEIGHT, popup_sub_win_height.Multi_Constraint())
      .Set(Am_VISIBLE, false)
      .Set(Am_FILL_STYLE, popup_fill_from_for_item)
      .Set(Am_UNDO_HANDLER, popup_sub_win_undo_handler)
      // should specify an undo handler
      .Add_Part(Am_SUB_MENU, sub_menu = Am_Menu.Create("Sub_Menu")
		.Set(Am_ITEMS, sub_menu_items)
		.Set(Am_FILL_STYLE, Am_From_Owner (Am_FILL_STYLE))
		.Set(Am_WIDGET_LOOK, sub_menu_look_from_win_for_item)
		.Set(Am_FONT, sub_menu_font_from_win_for_item)
		.Set(Am_SET_COMMAND_OLD_OWNER, sub_menu_set_old_owner)
		);

  //remove the interactor from the sub-menu
  sub_menu.Remove_Part( Am_INTERACTOR );

  proto = sub_menu.Get_Object( Am_ITEM_PROTOTYPE );
  proto.Set_Name("popup_sub_item")
      .Set( Am_ACTIVE, menu_bar_sub_item_active );

  Am_Menu_Bar_Sub_Window_Proto =
    Am_Pop_Up_Menu_From_Widget_Proto.Create("Sub_Menu_Window")
	  //under windows, these slots are set and the constraints
	  //go away, so make sure they are Multi_Constraint
      .Set( Am_LEFT, menu_bar_sub_win_left.Multi_Constraint() )
      .Set( Am_TOP, menu_bar_sub_win_top.Multi_Constraint() )
      ;

  Am_Menu_Bar = Am_Menu.Create("Menu_Bar")
      //default width = width of container (usually a window)
      .Set( Am_WIDTH, menu_bar_width )
      .Set( Am_HEIGHT, menu_bar_height )
      .Set( Am_HOW_SET, Am_CHOICE_SET )
      .Set( Am_FIXED_WIDTH, false )
      .Set( Am_FIXED_HEIGHT, true )
      .Set( Am_LEFT_OFFSET, menu_bar_x_offset )
      .Set( Am_TOP_OFFSET, menu_bar_y_offset )
      .Set( Am_LAYOUT, Am_Horizontal_Layout )
      .Set( Am_H_SPACING, menu_bar_h_spacing )
      .Set( Am_ITEMS, 0 )
      .Add( Am_SELECTION_WIDGET, Am_No_Object ) //can be set here or in cmds
      ;

  obj_adv = (Am_Object_Advanced&)Am_Menu_Bar;
  obj_adv.Get_Slot(Am_FILL_STYLE)
      .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON );
  demons = obj_adv.Get_Demons().Copy();
  demons.Set_Object_Demon( Am_DESTROY_OBJ, Am_Destroy_Menu_Bar );
  obj_adv.Set_Demons( demons );

  proto = Am_Menu_Bar.Get_Object( Am_ITEM_PROTOTYPE );
  proto.Remove_Slot( Am_ITEM_TO_COMMAND ); // make sure the inherited
                                           // constraint is destroyed
  proto.Set( Am_ITEM_TO_COMMAND, menu_bar_copy_item_to_command );
  proto.Add( Am_SUB_MENU, Am_No_Object );  // fix a bug that if create slot
                                           // later, doesn't re-evaluate formulas

  //make Am_SUB_MENU slot be local
  obj_adv = ( Am_Object_Advanced& )proto;
  obj_adv.Get_Slot( Am_SUB_MENU ).Set_Inherit_Rule( Am_LOCAL );

  inter = Am_Menu_Bar.Get_Object( Am_INTERACTOR )
      .Set( Am_START_WHERE_TEST, in_menu_bar_item )
      .Set( Am_MULTI_OWNERS, menu_bar_window_list )
      .Set( Am_INTERIM_DO_METHOD, menu_bar_inter_interim_do )
      .Set( Am_ABORT_DO_METHOD, menu_bar_inter_abort )
      .Set( Am_DO_METHOD, menu_bar_inter_do )
      .Set_Name("inter_in_menu_bar")
      ;

  inter.Get_Object(Am_COMMAND)
      .Set( Am_DO_METHOD, NULL ) //get rid of Am_Inter_For_Panel_Do
      .Set( Am_UNDO_METHOD, NULL )
      .Set( Am_REDO_METHOD, NULL )
      .Set( Am_SELECTIVE_UNDO_METHOD, NULL )
      .Set( Am_SELECTIVE_REPEAT_SAME_METHOD, NULL )
      .Set( Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL )
      ;

  ///////////////////////////////////////////////////////////////////////////
  // Option button: single button that pops up a menu
  ///////////////////////////////////////////////////////////////////////////

  // can't be a part of option button since needs to be stand-alone
  // window at the top level

  Am_Option_Button_Sub_Window_Proto =
      Am_Pop_Up_Menu_From_Widget_Proto.Create("Option_Sub_Window")
      .Set( Am_FOR_ITEM, NULL )
      .Set( Am_LEFT, option_sub_win_left )
      .Set( Am_TOP, option_sub_win_top )
      .Set( Am_VISIBLE, false )
     ;
  Am_Object sub_menu_in_option_button =
    Am_Option_Button_Sub_Window_Proto.Get_Object(Am_SUB_MENU);
  sub_menu_in_option_button.Set( Am_ITEMS, option_button_items )
    .Set_Name("sub_menu_in_option_button")
    .Set( Am_SET_COMMAND_OLD_OWNER, option_sub_menu_set_old_owner )
    .Add_Part( Am_INTERACTOR, inter = Am_Button_Panel.Get_Object( Am_INTERACTOR )
      .Create("Inter_In_Option_PopUp")
      )
      ;

  inter.Set( Am_MULTI_OWNERS, option_button_window_list );
  inter.Get_Object( Am_COMMAND )
    .Set( Am_DO_METHOD, option_sub_menu_do )
    .Set( Am_ABORT_DO_METHOD, option_hide_sub_menu )
    ;
  sub_menu_in_option_button.Remove_Part( Am_COMMAND );
  sub_menu_in_option_button.Add( Am_COMMAND, command_from_for_item )
    .Set_Inherit_Rule( Am_COMMAND, Am_INHERIT )
    ;

  // like menubars
  Am_Option_Button = Am_Button.Create("Option_Button")
    .Add( Am_ITEMS, 0 )
    .Set( Am_WIDTH, option_button_width )
    .Set( Am_HEIGHT, button_height )
    .Set( Am_DRAW_METHOD, option_button_draw )
    .Add( Am_FOR_ITEM, create_sub_menu )
    .Add( Am_SUB_MENU, 0 )
    .Add( Am_MENU_ITEM_LEFT_OFFSET, menu_item_left_offset ) // need this for drawing the text
    .Set_Inherit_Rule( Am_SUB_MENU, Am_LOCAL )
    .Add( Am_COMPUTE_INTER_VALUE, compute_inter_value )
    .Set( Am_REAL_STRING_OR_OBJ, get_real_string_from_inter_val )
    .Set( Am_WIDGET_ABORT_METHOD, option_button_abort_method)
    ;

  obj_adv = (Am_Object_Advanced&)Am_Option_Button;
  demons = obj_adv.Get_Demons().Copy();
  demons.Set_Object_Demon( Am_DESTROY_OBJ, destroy_option_button );
  obj_adv.Set_Demons( demons );

  inter = Am_Option_Button.Get_Object( Am_INTERACTOR )
    .Set( Am_CONTINUOUS, false )
    .Set( Am_DO_METHOD, option_button_start_do )
    .Set_Name("inter_in_option_button")
    ;
  //no command in interactor, instead use command in interactor of the menu
  inter.Set (Am_COMMAND, 0);
  inter.Get_Object( Am_IMPLEMENTATION_COMMAND )
    .Set( Am_IMPLEMENTATION_PARENT, Am_NOT_USUALLY_UNDONE );

  
  //do this last so it doesn't trigger the warning about illegal values
  Am_Option_Button.Set( Am_VALUE, option_button_value.Multi_Constraint());
  ///////////////////////////////////////////////////////////////////////////
  // Am_Pop_Up_Menu_Interactor: Pops up a menu
  ///////////////////////////////////////////////////////////////////////////

  Am_Pop_Up_Menu_Sub_Window_Proto =
      Am_Option_Button_Sub_Window_Proto.Create("Pop_Up_Menu_Sub_Window")
      .Set( Am_LEFT, 0 ) //set by interactor
      .Set( Am_TOP, 0 )
     ;

  Am_Pop_Up_Menu_Interactor = 
    Am_One_Shot_Interactor.Create("Am_Pop_Up_Menu_Interactor")
    .Add( Am_ITEMS, 0 )
    .Add( Am_FOR_ITEM, create_popup_sub_menu )
    .Add( Am_SUB_MENU, 0 )
    .Add( Am_FILL_STYLE, Am_Amulet_Purple)
    .Add( Am_WIDGET_LOOK, Am_Default_Widget_Look)
    .Set_Inherit_Rule( Am_SUB_MENU, Am_LOCAL )
    .Set( Am_VALUE, option_button_value.Multi_Constraint() )
    .Set( Am_DO_METHOD, popup_inter_start_do )
    .Add( Am_REAL_STRING_OR_OBJ, get_real_string_from_inter_val )
    ;

  obj_adv = (Am_Object_Advanced&)Am_Pop_Up_Menu_Interactor;
  demons = obj_adv.Get_Demons().Copy();
  demons.Set_Object_Demon( Am_DESTROY_OBJ, destroy_popup_interactor );
  obj_adv.Set_Demons( demons );
}
