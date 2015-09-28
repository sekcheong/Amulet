/* ************************************************************************
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the scroll bar widgets

   Designed and implemented by Brad Myers
*/

#include <am_inc.h>

#include AM_IO__H

#include WIDGETS_ADVANCED__H
#include STANDARD_SLOTS__H
#include VALUE_LIST__H
#include OPAL_ADVANCED__H  // for Am_DRAWONABLE, Am_Window_Coordinate
#include INTER_ADVANCED__H // needed for Am_Four_Ints
#include ANIM__H // for scrollbar animation

#include WIDGETS__H
#include GEM__H
#include INTER__H
#include OPAL__H
#include REGISTRY__H

// we need this image array to draw the background pattern of mac scroll bars

#if !defined(_WINDOWS)
static char mac_scroll_bg_stipple[] =
                           { 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22 };
#else
static const int mac_scroll_bg_stipple[] =
                           { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
#endif

Am_Image_Array Mac_Scroll_BG_Image_Array((char*)&mac_scroll_bg_stipple, 8, 8);

/******************************************************************************
 * Arrows
 ******************************************************************************/

/******************************************************************************
 * draw_win_arrow_box
 */

void
draw_win_arrow_box(
  am_rect arrow_rect,
  bool depressed,
  bool option,
  const Computed_Colors_Record& color_rec,
  Am_Drawonable* draw )
{
  // draw the outside rectangle
  Am_Style upper_left  = !depressed
                         ? ( option
                             ? color_rec.data->highlight_style
                             : color_rec.data->foreground_style )
                         : color_rec.data->shadow_style;
  Am_Style lower_right = !depressed ? Am_Black
                                    : color_rec.data->shadow_style;
  Am_Draw_Rect_Border( arrow_rect, upper_left, lower_right, draw );
  Inset_Rect( arrow_rect, 1 );

  // draw the inside rectangle
  upper_left  = !depressed
                ? ( option
                    ? color_rec.data->foreground_style
                    : color_rec.data->highlight_style)
                : color_rec.data->foreground_style;
  lower_right = !depressed ? color_rec.data->background_style
                           : color_rec.data->foreground_style;
  Am_Draw_Rect_Border( arrow_rect, upper_left, lower_right, draw );
  Inset_Rect( arrow_rect, 1 );

  // draw the fill
  draw->Draw_Rectangle( Am_No_Style, color_rec.data->foreground_style,
                        arrow_rect.left, arrow_rect.top,
                        arrow_rect.width, arrow_rect.height );
}

/******************************************************************************
 * draw_mac_arrow_box
 */

void
draw_mac_arrow_box(
  am_rect arrow_rect,
  bool active,
  const Computed_Colors_Record& color_rec,
  Am_Drawonable* draw )
{
  draw->Draw_Rectangle( Am_Black, color_rec.data->foreground_style,
                        arrow_rect.left, arrow_rect.top,
                        arrow_rect.width, arrow_rect.height );
  if( active )
  {
    Inset_Rect( arrow_rect, 1 );
    Am_Draw_Rect_Border( arrow_rect, color_rec.data->highlight_style,
                         color_rec.data->background_style, draw );
  }
}

/******************************************************************************
 * draw_up_arrow
 */

void
draw_up_arrow(
  int left, int top, int width, int height,
  Am_Widget_Look look,
  bool depressed,
  bool active,
  const Computed_Colors_Record& color_rec,
  Am_Drawonable* draw )
{
  am_rect arrow_rect( left, top, width, height );
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
    {
      const Am_Style fill1 (depressed ? color_rec.data->background_style :
                            color_rec.data->foreground_style);
      const Am_Style line1 (depressed ? color_rec.data->highlight_style :
                            color_rec.data->shadow_style);
      const Am_Style line2 (depressed ? color_rec.data->shadow_style :
                            color_rec.data->highlight_style);
      int center_x = left + width / 2;
      int rightm1 = left + width - 1 - 1;
      int bottom = top + height - 1;
      
      //right line
      draw->Draw_2_Lines(line1, Am_No_Style,
             center_x, top,
             rightm1, bottom - 1,
             left, bottom - 1);
      //left line
      draw->Draw_Line(line2, center_x, top, left, bottom - 1);

      //right line
      draw->Draw_2_Lines(line1, fill1,
             center_x, top + 1,
             rightm1 - 1, bottom - 2,
             left + 1, bottom - 2);
      //left line
      draw->Draw_Line(line2, center_x, top + 1, left + 1, bottom - 2);
      break;
    }
    case Am_WINDOWS_LOOK_val:
      draw_win_arrow_box( arrow_rect, depressed, false, color_rec, draw );

    {
      int a_top    = !depressed ? 6 : 7,
          a_left   = !depressed ? 7 : 8,
          a_width  = 0, // width is six, but it starts our at zero
                        // (actually 1, because of pixel drawing)
          a_height = 4;
      
      for( int y = 0; y < a_height; y++ )
      {
        draw->Draw_Line( Am_Black,
                         left + a_left, top + a_top + y,
                         left + a_left + a_width, top + a_top + y );
        a_left--;
        a_width += 2;
      }
      break;
    }

    case Am_MACINTOSH_LOOK_val:
    {
      Am_Style fill_style = (active)
                            ? ( !depressed 
                                ? color_rec.data->background_style
                                : Am_Black )
                            : color_rec.data->foreground_style;

      draw_mac_arrow_box( arrow_rect, active, color_rec, draw );
      draw->Draw_3_Lines( color_rec.data->shadow_style, fill_style,
                          left + 2,  top + 8,
                          left + 12, top + 8,
                          left + 7,  top + 3,
                          left + 2,  top + 8 );
      draw->Draw_3_Lines( color_rec.data->shadow_style, fill_style,
                          left + 5,  top + 8,
                          left + 5,  top + 12,
                          left + 9,  top + 12,
                          left + 9,  top + 8 );
      draw->Draw_Line(    fill_style,
                          left + 6,  top + 8,
                          left + 8,  top + 8 );
      break;
    }
    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
}

/******************************************************************************
 * draw_down_arrow
 */

void draw_down_arrow(
  int left, int top, int width, int height,
  Am_Widget_Look look,
  bool depressed,
  bool active,
  bool option,
  const Computed_Colors_Record& color_rec,
  Am_Drawonable* draw )
{
  am_rect arrow_rect( left, top, width, height );
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
    {
      const Am_Style fill1 (depressed ? color_rec.data->background_style :
                            color_rec.data->foreground_style);
      const Am_Style line1 (depressed ? color_rec.data->shadow_style :
                            color_rec.data->highlight_style);
      const Am_Style line2 (depressed ? color_rec.data->highlight_style :
                            color_rec.data->shadow_style);

      int center_x = left + width/2;
      int right = left + width - 1;
      int bottom = top + height - 1;
      
      //left line
      draw->Draw_2_Lines(line1, Am_No_Style,
             center_x, bottom,
             left + 1, top,
             right, top);
      //right line
      draw->Draw_Line(line2, right, top, center_x, bottom);
      //left line
      draw->Draw_2_Lines(line1, fill1,
             center_x, bottom - 1,
             left + 2, top + 1,
             right - 1, top + 1);
      //right line
      draw->Draw_Line (line2, right - 1, top + 1, center_x, bottom - 1);
      break;
    }

    case Am_WINDOWS_LOOK_val:
    {
      draw_win_arrow_box( arrow_rect, depressed, option, color_rec, draw );

      int a_top    = !depressed ? 6 : 7,
          a_left   = !depressed ? 4 : 5,
          a_width  = 6, // 7 - 1, because extra pixel is drawn at end of line
          a_height = 4;
      
      if( height > 16 ) // so that the option button arrow draw correctly
        a_top += (height - 16) / 2;

      for( int y = 0; y < a_height; y++ )
      {
        draw->Draw_Line( Am_Black,
                         left + a_left, top + a_top + y,
                         left + a_left + a_width, top + a_top + y );
        a_left++;
        a_width -= 2;
      }
      break;
    }

    case Am_MACINTOSH_LOOK_val:
    {
      Am_Style fill_style = (active)
                            ? ( !depressed
                                ? color_rec.data->background_style
                                : Am_Black )
                            : color_rec.data->foreground_style;

      draw_mac_arrow_box( arrow_rect, active, color_rec, draw );
      draw->Draw_3_Lines( color_rec.data->shadow_style, fill_style,
                          left + 2,  top + 7,
                          left + 12, top + 7,
                          left + 7,  top + 12,
                          left + 2,  top + 7 );
      draw->Draw_3_Lines( color_rec.data->shadow_style, fill_style,
                          left + 5,  top + 7,
                          left + 5,  top + 3,
                          left + 9,  top + 3,
                          left + 9,  top + 7 );
      draw->Draw_Line(    fill_style,
                          left + 6,  top + 7,
                          left + 8,  top + 7 );
      break;
    }

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
}

/******************************************************************************
 * draw_left_arrow
 */

void draw_left_arrow(
  int left, int top, int width, int height,
  Am_Widget_Look look,
  bool depressed,
  bool active,
  const Computed_Colors_Record& color_rec,
  Am_Drawonable* draw )
{
  am_rect arrow_rect( left, top, width, height );
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
    {
      const Am_Style fill1 (depressed ? color_rec.data->background_style :
                            color_rec.data->foreground_style);
      const Am_Style line1 (depressed ? color_rec.data->highlight_style :
                            color_rec.data->shadow_style);
      const Am_Style line2 (depressed ? color_rec.data->shadow_style :
                            color_rec.data->highlight_style);

      int center_y = top + (height - 2)/2;
      int rightm1 = left + width - 2;
      int bottomm1 = top + height - 2;
      
      //bottom line
      draw->Draw_2_Lines(line1, Am_No_Style,
             left, center_y + 1,
             rightm1, bottomm1 + 1,
             rightm1, top + 1);
      //top line
      draw->Draw_Line(line2, left, center_y + 1, rightm1, top + 1);
      //bottom line
      draw->Draw_2_Lines(line1, fill1,
             left + 1, center_y + 1,
             rightm1 - 1, bottomm1,
             rightm1 - 1, top + 2);
      //top line
      draw->Draw_Line(line2, left + 1, center_y + 1, rightm1 - 1, top + 2);
      break;
    }

    case Am_WINDOWS_LOOK_val:
    {
      draw_win_arrow_box( arrow_rect, depressed, false, color_rec, draw );
      
      int a_top    = !depressed ? 7 : 8,
          a_left   = !depressed ? 5 : 6,
          a_width  = 4,
          a_height = 0; // actually 1, because of pixel drawing
      
      for( int x = 0; x < a_width; x++ )
      {
        draw->Draw_Line( Am_Black,
                         left + a_left + x, top + a_top,
                         left + a_left + x, top + a_top + a_height );
        a_top--;
        a_height += 2;
      }
      break;
    }

    case Am_MACINTOSH_LOOK_val:
    {
      draw_mac_arrow_box( arrow_rect, active, color_rec, draw );

      Am_Style fill_style = (active)
                            ? ( !depressed
                                ? color_rec.data->background_style
                                : Am_Black )
                            : color_rec.data->foreground_style;
      draw->Draw_3_Lines( color_rec.data->shadow_style, fill_style,
                          left + 8,  top + 2,
                          left + 8,  top + 12,
                          left + 3,  top + 7,
                          left + 8,  top + 2 );
      draw->Draw_3_Lines( color_rec.data->shadow_style, fill_style,
                          left + 8,  top + 5,
                          left + 12, top + 5,
                          left + 12, top + 9,
                          left + 8,  top + 9 );
      draw->Draw_Line(    fill_style,
                          left + 8,  top + 6,
                          left + 8,  top + 8 );
      break;
    }

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
}

/******************************************************************************
 * draw_right_arrow
 */

void draw_right_arrow(
  int left, int top, int width, int height,
  Am_Widget_Look look,
  bool depressed,
  bool active,
  const Computed_Colors_Record& color_rec,
  Am_Drawonable* draw )
{
  am_rect arrow_rect( left, top, width, height );
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
    {
      const Am_Style fill1 (depressed ? color_rec.data->background_style :
                            color_rec.data->foreground_style);
      const Am_Style line1 (depressed ? color_rec.data->shadow_style :
                            color_rec.data->highlight_style);
      const Am_Style line2 (depressed ? color_rec.data->highlight_style :
                            color_rec.data->shadow_style);

      int center_y = top + height/2;
      int right = left + width - 1;
      int bottom = top + height - 1;
      
      //top line
      draw->Draw_2_Lines(line1, Am_No_Style,
             left, bottom,
             left, top + 1,
             right, center_y);
      //bottom line
      draw->Draw_Line(line2, left, bottom, right, center_y);
      //top line
      draw->Draw_2_Lines(line1, fill1,
             left + 1, bottom - 1,
             left + 1, top + 2,
             right - 1, center_y);
      //bottom line
      draw->Draw_Line(line2, left + 1, bottom - 1, right - 1, center_y);
      break;
    }

    case Am_WINDOWS_LOOK_val:
    {
      draw_win_arrow_box( arrow_rect, depressed, false, color_rec, draw );
      
      int a_top    = !depressed ? 4 : 5,
          a_left   = !depressed ? 6 : 7,
          a_width  = 4,
          a_height = 6; // actually 7, because of pixel drawing

      for( int x = 0; x < a_width; x++ )
      {
        draw->Draw_Line( Am_Black,
                         left + a_left + x, top + a_top,
                         left + a_left + x, top + a_top + a_height );
        a_top++;
        a_height -= 2;
      }
      break;
    }

    case Am_MACINTOSH_LOOK_val:
    {
      draw_mac_arrow_box( arrow_rect, active, color_rec, draw );

      Am_Style fill_style = (active)
                            ? ( !depressed
                                ? color_rec.data->background_style
                                : Am_Black )
                            : color_rec.data->foreground_style;
      draw->Draw_3_Lines( color_rec.data->shadow_style, fill_style,
                          left + 7,  top + 2,
                          left + 7,  top + 12,
                          left + 12, top + 7,
                          left + 7,  top + 2 );
      draw->Draw_3_Lines( color_rec.data->shadow_style, fill_style,
                          left + 7,  top + 5,
                          left + 3,  top + 5,
                          left + 3,  top + 9,
                          left + 7,  top + 9 );
      draw->Draw_Line(    fill_style,
                          left + 7,  top + 6,
                          left + 7,  top + 8 );
      break;
    }

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
}

/******************************************************************************
 * scroll_arrow_draw
 */

Am_Define_Method( Am_Draw_Method, void, scroll_arrow_draw,
                ( Am_Object self, Am_Drawonable* drawonable,
                  int x_offset, int y_offset) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  bool active = self.Get( Am_ACTIVE );
  bool interim_selected = self.Get( Am_INTERIM_SELECTED );
  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  Am_Scroll_Arrow_Direction dir =
      (Am_Scroll_Arrow_Direction)(int)self.Get( Am_SCROLL_ARROW_DIRECTION );

  if( ( look != Am_MOTIF_LOOK     ) &&
      ( look != Am_WINDOWS_LOOK   ) &&
      ( look != Am_MACINTOSH_LOOK ) )
    Am_Error("Illegal look detected in scroll_arrow_draw");

  // finally ready to draw it
  switch( dir )
  {
    case Am_SCROLL_ARROW_UP:
      draw_up_arrow( left, top, width, height,
                     look, interim_selected, active, color_rec, drawonable );
      break;

    case Am_SCROLL_ARROW_DOWN:
      draw_down_arrow( left, top, width, height,
                       look, interim_selected, active, false, color_rec,
                       drawonable );
      break;

    case Am_SCROLL_ARROW_LEFT:
      draw_left_arrow( left, top, width, height,
                       look, interim_selected, active, color_rec, drawonable );
      break;

    case Am_SCROLL_ARROW_RIGHT:
      draw_right_arrow( left, top, width, height,
                        look, interim_selected, active, color_rec, drawonable );
      break;

    default:
      Am_Error("Bad Am_Scroll_Arrow_Direction in arrow object");
  }// end switch
}

/******************************************************************************
 * v_scroll_indicator_draw
 */

Am_Define_Method( Am_Draw_Method, void, v_scroll_indicator_draw,
                ( Am_Object self, Am_Drawonable* draw,
                  int x_offset, int y_offset) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  bool active = self.Get( Am_ACTIVE );
  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      Am_Draw_Motif_Box( left, top, width, height, false, color_rec, draw );
      break;

    case Am_WINDOWS_LOOK_val:
      {
        am_rect r( left, top, width, height );
        draw_win_arrow_box( r, false, false, color_rec, draw );
      }
      break;
    
    case Am_MACINTOSH_LOOK_val:
      if( active )
      {
        draw->Draw_Rectangle( color_rec.data->highlight_style,
                              color_rec.data->background_style,
                              left + 1, top + 1, 14, 15 );
        draw->Draw_3_Lines( color_rec.data->shadow_style, Am_No_Style,
                            left + 1,  top,
                            left + 14, top,
                            left + 14, top + 15,
                            left + 1,  top + 15 );
        draw->Draw_Rectangle( Am_No_Style, color_rec.data->highlight_style,
                              left + 5, top + 4, 6, 9 );
        for( int y = 5; y <= 11; y += 2 )
          draw->Draw_Line( color_rec.data->background_style,
                           left + 5,  top + y,
                           left + 10, top + y );
      }
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
}

/******************************************************************************
 * h_scroll_indicator_draw
 */

Am_Define_Method( Am_Draw_Method, void, h_scroll_indicator_draw,
                ( Am_Object self, Am_Drawonable* draw,
                  int x_offset, int y_offset) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  bool active = self.Get( Am_ACTIVE );
  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      Am_Draw_Motif_Box( left, top, width, height, false, color_rec, draw );
      break;

    case Am_WINDOWS_LOOK_val:
      {
        am_rect r( left, top, width, height );
        draw_win_arrow_box( r, false, false, color_rec, draw );
      }
      break;
    
    case Am_MACINTOSH_LOOK_val:
      if( active )
      {
        draw->Draw_Rectangle( color_rec.data->highlight_style,
                              color_rec.data->background_style,
                              left + 1, top + 1, 15, 14 );
        draw->Draw_3_Lines( color_rec.data->shadow_style, Am_No_Style,
                            left,      top + 1,
                            left,      top + 14,
                            left + 15, top + 14,
                            left + 15, top + 1 );
        draw->Draw_Rectangle( Am_No_Style, color_rec.data->highlight_style,
                              left + 4, top + 5, 9, 6 );
        for( int x = 5; x <= 11; x += 2 )
          draw->Draw_Line( color_rec.data->background_style,
                           left + x, top + 5,
                           left + x, top + 10 );
      }
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
}

/******************************************************************************
 * scroll_narrow_side_size
 */

Am_Define_Formula( int, scroll_narrow_side_size )
{
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int size = 16; // default
  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      size = 20;
      break;

    case Am_WINDOWS_LOOK_val:
      size = 16;
      break;

    case Am_MACINTOSH_LOOK_val:
      size = 16;
      break;

    default:
      Am_Error ("Unknown Look parameter");
      break;
  }
  return size;
}

/******************************************************************************
 * scroll_indicator_pos
 *   self is the indicator.
 *   Interesting slots are in the scroll bar
 */

Am_Define_Formula( int, scroll_indicator_pos )
{
  int return_val;
  Am_Object scroll_bar = self.Get_Owner();
  if( !scroll_bar.Valid() )
    return 10;
  int minpos = scroll_bar.Get( Am_SCROLL_AREA_MIN );
  int maxpos = scroll_bar.Get( Am_SCROLL_AREA_MAX );
  Am_Value value_v;
  Am_Value value_1;
  Am_Value value_2;
  value_v = scroll_bar.Get(Am_VALUE);
  value_1 = scroll_bar.Get(Am_VALUE_1);
  value_2 = scroll_bar.Get(Am_VALUE_2);

  if( value_v.type == Am_INT && value_1.type == Am_INT
      && value_2.type == Am_INT )
    return_val = (int)Am_Clip_And_Map( (long)value_v, (long)value_1,
                                       (long)value_2, (long)minpos,
                                       (long)maxpos );
  else // calc in float and then convert result to integers
    return_val = (int)Am_Clip_And_Map( (float)value_v, (float)value_1,
                                       (float)value_2, (float)minpos,
                                       (float)maxpos );
  return return_val;
}

/******************************************************************************
 * v_scroll_area_max
 */

Am_Define_Formula( int, v_scroll_area_max )
{
  Am_Object indicator = self.Get_Object( Am_SCROLL_INDICATOR );
  int indicator_size = indicator.Get( Am_HEIGHT );
  int minpos = self.Get( Am_SCROLL_AREA_MIN );
  int maxpos = (int)self.Get( Am_SCROLL_AREA_SIZE ) - indicator_size + minpos;
  return maxpos;
}

Am_Define_Formula( int, just_arrows_height) {
  int width = self.Get( Am_WIDTH );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  // 2 for key selected and 2 for box
  int border = (look == Am_MOTIF_LOOK ) ? 4 : 0;
  int arrow_size = width - 2*border;
  int height = border + arrow_size + arrow_size + border;
  return height;
}

/******************************************************************************
 * h_scroll_area_max
 */

Am_Define_Formula( int, h_scroll_area_max )
{
  Am_Object indicator = self.Get_Object( Am_SCROLL_INDICATOR );
  int indicator_size = indicator.Get( Am_WIDTH );
  int minpos = self.Get( Am_SCROLL_AREA_MIN );
  int maxpos = (int)self.Get( Am_SCROLL_AREA_SIZE ) - indicator_size + minpos;
  return maxpos;
}

/******************************************************************************
 * scroll_indicator_size
 *   For motif and windows looks, the indicator size is proportional to the
 *   the visible area. The size is constant on the mac.
 */

Am_Define_Formula( int, scroll_indicator_size )
{
  Am_Object scroll_bar = self.Get_Owner();
  if( !scroll_bar.Valid() )
    return 10;
  Am_Widget_Look look = scroll_bar.Get( Am_WIDGET_LOOK );

  int size = 0;
  if( look == Am_MACINTOSH_LOOK )
    size = 16;
  else
  {
    int maxsize = scroll_bar.Get( Am_SCROLL_AREA_SIZE );
    float percent = scroll_bar.Get( Am_PERCENT_VISIBLE );
    if( percent > 1.0f )
      percent = 1.0f;
    size = (int)(maxsize*percent);
    if( size < 6 )
      size = 6;
  }
  return size;
}

/******************************************************************************
 * v_scroll_layout_formula
 *   Set the positions of the arrows and indicator based on size of scroll bar.
 *   Only sets the left and width of the indicator, height and top are formulas
 */

Am_Define_Formula( int, v_scroll_layout_formula )
{
  Am_Object arrow1 = self.Get_Object( Am_SCROLL_ARROW1 );
  Am_Object arrow2 = self.Get_Object( Am_SCROLL_ARROW2 );
  Am_Object indicator = self.Get_Object( Am_SCROLL_INDICATOR );
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  // 2 for key selected and 2 for box
  int border = (look == Am_MOTIF_LOOK ) ? 4 : 0;
  int arrow_size = width - 2*border;

  arrow1.Set( Am_LEFT, border );
  arrow1.Set( Am_TOP, border );
  arrow1.Set( Am_WIDTH, arrow_size );
  arrow1.Set( Am_HEIGHT, arrow_size ); // square

  arrow2.Set( Am_LEFT, border );
  arrow2.Set( Am_TOP, height - arrow_size - border ); // minus size of arrows
  arrow2.Set( Am_WIDTH, arrow_size );
  arrow2.Set( Am_HEIGHT, arrow_size ); // square

  if( look == Am_MOTIF_LOOK )
  {                                                   // bottom of top arrow
    self.Set( Am_SCROLL_AREA_MIN, arrow_size + border + 1 );
    self.Set( Am_SCROLL_AREA_SIZE, height - 2*arrow_size - 2*border - 2 );
  }
  else
  {
    self.Set( Am_SCROLL_AREA_MIN, arrow_size + border ); // bottom of top arrow
    self.Set( Am_SCROLL_AREA_SIZE, height - 2*arrow_size - 2*border );
  }
  if (indicator.Valid()) { //no indicator for Am_Vertical_Up_Down_Counter
    indicator.Set( Am_LEFT, border );
    indicator.Set( Am_WIDTH, width - 2*border );
  }
  return 0;  // return value not used
}

/****************************************************************************** * h_scroll_layout_formula
 *   Set the positions of the arrows and indicator based on size of scroll bar
 *   Only sets the height and top of the indicator, left and width are formulas
 */

Am_Define_Formula( int, h_scroll_layout_formula )
{
  Am_Object arrow1 = self.Get_Object( Am_SCROLL_ARROW1 );
  Am_Object arrow2 = self.Get_Object( Am_SCROLL_ARROW2 );
  Am_Object indicator = self.Get_Object( Am_SCROLL_INDICATOR );
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );

  int border = (look == Am_MOTIF_LOOK ) ? 4 : 0; // 2 for key sel and 2 for box
  int arrow_size = height - 2*border;
  
  arrow1.Set( Am_LEFT, border );
  arrow1.Set( Am_TOP, border );
  arrow1.Set( Am_WIDTH, arrow_size );
  arrow1.Set( Am_HEIGHT, arrow_size ); // square

  arrow2.Set( Am_LEFT, width - arrow_size - border ); // minus size of arrows
  arrow2.Set( Am_TOP, border );
  arrow2.Set( Am_WIDTH, arrow_size );
  arrow2.Set( Am_HEIGHT, arrow_size ); // square

  if( look == Am_MOTIF_LOOK )
  {                                                   //bottom of top arrow
    self.Set( Am_SCROLL_AREA_MIN, arrow_size + border + 1 );
    self.Set( Am_SCROLL_AREA_SIZE, width - 2*arrow_size - 2*border - 2 );
  }
  else
  {
    self.Set( Am_SCROLL_AREA_MIN, arrow_size + border ); //bottom of top arrow
    self.Set( Am_SCROLL_AREA_SIZE, width - 2*arrow_size - 2*border );
  }

  indicator.Set( Am_TOP, border );
  indicator.Set( Am_HEIGHT, height - 2*border );

  return 0;  // return value not used
}

/****************************************************************************** * scroll_draw
 *   draw the background, then draw the parts
 */

Am_Define_Method( Am_Draw_Method, void, scroll_draw,
                 (Am_Object self, Am_Drawonable* draw,
                  int x_offset, int y_offset) )
{
  int left = (int)self.Get( Am_LEFT ) + x_offset;
  int top = (int)self.Get( Am_TOP ) + y_offset;
  int width = self.Get( Am_WIDTH );
  int height = self.Get( Am_HEIGHT );
  bool active = self.Get( Am_ACTIVE );
  bool key_selected = self.Get( Am_KEY_SELECTED );
  Computed_Colors_Record color_rec = self.Get( Am_STYLE_RECORD );
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  // finally ready to draw it

  switch( look.value )
  {
    case Am_MOTIF_LOOK_val:
      if( key_selected) // draw box showing that the keyboard is over this item
        draw->Draw_Rectangle( Am_Key_Border_Line, Am_No_Style,
                              left, top, width, height);
      Am_Draw_Motif_Box( left+2, top+2, width-4, height-4,
                         true, color_rec, draw );
      break;

    case Am_WINDOWS_LOOK_val:
      draw->Draw_Rectangle( Am_No_Style, color_rec.data->highlight2_style,
                            left, top, width, height );
      break;

    case Am_MACINTOSH_LOOK_val:
      if( active )
      {
        float red, green, blue;
        color_rec.data->shadow_style.Get_Values( red, green, blue );

        Am_Style fill_style = Am_Style( red, green, blue,
                                        0, Am_CAP_BUTT,  Am_JOIN_MITER,
                                        Am_LINE_SOLID, Am_DEFAULT_DASH_LIST,
                                        Am_DEFAULT_DASH_LIST_LENGTH,
                                        Am_FILL_STIPPLED,
                                        Am_FILL_POLY_EVEN_ODD,
                                        Mac_Scroll_BG_Image_Array );
      
        draw->Draw_Rectangle( Am_Black, color_rec.data->foreground_style,
                              left, top, width, height );
        draw->Draw_Rectangle( Am_Black, fill_style, left, top, width, height );
      }
      else
        draw->Draw_Rectangle( Am_Black, color_rec.data->foreground_style,
                              left, top, width, height );

      break;

    default:
      Am_Error("Illegal look detected in scroll_draw");
  }

  // now call the prototype's method to draw the parts
  Am_Draw_Method method;
  method = Am_Aggregate.Get( Am_DRAW_METHOD );
  method.Call( self, draw, x_offset, y_offset );
}

/////////////////////////////////////////////////////////////////
// For the interactors and commands
/////////////////////////////////////////////////////////////////

//////////////////  Indicator //////////////////////

//For the where test of the indicator

Am_Define_Method(Am_Where_Method, Am_Object, in_scroll_indicator,
                 (Am_Object /* inter */, Am_Object object,
                  Am_Object event_window, int x, int y)) {
  Am_Object indicator = object.Get_Object(Am_SCROLL_INDICATOR);
  if( Am_Point_In_All_Owners(indicator, x, y, event_window)
      &&( Am_Point_In_Obj( indicator, x, y, event_window).Valid()))
    return indicator;

  //if get here, then not indicator
  return Am_No_Object;
}

void set_up_for_undo(Am_Object inter_command, Am_Object scroll_command,
                     Am_Object scrollbar) {
  //set old value to current value
  Am_Value old_value;
  old_value=scrollbar.Peek(Am_VALUE);
  scroll_command.Set( Am_OLD_VALUE, old_value, Am_OK_IF_NOT_THERE);
  inter_command.Set( Am_OLD_VALUE, old_value, Am_OK_IF_NOT_THERE);
}

Am_Define_Method(Am_Object_Method, void, save_pos_for_undo,
                 (Am_Object command_obj)) {
  Am_Object inter = command_obj.Get_Owner();
  Am_Object scrollbar = inter.Get_Owner();
  Am_Object scroll_command = scrollbar.Get_Object(Am_COMMAND);
  set_up_for_undo(command_obj, scroll_command, scrollbar);
  //find the top-level parent and mark it as not queued until the
  //final do action
  Am_Object obj;
  Am_Value value;
  obj = scroll_command;
  while( true) {
    value=obj.Peek(Am_IMPLEMENTATION_PARENT);
    if( value.Valid()) {
      if( value.type != Am_OBJECT)
        break; //already marked as not queued
    }
    else { // not valid
      obj.Set( Am_IMPLEMENTATION_PARENT, Am_MARKER_FOR_SCROLL_INC);
      break;
    }
    obj = value;
  }
}

//clear the Am_IMPLEMENTATION_PARENT marker
Am_Define_Method(Am_Object_Method, void, clear_save_pos_for_undo,
                 (Am_Object command_obj)) {
  Am_Object obj, parent;
  Am_Value value;
  obj = command_obj;
  while( true) {
    value=obj.Peek(Am_IMPLEMENTATION_PARENT);
    if (value.Valid()) {
      if( value.type != Am_OBJECT) {
        if( value == Am_MARKER_FOR_SCROLL_INC)
          obj.Set( Am_IMPLEMENTATION_PARENT, 0);
        break;
      }
    }
    else { // not valid
      break;
    }
    obj = value;
  }
}

//clear the Am_IMPLEMENTATION_PARENT marker and also restore the value
//of the widget
Am_Define_Method(Am_Object_Method, void, abort_save_pos_for_undo,
                 (Am_Object command_obj)) {
  clear_save_pos_for_undo_proc(command_obj);
  //first owner is the interactor
  Am_Object scrollbar = command_obj.Get_Owner().Get_Owner();
  Am_Object scroll_command = scrollbar.Get_Object(Am_COMMAND);
  Am_Value old_value;
  old_value=scroll_command.Peek(Am_OLD_VALUE);
  scrollbar.Set(Am_VALUE, old_value);
  //cout << "...Aborting, setting " << scrollbar << " value with " <<
  //   old_value  << " from " << scroll_command << endl << flush;
}

//internal procedure used by horiz and vertical
void set_scrollbar_and_commands(Am_Value value, Am_Object scrollbar,
                                Am_Object scroll_command,
                                Am_Object inter_command,
				bool set_scrollbar = true) {
  if (set_scrollbar)
    scrollbar.Set( Am_VALUE, value );
  if( scroll_command.Valid())
    scroll_command.Set( Am_VALUE, value);
  inter_command.Set( Am_VALUE, value);
}

void scroll_indicator_inter_interim_do(Am_Object inter, int cur_value) {
  Am_Object scrollbar = inter.Get_Owner();
  if( scrollbar.Valid() ) {
    Am_Object scroll_command;
    scroll_command = scrollbar.Get(Am_COMMAND);
    Am_Object indicator;
    indicator = inter.Get(Am_START_OBJECT);
    //map the x or y value to the current value
    Am_Value val1, val2, value;
    val1=scrollbar.Peek(Am_VALUE_1);
    val2=scrollbar.Peek(Am_VALUE_2);
    int minpos = scrollbar.Get(Am_SCROLL_AREA_MIN);
    int maxpos = scrollbar.Get(Am_SCROLL_AREA_MAX);

    //now map the value
    if( val1.type == Am_INT && val2.type == Am_INT) {
      value = Am_Clip_And_Map((long)cur_value, (long)minpos, (long)maxpos,
                              (long)val1, (long)val2);
    }
    else { // calc in float
      value = Am_Clip_And_Map((float)cur_value, (float)minpos,
                              (float)maxpos, (float)val1, (float)val2);
    }
    set_scrollbar_and_commands(value, scrollbar, scroll_command,
                               inter.Get_Object(Am_COMMAND));
    //pretend this is a final function, so call all parent methods
    Am_Move_Grow_Register_For_Undo(inter);
  }
}

Am_Define_Method(Am_Current_Location_Method, void,
                 v_scroll_indicator_inter_interim_do,
                 (Am_Object inter, Am_Object /* object_modified */,
                  Am_Inter_Location data)) {
  int left, top, w, h;
  data.Get_Points(left, top, w, h);
  scroll_indicator_inter_interim_do(inter, top);
}

Am_Define_Method(Am_Current_Location_Method, void,
                 h_scroll_indicator_inter_interim_do,
                 (Am_Object inter, Am_Object /* object_modified */,
                  Am_Inter_Location data)) {
  int left, top, w, h;
  data.Get_Points(left, top, w, h);
  scroll_indicator_inter_interim_do(inter, left);
}

//////////////////  Arrows //////////////////////

// For the where test of arrow interactor: see if in an arrow
Am_Define_Method(Am_Where_Method, Am_Object, in_scroll_arrows,
                 (Am_Object /* inter */, Am_Object object,
                  Am_Object event_window, int x, int y)) {
  Am_Object result = Am_No_Object;
  if (Am_Point_In_All_Owners(object, x, y, event_window))
    result = Am_Point_In_Part( object, x, y, event_window, false);
  if( result.Valid() ) {
    if( result == object.Get_Object(Am_SCROLL_ARROW1) ||
        result == object.Get_Object(Am_SCROLL_ARROW2))
      return result;
    else return Am_No_Object; // not an arrow
  }
  else return Am_No_Object;
}

#if 0
void int_adjust_value(Am_Value &output_value, int cur_val,
                       bool towards_val1, int inc_amt, int val1, int val2) {
  int final_val;
  if( val1 < val2) {
    if( towards_val1) {
      if( cur_val > val2) cur_val = val2; //if starts off too big
      final_val = cur_val - inc_amt;
      if( final_val < val1) final_val = val1;
    }
    else {
      if( cur_val < val1) cur_val = val1; //if starts off too small
      final_val = cur_val + inc_amt;
      if( final_val > val2) final_val = val2;
    }
  }
  else {  // val1 >= val2
    if( towards_val1) {
      if( cur_val < val2) cur_val = val2; //if starts off too small
      final_val = cur_val + inc_amt;
      if( final_val > val1) final_val = val1;
    }
    else {
      if( cur_val > val1) cur_val = val1; //if starts off too big
      final_val = cur_val - inc_amt;
      if( final_val < val2) final_val = val2;
    }
  }
  output_value = final_val;
}
void float_adjust_value(Am_Value &output_value, float cur_val,
                        bool towards_val1, float inc_amt,
                        float val1, float val2) {
  float final_val;
  if( val1 < val2) {
    if( towards_val1) {
      if( cur_val > val2) cur_val = val2; //if starts off too big
      final_val = cur_val - inc_amt;
      if( final_val < val1) final_val = val1;
    }
    else {
      if( cur_val < val1) cur_val = val1; //if starts off too small
      final_val = cur_val + inc_amt;
      if( final_val > val2) final_val = val2;
    }
  }
  else {
    if( towards_val1) {
      if( cur_val < val2) cur_val = val2; //if starts off too small
      final_val = cur_val + inc_amt;
      if( final_val > val1) final_val = val1;
    }
    else {
      if( cur_val > val1) cur_val = val1; //if starts off too big
      final_val = cur_val - inc_amt;
      if( final_val < val2) final_val = val2;
    }
  }
  output_value = final_val;
}

void adjust_value(Am_Value &cur_value, bool towards_val1,
                      Am_Value inc_amt, Am_Value val1, Am_Value val2) {
  if( inc_amt.type == Am_INT && val1.type == Am_INT && val2.type == Am_INT)
    int_adjust_value(cur_value, (int)cur_value, towards_val1, inc_amt, val1,
                     val2);
  else float_adjust_value(cur_value, (float)cur_value, towards_val1,
                          (float) inc_amt, (float)val1, (float)val2 );
}

Am_Define_Method(Am_Object_Method, void, scroll_arrow_inter_command_do,
                 (Am_Object command)) {
  Am_Object arrow;
  Am_Object inter = command.Get_Owner();
  if(inter.Valid() ) {
    arrow = inter.Get(Am_INTERIM_VALUE);
    Am_Object scrollbar = inter.Get_Owner();
    if( scrollbar.Valid() ) {
      Am_Object scroll_command;
      scroll_command = scrollbar.Get(Am_COMMAND);
      if( scroll_command.Valid() )
        set_up_for_undo(command, scroll_command, scrollbar);

      Am_Value inc_amt, cur_value, val1, val2;
      inc_amt=scrollbar.Peek(Am_SMALL_INCREMENT);
      val1=scrollbar.Peek(Am_VALUE_1);
      val2=scrollbar.Peek(Am_VALUE_2);
      cur_value=scrollbar.Peek(Am_VALUE);
      bool towards_val1;
      if( arrow == scrollbar.Get_Object(Am_SCROLL_ARROW1))
        towards_val1 = true;
      else if( arrow == scrollbar.Get_Object(Am_SCROLL_ARROW2))
        towards_val1 = false;
      else Am_Error("command value not one of the arrows");
       adjust_value(cur_value, towards_val1, inc_amt, val1, val2);

      set_scrollbar_and_commands(cur_value, scrollbar, scroll_command,
                                 command);
    }
  }
}
#endif

Am_Define_Method(Am_Where_Method, Am_Object, never_go_outside,
                 (Am_Object inter, Am_Object /*object*/,
                  Am_Object /*event_window*/, int /*x*/, int /*y*/)) {
  return inter.Get (Am_START_OBJECT);
}

Am_Define_Method(Am_Object_Method, void, scroll_arrow_inter_command_start,
                 (Am_Object command)) {
  save_pos_for_undo.Call (command);
  
  Am_Object arrow;
  Am_Object inter = command.Get_Owner();
  if(inter.Valid() ) {
    arrow = inter.Get(Am_START_OBJECT);
    Am_Object scrollbar = inter.Get_Owner();
    if( scrollbar.Valid() ) {
      Am_Object scroll_command;
      scroll_command = scrollbar.Get(Am_COMMAND);

      Am_Value cur_value, val1, val2;
      val1=scrollbar.Peek(Am_VALUE_1);
      val2=scrollbar.Peek(Am_VALUE_2);
      cur_value=scrollbar.Peek(Am_VALUE);

      // clip cur_value inside val1 and val2
      if( val1.type == Am_INT && val2.type == Am_INT) {
	cur_value = Am_Clip_And_Map((long)cur_value, 
				    (long)val1, (long)val2,
				    (long)val1, (long)val2);
      }
      else { // calc in float
	cur_value = Am_Clip_And_Map((float)cur_value, 
				    (float)val1, (float)val2,
				    (float)val1, (float)val2);
      }
      scrollbar.Set (Am_VALUE, cur_value);
      
      // choose the appropriate target value
      if( arrow == scrollbar.Get_Object(Am_SCROLL_ARROW1))
	cur_value = val1;
      else if( arrow == scrollbar.Get_Object(Am_SCROLL_ARROW2))
	cur_value = val2;
      else Am_Error("command value not one of the arrows");
      
      // set up the animation
      Am_Object animator = Am_Get_Animator (scrollbar, Am_VALUE);
      animator.Set (Am_SMALL_INCREMENT, scrollbar.Get (Am_SMALL_INCREMENT));
      animator.Set (Am_INTERACTOR, inter);

      // trigger the animation
      scrollbar.Set (Am_VALUE, cur_value, Am_WITH_ANIMATION);
      set_scrollbar_and_commands(cur_value, scrollbar, scroll_command,
                                 command, false);
    }
  }
}

Am_Define_Method(Am_Object_Method, void, scroll_arrow_inter_command_do,
                 (Am_Object command)) {
  Am_Object inter = command.Get_Owner();
  if(inter.Valid() ) {
    Am_Object scrollbar = inter.Get_Owner();
    if( scrollbar.Valid() ) {
      Am_Object scroll_command;
      scroll_command = scrollbar.Get(Am_COMMAND);

      Am_Value cur_value;
      cur_value=scrollbar.Peek(Am_VALUE);

      set_scrollbar_and_commands(cur_value, scrollbar, scroll_command, command);
      command.Set( Am_OLD_VALUE, scroll_command.Get (Am_OLD_VALUE), Am_OK_IF_NOT_THERE);

    }
  }

  clear_save_pos_for_undo.Call (command);
}

//////////////////  Background (page) //////////////////////

#if 0
//internal procedure used by both horiz and vert
void scroll_page_inter_command_do( Am_Object command, bool vertical,
                                   Am_Object ref_obj, int mousex, int mousey) {
  Am_Object inter = command.Get_Owner();
  if(inter.Valid() ) {
    Am_Object scrollbar = inter.Get_Owner();
    if( scrollbar.Valid() ) {
      Am_Object scroll_command;
      scroll_command = scrollbar.Get(Am_COMMAND);
      if( scroll_command.Valid() )
        set_up_for_undo(command, scroll_command, scrollbar);
      Am_Value inc_amt;
      Am_Value cur_value;
      Am_Value val1;
      Am_Value val2;
      inc_amt=scrollbar.Peek(Am_LARGE_INCREMENT);
      val1=scrollbar.Peek(Am_VALUE_1);
      val2=scrollbar.Peek(Am_VALUE_2);
      cur_value=scrollbar.Peek(Am_VALUE);
      bool towards_val1 = false;
      // see if click is above the indicator
      Am_Object indicator = scrollbar.Get_Object(Am_SCROLL_INDICATOR);
      int indicator_pos, x, y;
      if( Am_Translate_Coordinates( ref_obj, mousex, mousey,
                                    scrollbar, x, y)) {
        if( vertical) {
          indicator_pos = indicator.Get(Am_TOP);
          if( indicator_pos > y) towards_val1 = true;
        }
        else { //horizontal
          indicator_pos = indicator.Get(Am_LEFT);
          if( indicator_pos > x) towards_val1 = true;
        }

        adjust_value(cur_value, towards_val1, inc_amt, val1, val2);
        set_scrollbar_and_commands(cur_value, scrollbar, scroll_command,
                                   command);
      }
    }
  }
}

Am_Define_Method(Am_Mouse_Event_Method, void,
                 vertical_scroll_page_inter_command_do,
                ( Am_Object command, int mouse_x, int mouse_y,
                  Am_Object ref_obj, Am_Input_Char /* ic */)) {
  scroll_page_inter_command_do(command, true, ref_obj, mouse_x, mouse_y);
}
Am_Define_Method(Am_Mouse_Event_Method, void,
                 horizontal_scroll_page_inter_command_do,
                ( Am_Object command, int mouse_x, int mouse_y,
                  Am_Object ref_obj, Am_Input_Char /* ic */)) {
  scroll_page_inter_command_do(command, false, ref_obj, mouse_x, mouse_y);
}
#endif

//internal procedure used by both horiz and vert
void scroll_page_inter_command_start ( Am_Object command, bool vertical,
			      Am_Object ref_obj, int mousex, int mousey) {
  save_pos_for_undo.Call (command);
  
  Am_Object inter = command.Get_Owner();
  if(inter.Valid() ) {
    Am_Object scrollbar = inter.Get_Owner();
    if( scrollbar.Valid() ) {
      Am_Object scroll_command;
      scroll_command = scrollbar.Get(Am_COMMAND);

      Am_Value cur_value, val1, val2;
      val1=scrollbar.Peek(Am_VALUE_1);
      val2=scrollbar.Peek(Am_VALUE_2);
      cur_value=scrollbar.Peek(Am_VALUE);

      // clip cur_value inside val1 and val2
      if( val1.type == Am_INT && val2.type == Am_INT) {
	cur_value = Am_Clip_And_Map((long)cur_value, 
				    (long)val1, (long)val2,
				    (long)val1, (long)val2);
      }
      else { // calc in float
	cur_value = Am_Clip_And_Map((float)cur_value, 
				    (float)val1, (float)val2,
				    (float)val1, (float)val2);
      }
      scrollbar.Set (Am_VALUE, cur_value);
      
      // choose the appropriate target value,
      // based on whether click is above the indicator
      bool towards_val1 = false;
      Am_Object indicator = scrollbar.Get_Object(Am_SCROLL_INDICATOR);
      int indicator_pos, x, y;
      if( Am_Translate_Coordinates( ref_obj, mousex, mousey,
                                    scrollbar, x, y)) {
        if( vertical) {
          indicator_pos = indicator.Get(Am_TOP);
          if( indicator_pos > y) towards_val1 = true;
        }
        else { //horizontal
          indicator_pos = indicator.Get(Am_LEFT);
          if( indicator_pos > x) towards_val1 = true;
        }
      }
      if( towards_val1 )
	cur_value = val1;
      else
	cur_value = val2;
      
      // set up the animation
      Am_Object animator = Am_Get_Animator (scrollbar, Am_VALUE);
      animator.Set (Am_SMALL_INCREMENT, scrollbar.Get (Am_LARGE_INCREMENT));
      animator.Set (Am_INTERACTOR, inter);

      // trigger the animation
      scrollbar.Set (Am_VALUE, cur_value, Am_WITH_ANIMATION);
      set_scrollbar_and_commands(cur_value, scrollbar, scroll_command,
                                 command, false);
    }
  }
}

Am_Define_Method(Am_Mouse_Event_Method, void,
                 vertical_scroll_page_inter_command_start,
                ( Am_Object command, int mouse_x, int mouse_y,
                  Am_Object ref_obj, Am_Input_Char /* ic */)) {
  scroll_page_inter_command_start(command, true, ref_obj, mouse_x, mouse_y);
}

Am_Define_Method(Am_Mouse_Event_Method, void,
                 horizontal_scroll_page_inter_command_start,
                ( Am_Object command, int mouse_x, int mouse_y,
                  Am_Object ref_obj, Am_Input_Char /* ic */)) {
  scroll_page_inter_command_start(command, false, ref_obj, mouse_x, mouse_y);
}

/////////////  Animation (autorepeat for arrows and page) ////////////

Am_Define_Method(Am_Object_Method, void, scrollbar_timer_start,
		 (Am_Object interp)) 
{
  // copy parameters down from scrollbar to interpolator
  Am_Object scrollbar = interp.Get (Am_OPERATES_ON);
  interp.Set (Am_INITIAL_DELAY, scrollbar.Get (Am_INITIAL_DELAY));
  interp.Set (Am_REPEAT_DELAY, scrollbar.Get (Am_REPEAT_DELAY));

  // set a one-shot timer for INITIAL_DELAY
  Am_Time delay = interp.Get(Am_INITIAL_DELAY);
  Am_Register_Timer(delay, interp, Am_ANIMATION_METHOD, true);
  interp.Set (Am_FIRST_TICK, true);

  // invoke the stepper animation method for time == INITIAL_DELAY
  // (so that value jumps immediately)
  Am_Timer_Method method = Am_Stepping_Animator.Get (Am_ANIMATION_METHOD);
  method.Call (interp, delay);

  // propagate values to the scrollbar's various parts
  Am_Object inter = interp.Get (Am_INTERACTOR);
  Am_Object inter_command = inter.Get (Am_COMMAND);
  Am_Object scroll_command = scrollbar.Get (Am_COMMAND);
  set_scrollbar_and_commands(interp.Get (Am_VALUE), scrollbar, 
			     scroll_command,
			     inter_command,
			     false);
  //pretend this is a final function, so call all parent methods
  Am_Register_For_Undo(inter, inter_command, 
		       Am_No_Object, Am_No_Location, NULL);
}

Am_Define_Method (Am_Timer_Method, void, scrollbar_timer,
		  (Am_Object interp, const Am_Time& elapsed_time))
{
  bool first_tick = interp.Get (Am_FIRST_TICK);

  if (first_tick) {
    // set a continuous timer for every REPEAT_DELAY
    Am_Time delay = interp.Get (Am_REPEAT_DELAY);
    Am_Register_Timer(delay, interp, Am_ANIMATION_METHOD, false);
    interp.Set (Am_FIRST_TICK, false);
  }

  // call the stepper now
  Am_Timer_Method method = Am_Stepping_Animator.Get (Am_ANIMATION_METHOD);
  method.Call (interp, elapsed_time);

  // propagate values to the scrollbar's various parts
  Am_Object scrollbar = interp.Get (Am_OPERATES_ON);
  Am_Object inter = interp.Get (Am_INTERACTOR);
  Am_Object inter_command = inter.Get (Am_COMMAND);
  Am_Object scroll_command = scrollbar.Get (Am_COMMAND);
  set_scrollbar_and_commands(interp.Get (Am_VALUE), scrollbar, 
			     scroll_command,
			     inter_command,
			     false);
  //pretend this is a final function, so call all parent methods
  Am_Register_For_Undo(inter, inter_command, 
		       Am_No_Object, Am_No_Location, NULL);
}

///////////////////////////////////////////////////////////////////////////
// Scrolling_Group
///////////////////////////////////////////////////////////////////////////

// scroll bar has SCROLL_BORDER pixels blank border all around.
// SCROLL_EXTRA_WIDTH is 2*SCROLL_BORDER
// SCROLL_MARGIN is # pixels between the scroll bars and the clip region
//#define SCROLL_BORDER 0 //2
//#define SCROLL_EXTRA_WIDTH 0 //4
#define SCROLL_MARGIN 2

// Am_Translate_Coordinates_Method:
//  - Stored in slot Am_TRANSLATE_COORDINATES_METHOD
//  - Given a point in the coordinate system of the group, converts
//    it to be in the coordinate system of scrolling group's owner
Am_Define_Method(Am_Translate_Coordinates_Method, void,
                 scroll_group_translate_coordinates,
                ( const Am_Object& self, const Am_Object& for_part,
                  int in_x, int in_y, int& out_x, int& out_y))
{
  bool using_constraint = true;
  //first, offset by the origin of the inside of the group
  if( using_constraint)
  {
    out_x = (int)self.Get( Am_LEFT) + in_x;
    out_y = (int)self.Get( Am_TOP) + in_y;
  }
  else
  {
    out_x = (int)self.Get( Am_LEFT) + in_x;
    out_y = (int)self.Get( Am_TOP) + in_y;
  }

  Am_Object h_scroller = self.Get_Object(Am_H_SCROLLER);
  Am_Object v_scroller = self.Get_Object(Am_V_SCROLLER);
  if( for_part != h_scroller && for_part != v_scroller)
  {
    // then is for something on the inside of the scrolling region
    if( using_constraint)
    {
      out_x += (int)self.Get( Am_CLIP_LEFT) - (int)self.Get(Am_X_OFFSET);
      out_y += (int)self.Get( Am_CLIP_TOP)  - (int)self.Get(Am_Y_OFFSET);
    }
    else
    {
      out_x += (int)self.Get( Am_CLIP_LEFT) - (int)self.Get(Am_X_OFFSET);
      out_y += (int)self.Get( Am_CLIP_TOP)  - (int)self.Get(Am_Y_OFFSET);
    }
  }
}

//coords are in my owner's
bool
in_inside_scroll_group(
  Am_Object in_obj,
  int x,
  int y )
{
  if( (bool)in_obj.Get( Am_VISIBLE))
  {
    int left, top, clipleft, cliptop, clipwidth, clipheight;
    left = in_obj.Get(Am_LEFT);
    top = in_obj.Get(Am_TOP);
    clipleft = in_obj.Get(Am_CLIP_LEFT);
    cliptop = in_obj.Get(Am_CLIP_TOP);
    if( x < left + clipleft || y < top + cliptop )
      return false;
    clipwidth = in_obj.Get(Am_CLIP_WIDTH);
    clipheight = in_obj.Get(Am_CLIP_HEIGHT);
    if( ( x >= left + clipleft + clipwidth ) || ( y >= top + cliptop + clipheight ) )
      return false;
    return true;
  }
  else
    return false;
}

//coords are w.r.t ref_obj, make them be w.r.t owner of in_obj
void
translate_coords_to_owner(
  Am_Object in_obj,
  Am_Object ref_obj,
  int x, int y, int &x1, int &y1 )
{
  Am_Object owner;
  owner = in_obj.Get_Owner();
  if( owner.Valid() && owner == ref_obj)
  {
    x1 = x;
    y1 = y;  //already OK
  }
  else //not the owner, use expensive transformation
    Am_Translate_Coordinates( ref_obj, x, y, owner, x1, y1 );
}

Am_Define_Method(Am_Point_In_Method, Am_Object, scroll_group_point_in_obj,
                ( const Am_Object& in_obj, int x, int y,
                  const Am_Object& ref_obj))
{
  if( (bool)in_obj.Get( Am_VISIBLE))
  {
    Am_Object owner = in_obj.Get_Owner();
    if( owner.Valid())
    {
      if( owner != ref_obj) // otherwise x,y ok
        Am_Translate_Coordinates( ref_obj, x, y, owner, x, y);
      int left = in_obj.Get(Am_LEFT);
      int top = in_obj.Get(Am_TOP);
      if( (x < left) ||( y < top)) return Am_No_Object;
      if( (x >= left + (int)in_obj.Get( Am_WIDTH)) ||
          (y >= top + (int)in_obj.Get( Am_HEIGHT)))
        return Am_No_Object;
      return in_obj;
    }
  }
  return Am_No_Object;
}

// x and y are in coord of ref_obj
Am_Define_Method(Am_Point_In_Or_Self_Method, Am_Object, scroll_group_point_in_part,
                ( const Am_Object& in_obj, int x, int y,
                  const Am_Object& ref_obj,
                  bool want_self, bool want_groups))
{
  //only works for objects inside my clip region, so can't "click through"
  //the scroll bars.
  int x1, y1;
  translate_coords_to_owner(in_obj, ref_obj, x, y, x1, y1);
  if( in_inside_scroll_group( in_obj, x1, y1)) {
    int offx, offy;
    scroll_group_translate_coordinates_proc(in_obj, Am_No_Object,
                                            0, 0, offx, offy);
    x1 -= offx;  //translate coordinates to the inside
    y1 -= offy;  //of the scrolling-group, which is the owner of the parts
    Am_Value_List comp;
    comp = in_obj.Get( Am_GRAPHICAL_PARTS);
    Am_Object object;
    for( comp.End() ; !comp.First() ; comp.Prev() ) {
      object = comp.Get() ;
      if( (want_groups || !am_is_group_and_not_pretending(object)) &&
          Am_Point_In_Obj( object, x1, y1, in_obj))
        return object;
    }
    //went through list, not in a part
    if( want_self && (want_groups || !am_is_group_and_not_pretending(in_obj)))
      return in_obj;
    else return Am_No_Object;
  }
  else return Am_No_Object;
}

Am_Define_Method(Am_Point_In_Or_Self_Method, Am_Object, scroll_group_point_in_leaf,
                ( const Am_Object& in_obj, int x, int y,
                  const Am_Object& ref_obj,
                  bool want_self, bool want_groups))
{
  //only works for objects inside my clip region, so can't "click through"
  //the scroll bars.
  int x1, y1;
  translate_coords_to_owner( in_obj, ref_obj, x, y, x1, y1 );
  if( in_inside_scroll_group( in_obj, x, y ) )
  {
    Am_Value val;
    val=in_obj.Peek(Am_PRETEND_TO_BE_LEAF);
    if( val.Valid() )
      return in_obj; // true if slot exists and is non-null
    else
    {
      int offx, offy;
      scroll_group_translate_coordinates_proc( in_obj, Am_No_Object, 0, 0,
                                               offx, offy);
      x1 -= offx;  //translate coordinates to the inside
      y1 -= offy;  //of the scrolling-group, which is the owner of the parts
      Am_Value_List comp;
      comp = in_obj.Get( Am_GRAPHICAL_PARTS );
      Am_Object object, ret;
      for( comp.End(); !comp.First(); comp.Prev() )
      {
        object = comp.Get() ;
        ret = Am_Point_In_Leaf( object, x1, y1, in_obj, want_self, want_groups );
        if( ret.Valid() )
          return ret;
      }
      //went through list, not in a part
      if( want_self && ( want_groups || !am_is_group_and_not_pretending( in_obj ) ) )
        return in_obj;
      else
        return Am_No_Object;
    }
  }
  else // if not in me, return NULL
    return Am_No_Object;
}

int get_scroll_border_thickness( Am_Object &self)
{
  Am_Value v;
  v = self.Peek(Am_LINE_STYLE);
  if( v.Valid() )
  {
    Am_Style border_style = v;
    short thickness;
    Am_Line_Cap_Style_Flag cap;
    border_style.Get_Line_Thickness_Values( thickness, cap );
    if( thickness == 0 )
      thickness = 1;
    return
      thickness;
  }
  else
    return 0;
}

// The left of the area that is used for the view of the insides
Am_Define_Formula(int, scroll_clip_left)
{
  int ret = get_scroll_border_thickness(self);
  if( (bool)self.Get(Am_V_SCROLL_BAR) && (bool)self.Get(Am_V_SCROLL_BAR_ON_LEFT))
  {
    Am_Object v_scroller = self.Get_Object(Am_V_SCROLLER);
    Am_Widget_Look look = v_scroller.Get( Am_WIDGET_LOOK );
    int extra_width = (look == Am_MOTIF_LOOK) ? 4 : 0;
    int scroll_width = (int)v_scroller.Get( Am_WIDTH ) - extra_width + SCROLL_MARGIN;
    ret += scroll_width;
  }
  return ret;
}

// The top of the area that is used for the view of the insides
Am_Define_Formula(int, scroll_clip_top)
{
  int ret = get_scroll_border_thickness(self);
  if( (bool)self.Get(Am_H_SCROLL_BAR) && ( bool)self.Get(Am_H_SCROLL_BAR_ON_TOP))
  {
    Am_Object h_scroller = self.Get_Object(Am_H_SCROLLER);
    Am_Widget_Look look = h_scroller.Get( Am_WIDGET_LOOK );
    int extra_width = (look == Am_MOTIF_LOOK) ? 4 : 0;
    int scroll_height = (int)h_scroller.Get(Am_HEIGHT) - extra_width + SCROLL_MARGIN;
    ret += scroll_height;
  }
  return ret;
}

// The area of the group that is used for the view of the insides
Am_Define_Formula(int, scroll_clip_width)
{
  int borderwidth = 2*get_scroll_border_thickness(self);
  int group_width = (int)self.Get(Am_WIDTH) - borderwidth;
  if( (bool)self.Get(Am_V_SCROLL_BAR))
  {
    Am_Object v_scroller = self.Get_Object(Am_V_SCROLLER);
    Am_Widget_Look look = v_scroller.Get( Am_WIDGET_LOOK );
    int extra_width = (look == Am_MOTIF_LOOK) ? 4 : 0;
    return group_width - (int) v_scroller.Get(Am_WIDTH) + extra_width - SCROLL_MARGIN;
  }
  else
    return group_width;
}

// The area of the group that is used for the view of the insides
Am_Define_Formula(int, scroll_clip_height)
{
  int borderwidth = 2*get_scroll_border_thickness(self);
  int group_height = (int)self.Get(Am_HEIGHT) - borderwidth;
  if( (bool)self.Get(Am_H_SCROLL_BAR))
  {
    Am_Object h_scroller = self.Get_Object(Am_H_SCROLLER);
    Am_Widget_Look look = h_scroller.Get( Am_WIDGET_LOOK );
    int extra_width = (look == Am_MOTIF_LOOK) ? 4 : 0;
    return group_height - (int) h_scroller.Get(Am_HEIGHT) + extra_width - SCROLL_MARGIN;
  }
  else
    return group_height;
}

//Draw the scroll bars, then the background, then transform the
//coordinates and draw the graphical parts.
Am_Define_Method(Am_Draw_Method, void, scrolling_group_draw,
                 (Am_Object self, Am_Drawonable* drawonable,
                  int x_offset, int y_offset))
{
  int myleft = (int)self.Get( Am_LEFT) + x_offset;
  int mytop = (int)self.Get( Am_TOP) + y_offset;
  int mywidth = self.Get( Am_WIDTH);
  int myheight = self.Get( Am_HEIGHT);
  //Am_CLIP_LEFT and TOP are in my coord system, need window coords so add
  int insideleft = (int)self.Get(Am_CLIP_LEFT) + myleft;
  int insidetop = (int)self.Get(Am_CLIP_TOP) + mytop;
  int insideheight = self.Get(Am_CLIP_HEIGHT);
  int insidewidth = self.Get(Am_CLIP_WIDTH);
  Am_State_Store* state;
  if( mywidth && myheight) {
    // set a clip region here in case group is too small for the scroll bars
    drawonable->Push_Clip( myleft, mytop, mywidth, myheight);

    if( (bool)self.Get(Am_H_SCROLL_BAR)) {
      Am_Object h_scroller = self.Get_Object(Am_H_SCROLLER);
      state = Am_State_Store::Narrow( h_scroller.Get( Am_PREV_STATE));
      if( state->Visible( drawonable, myleft, mytop))
        Am_Draw( h_scroller, drawonable, myleft, mytop);
    }
    if( (bool)self.Get(Am_V_SCROLL_BAR)) {
      Am_Object v_scroller = self.Get_Object(Am_V_SCROLLER);
      state = Am_State_Store::Narrow( v_scroller.Get( Am_PREV_STATE));
      if( state->Visible( drawonable, myleft, mytop))
        Am_Draw( v_scroller, drawonable, myleft, mytop);
    }
    // draw the background
    Am_Style background_style, line_style;
    // use Am_INNER_FILL_STYLE if supplied, otherwise Am_FILL_STYLE
    Am_Value value;
    value=self.Peek(Am_INNER_FILL_STYLE);
    if( Am_Type_Class( value.type) == Am_WRAPPER) background_style = value;
    else background_style = self.Get(Am_FILL_STYLE);
    //  filled with background_style
    value=self.Peek(Am_LINE_STYLE);
    short thickness = 0;
    if( value.Valid())
    {
      line_style = value;  //else leave line_style NULL
      Am_Line_Cap_Style_Flag cap;
      line_style.Get_Line_Thickness_Values( thickness, cap);
      if( thickness == 0)
        thickness = 1;
    }
    drawonable->Draw_Rectangle (line_style, background_style,
                                insideleft-thickness, insidetop-thickness,
                                insidewidth  + 2*thickness,
                                insideheight + 2*thickness);

    drawonable->Pop_Clip(); // done scroll bar drawin.type
    //set clip of inside
    drawonable->Push_Clip (insideleft, insidetop, insidewidth, insideheight);
    //now offset the area by the x and y offsets
    int x_offset = self.Get(Am_X_OFFSET);
    int y_offset = self.Get(Am_Y_OFFSET);
    insideleft -= x_offset;
    insidetop -= y_offset;

    Am_Value_List components;
    components = self.Get (Am_GRAPHICAL_PARTS);
    Am_Object item;
    for (components.Start(); !components.Last(); components.Next())
    {
      item = components.Get();
      state = Am_State_Store::Narrow (item.Get (Am_PREV_STATE));
      if (state->Visible (drawonable, insideleft, insidetop))
        Am_Draw (item, drawonable, insideleft, insidetop);
    }
    drawonable->Pop_Clip();
  }
}

Am_Define_Method(Am_Invalid_Method, void, scrolling_group_invalid,
                 (Am_Object group, Am_Object which_part,
                  int left, int top, int width, int height))
{
  Am_Object owner = group.Get_Owner(Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
  if( owner )
  {
    int my_left = group.Get (Am_LEFT, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    int my_top = group.Get (Am_TOP, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    int my_width = group.Get (Am_WIDTH, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    int my_height = group.Get (Am_HEIGHT, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    int final_left, final_top, final_width, final_height;
    Am_Object h_scroller = group.Get_Object(Am_H_SCROLLER, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    Am_Object v_scroller = group.Get_Object(Am_V_SCROLLER, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    // check if one of the scroll bars
    if (which_part != h_scroller && which_part != v_scroller)
    {
      // then transform based on offsets for the inside

      // adjust both the clip region and the object to be in my
      // parent's coordinate system
      int insideleft = group.Get(Am_CLIP_LEFT, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
      int insidetop = group.Get(Am_CLIP_TOP, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
      my_width = group.Get(Am_CLIP_WIDTH, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR); //just re-set the value directly
      my_height = group.Get(Am_CLIP_HEIGHT, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);

      my_left += insideleft;
      my_top += insidetop;

      //now offset the area by the x and y offsets
      left -= (int)group.Get(Am_X_OFFSET, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
      top -=  (int)group.Get(Am_Y_OFFSET, Am_NO_DEPENDENCY | Am_RETURN_ZERO_ON_ERROR);
    } // done with inside scrolling part

    // now clip incoming rectangle to my rectangle
    Am_Invalid_Rectangle_Intersect(left, top, width, height,
                                   my_left, my_top, my_width, my_height,
                                   final_left, final_top,
                                   final_width, final_height);
    if ((final_width > 0) && (final_height > 0))
      Am_Invalidate (owner, group, final_left, final_top, final_width,
                     final_height);
  }
}

//in the Am_Scrolling_Group itself to define how big a page jump is
Am_Define_Formula(int, h_scroll_jump_page)
{
  return (int)self.Get(Am_WIDTH) - 10;
}

//in the Am_Scrolling_Group itself to define how big a page jump is
Am_Define_Formula(int, v_scroll_jump_page)
{
  return (int)self.Get(Am_HEIGHT) - 10;
}

Am_Define_Formula(int, v_scroll_left)
{
  Am_Object group = self.Get_Owner();
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int scroll_border = (look == Am_MOTIF_LOOK) ? 2 : 0;
  if( (bool)group.Get( Am_V_SCROLL_BAR_ON_LEFT ) )
    return -scroll_border;
  else // on right
    return (int)group.Get(Am_WIDTH) - (int)self.Get(Am_WIDTH) + scroll_border;
}

Am_Define_Formula(int, v_scroll_top)
{
  Am_Object group = self.Get_Owner();
  int group_border = get_scroll_border_thickness(group);
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int scroll_border = (look == Am_MOTIF_LOOK) ? 2 : 0;
  int ret = (int)group.Get(Am_CLIP_TOP) - scroll_border - group_border;
  return ret;
}

Am_Define_Formula(int, v_scroll_height)
{
  Am_Object group = self.Get_Owner();
  int group_border = get_scroll_border_thickness(group);
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int scroll_border = (look == Am_MOTIF_LOOK) ? 2 : 0;
  int ret = (int)group.Get(Am_CLIP_HEIGHT) + 2*scroll_border + 2*group_border;
  return ret;
}

Am_Define_Formula(bool, v_scroll_visible)
{
  Am_Object group = self.Get_Owner();
  if ((bool)group.Get(Am_VISIBLE) && (bool)group.Get(Am_V_SCROLL_BAR))
    return true;
  else
    return false;
}

Am_Define_Formula(bool, v_scroll_active)
{
  Am_Object group = self.Get_Owner();
  if( (bool)group.Get(Am_ACTIVE) &&
      ( (int)group.Get(Am_INNER_HEIGHT) >= (int)group.Get(Am_CLIP_HEIGHT) ) )
    return true;
  else
  {
    if (group.Valid()) {
      group.Set( Am_Y_OFFSET, 0);  //make sure at origin when go inactive
      group.Get_Object(Am_V_SCROLLER, Am_NO_DEPENDENCY).Get_Object(Am_COMMAND, Am_NO_DEPENDENCY).Set( Am_VALUE, 0);
    }
    return false;
  }
}

Am_Define_Formula(int, v_scroll_value_2)
{
  Am_Object group = self.Get_Owner();
  int val = (int)group.Get(Am_INNER_HEIGHT) - (int)group.Get(Am_CLIP_HEIGHT);
  if (val < 1) val = 1;
  return val;
}

Am_Define_Formula(float, v_scroll_percent_visible)
{
  Am_Object group = self.Get_Owner();
  float innerh =  group.Get(Am_INNER_HEIGHT);
  if (innerh == 0.0f) innerh = 1.0f;
  float val = (float)group.Get(Am_CLIP_HEIGHT) / innerh;
  if (val > 1.0) val = 1.0f;
  return val;
}

//////// Horizontal scrollbar stuff

Am_Define_Formula(int, h_scroll_left)
{
  Am_Object group = self.Get_Owner();
  int group_border = get_scroll_border_thickness(group);
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int scroll_border = (look == Am_MOTIF_LOOK) ? 2 : 0;
  return (int)group.Get(Am_CLIP_LEFT) - scroll_border - group_border;
}

Am_Define_Formula(int, h_scroll_top)
{
  Am_Object group = self.Get_Owner();
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int scroll_border = (look == Am_MOTIF_LOOK) ? 2 : 0;
  if( (bool)group.Get( Am_H_SCROLL_BAR_ON_TOP ) )
    return -scroll_border;
  else // on bottom
    return (int)group.Get(Am_HEIGHT) - (int)self.Get(Am_HEIGHT) + scroll_border;
}

Am_Define_Formula(int, h_scroll_width)
{
  Am_Object group = self.Get_Owner();
  int group_border = get_scroll_border_thickness(group);
  Am_Widget_Look look = self.Get( Am_WIDGET_LOOK );
  int scroll_border = (look == Am_MOTIF_LOOK) ? 2 : 0;
  return (int)group.Get(Am_CLIP_WIDTH) + 2*scroll_border + 2*group_border;
}

Am_Define_Formula(bool, h_scroll_visible)
{
  Am_Object group = self.Get_Owner();
  if ((bool)group.Get(Am_VISIBLE) &&
      (bool)group.Get(Am_H_SCROLL_BAR)) return true;
  else return false;
}

Am_Define_Formula(bool, h_scroll_active) {
  Am_Object group = self.Get_Owner();
  if ((bool)group.Get(Am_ACTIVE) &&
      ((int)group.Get(Am_INNER_WIDTH) >=
       (int)group.Get(Am_CLIP_WIDTH)))
    return true;
  else {
    if (group.Valid()) {
      group.Set( Am_X_OFFSET, 0);  //make sure at origin when go inactive
      group.Get_Object(Am_H_SCROLLER, Am_NO_DEPENDENCY).Get_Object(Am_COMMAND, Am_NO_DEPENDENCY).Set( Am_VALUE, 0);
    }
    return false;
  }
}

Am_Define_Formula(int, h_scroll_value_2) {
  Am_Object group = self.Get_Owner();
  int val = (int)group.Get(Am_INNER_WIDTH) - (int)group.Get(Am_CLIP_WIDTH);
  if (val < 1) val = 1;
  return val;
}

Am_Define_Formula(float, h_scroll_percent_visible) {
  Am_Object group = self.Get_Owner();
  float innerw =  group.Get(Am_INNER_WIDTH);
  if (innerw == 0.0f) innerw = 1.0f;
  float val = (float)group.Get(Am_CLIP_WIDTH) / innerw;
  if (val > 1.0) val = 1.0f;
  return val;
}

//explicitly remove Am_V_SCROLLER and Am_H_SCROLLER from the
//graphical objects list so programmers iterating through the list
//won't be confused by them
void remove_scrollers(Am_Object scroll_group)
{
  Am_Value_List components;
  components = scroll_group.Get (Am_GRAPHICAL_PARTS);
  // probably would be OK to just set the list to empty, but this is safer
  components.Start();
  if(!components.Member(scroll_group.Get_Object(Am_V_SCROLLER)))
    Am_Error("Scroll group doesn't contain V_scroller");
  components.Delete();

  components.Start();
  if(!components.Member(scroll_group.Get_Object(Am_H_SCROLLER)))
    Am_Error("Scroll group doesn't contain H_scroller");
  components.Delete();
  scroll_group.Set( Am_GRAPHICAL_PARTS, components);
}

////////////////////////////////////////////////////////////////////////
// Scroll group do and undo
////////////////////////////////////////////////////////////////////////

void scroll_group_inter_abort_do(Am_Object command_obj, Am_Slot_Key slot)
{
  clear_save_pos_for_undo_proc(command_obj); //do the standard scrollbar stuff
  Am_Object inter = command_obj.Get_Owner();
  Am_Object scrollbar = inter.Get_Owner();
  Am_Object scroll_group = scrollbar.Get_Owner();
  Am_Object scroll_bar_command = scrollbar.Get_Object(Am_COMMAND);
  int val = scroll_bar_command.Get(Am_OLD_VALUE);
  scroll_group.Set(slot, val);
  // cout << "...Abort Setting Value of " << scrollbar << " to " << val
  //      << endl << flush;
  scrollbar.Set(Am_VALUE, val);
}

Am_Define_Method(Am_Object_Method, void, scroll_group_v_inter_abort_do,
                 (Am_Object command))
{
  scroll_group_inter_abort_do(command, Am_Y_OFFSET);
}

Am_Define_Method(Am_Object_Method, void, scroll_group_h_inter_abort_do,
                 (Am_Object command))
{
  scroll_group_inter_abort_do(command, Am_X_OFFSET);
}

//update the y_offset of the group
Am_Define_Method(Am_Object_Method, void, scroll_group_v_do,
                 (Am_Object command))
{
  Am_Object v_scroll = command.Get_Owner();
  Am_Object scroll_group = v_scroll.Get_Owner();
  int val = v_scroll.Get(Am_VALUE);
  scroll_group.Set( Am_Y_OFFSET, val);
}

//update the x_offset of the group
Am_Define_Method(Am_Object_Method, void, scroll_group_h_do,
                 (Am_Object command))
{
  Am_Object h_scroll = command.Get_Owner();
  Am_Object scroll_group = h_scroll.Get_Owner();
  int val = h_scroll.Get(Am_VALUE);
  scroll_group.Set( Am_X_OFFSET, val);
}

void scroll_group_general_undo_redo(Am_Object command_obj, bool undo,
                                    bool selective, Am_Slot_Key slot)
{
  Am_Value new_value, old_value;
  Am_Object scroll_bar, scroll_group;
  scroll_bar = command_obj.Get(Am_SAVED_OLD_OWNER);
  scroll_group = scroll_bar.Get_Owner();
  if (scroll_group.Valid()) {

    if (selective)  // then get current value from the scroll_bar
      new_value=scroll_bar.Peek(Am_VALUE);
    else // get current value from the command_obj
      new_value=command_obj.Peek(Am_VALUE);

    if (undo) old_value=command_obj.Peek(Am_OLD_VALUE);
    else  // repeat
      old_value=command_obj.Peek(Am_VALUE);

    command_obj.Set( Am_OLD_VALUE, new_value);
    command_obj.Set( Am_VALUE, old_value);
    //also set scroll group
    if (scroll_group.Valid()) {
      scroll_group.Set(slot, old_value);
    }
  }
}

Am_Define_Method(Am_Object_Method, void, scroll_group_v_undo,
                 (Am_Object command_obj))
{
  scroll_group_general_undo_redo(command_obj, true, false, Am_Y_OFFSET);
}

Am_Define_Method(Am_Object_Method, void, scroll_group_v_selective_undo,
                 (Am_Object command_obj))
{
  scroll_group_general_undo_redo(command_obj, true, true, Am_Y_OFFSET);
}

Am_Define_Method(Am_Object_Method, void, scroll_group_v_selective_repeat,
                 (Am_Object command_obj))
{
  scroll_group_general_undo_redo(command_obj, false, true, Am_Y_OFFSET);
}

Am_Define_Method(Am_Object_Method, void, scroll_group_h_undo,
                 (Am_Object command_obj))
{
  scroll_group_general_undo_redo(command_obj, true, false, Am_X_OFFSET);
}

Am_Define_Method(Am_Object_Method, void, scroll_group_h_selective_undo,
                 (Am_Object command_obj))
{
  scroll_group_general_undo_redo(command_obj, true, true, Am_X_OFFSET);
}

Am_Define_Method(Am_Object_Method, void, scroll_group_h_selective_repeat,
                 (Am_Object command_obj))
{
  scroll_group_general_undo_redo(command_obj, false, true, Am_X_OFFSET);
}

////////////////////////////////////////////////////////////////////////

//goes in scroll bar inter
Am_Define_Formula (Am_Value, set_v_scroller_value_from_y_offset)
{
  Am_Value value;
  Am_Object scroll_group = self.Get_Owner();
  value = scroll_group.Peek(Am_Y_OFFSET);
  self.Set (Am_VALUE, value, Am_DONT_TELL_ANIMATORS);
  return value;
}

//goes in scroll bar inter
Am_Define_Formula (Am_Value, set_h_scroller_value_from_x_offset)
{
  Am_Value value;
  Am_Object scroll_group = self.Get_Owner();
  value = scroll_group.Peek(Am_X_OFFSET);
  self.Set (Am_VALUE, value, Am_DONT_TELL_ANIMATORS);
  return value;
}

Am_Define_Formula (Am_Value, get_impl_parent_from_group)
{
  Am_Value value;
  //self is cmd, owner is scroll_bar, owner is scroll_group
  Am_Object group = self.Get_Owner().Get_Owner();
  if( group.Valid() )
    value = group.Peek(Am_COMMAND);
  else
    value = Am_NOT_USUALLY_UNDONE;
  return value;
}

////////////////////////////////////////////////////////////////////////

//exported objects
Am_Object Am_Vertical_Scroll_Bar = 0;
Am_Object Am_Horizontal_Scroll_Bar = 0;
Am_Object Am_Vertical_Up_Down_Counter;
Am_Object Am_Scrolling_Group;

// internal objects
Am_Object Am_Scroll_Arrow;
Am_Object Am_Scroll_Indicator;

void Am_Scroll_Widgets_Initialize()
{
  Am_Object inter;             // interactor in the widget
  Am_Object command_obj;
  Am_Object_Advanced obj_adv;  // to get at advanced features like
                               // local-only and demons.

  ///////////////////////////////////////////////////////////////////////////
  // Scroll Internal Command
  ///////////////////////////////////////////////////////////////////////////

  Am_Object Am_Scroll_Inter_Command = Am_Command.Create("Scroll_Inter_Command")
    .Set( Am_IMPLEMENTATION_PARENT, Am_Get_Owners_Command )
    .Set( Am_DO_METHOD, NULL )
    .Set( Am_UNDO_METHOD, Am_Widget_Inter_Command_Undo )
    .Set( Am_REDO_METHOD, Am_Widget_Inter_Command_Undo )
    .Set( Am_SELECTIVE_UNDO_METHOD, Am_Widget_Inter_Command_Selective_Undo )
    .Set( Am_SELECTIVE_REPEAT_SAME_METHOD,
                                  Am_Widget_Inter_Command_Selective_Repeat )
    .Set( Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL )
    ;

  ///////////////////////////////////////////////////////////////////////////
  // Scroll bars
  ///////////////////////////////////////////////////////////////////////////

  /****************************************************************************
   * Am_Scroll_Arrow
   */

  Am_Scroll_Arrow = Am_Graphical_Object.Create ("Am_Scroll_Arrow" )
     .Add( Am_INTERIM_SELECTED, false )
     .Add( Am_SCROLL_ARROW_DIRECTION, (int)Am_SCROLL_ARROW_UP )
     .Add( Am_WIDGET_LOOK, Am_From_Owner( Am_WIDGET_LOOK ) )
     .Add( Am_FILL_STYLE, Am_Amulet_Purple )
     .Add( Am_STYLE_RECORD, Am_Get_Computed_Colors_Record_Form )
     .Set( Am_DRAW_METHOD, scroll_arrow_draw )
     .Add( Am_ACTIVE, Am_From_Owner( Am_ACTIVE ) )
     ;

  obj_adv = (Am_Object_Advanced&)Am_Scroll_Arrow;
  obj_adv.Get_Slot( Am_SELECTED )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON );
  obj_adv.Get_Slot( Am_INTERIM_SELECTED )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON );
  obj_adv.Get_Slot( Am_FILL_STYLE )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON );
  obj_adv.Get_Slot( Am_SCROLL_ARROW_DIRECTION )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON );

  /****************************************************************************
   * Am_Scroll_Indicator
   */

  Am_Scroll_Indicator = Am_Graphical_Object.Create("Am_Scroll_Indicator")
     .Add( Am_INTERIM_SELECTED, false )
     .Add( Am_WIDGET_LOOK, Am_From_Owner( Am_WIDGET_LOOK ) )
     .Add( Am_FILL_STYLE, Am_Amulet_Purple )
     .Add( Am_STYLE_RECORD, Am_Get_Computed_Colors_Record_Form )
     .Add( Am_ACTIVE, Am_From_Owner( Am_ACTIVE ) )
        ;

  /****************************************************************************
   * Scroll_Bar
   */

  Am_Object inter1;
  Am_Object inter2;
  Am_Object inter3;

  Am_Object Scroll_Bar = Am_Widget_Group.Create("Scroll_Bar")
     .Add( Am_VALUE, 50 )
     .Set( Am_VALUE, Am_Animate_With 
	  (Am_Stepping_Animator.Create ("Scroll_Bar_Animator")
	   .Set (Am_START_DO_METHOD, scrollbar_timer_start)
	   .Set (Am_ANIMATION_METHOD, scrollbar_timer)
	   .Add (Am_FIRST_TICK, false)
	   .Add (Am_INTERACTOR, 0)
	   .Add (Am_INITIAL_DELAY, 0)
	   .Set (Am_ACTIVE, false)  // inactive by default
       ))
     .Set( Am_WIDTH, 20 )
     .Set( Am_HEIGHT, 200 )
     .Add( Am_KEY_SELECTED, false )
     .Add( Am_ACTIVE, Am_Active_From_Command )
     .Add( Am_ACTIVE_2, true ) // used by interactive tools
     .Add( Am_FILL_STYLE, Am_Amulet_Purple )
     .Add( Am_STYLE_RECORD, Am_Get_Computed_Colors_Record_Form )
     .Add( Am_VALUE_1, 0 ) //default type is int, but can be float
     .Add( Am_VALUE_2, 100 )
     .Add( Am_SMALL_INCREMENT, 1 )
     .Add( Am_INITIAL_DELAY, Am_Time(500))
     .Add( Am_REPEAT_DELAY, Am_Time(50))
     .Add( Am_LARGE_INCREMENT, 10 )
     .Add( Am_PERCENT_VISIBLE, 0.2 )
     .Set( Am_DRAW_METHOD, scroll_draw )
     .Add( Am_SCROLL_AREA_SIZE, 166 ) //temp, set by layout
     .Add( Am_SCROLL_AREA_MIN, 17 )   //temp, set by layout
     .Add( Am_WIDGET_START_METHOD, Am_Standard_Widget_Start_Method )
     .Add( Am_WIDGET_ABORT_METHOD, Am_Standard_Widget_Abort_Method )
     .Add( Am_WIDGET_STOP_METHOD, Am_Standard_Widget_Stop_Method )
     .Add_Part( Am_SCROLL_ARROW1, Am_Scroll_Arrow.Create("Scroll_Arrow1")
                .Set( Am_SCROLL_ARROW_DIRECTION, (int)Am_SCROLL_ARROW_UP )
                .Set( Am_FILL_STYLE, Am_From_Owner( Am_FILL_STYLE ) )
                .Set( Am_VISIBLE, Am_From_Owner( Am_VISIBLE ) )
                )
     .Add_Part( Am_SCROLL_ARROW2, Am_Scroll_Arrow.Create("Scroll_Arrow2")
                .Set( Am_SCROLL_ARROW_DIRECTION, (int)Am_SCROLL_ARROW_DOWN )
                .Set( Am_FILL_STYLE, Am_From_Owner (Am_FILL_STYLE) )
                .Set( Am_VISIBLE, Am_From_Owner (Am_VISIBLE) )
                )
     .Add_Part(Am_SCROLL_INDICATOR,
                Am_Scroll_Indicator.Create("Scroll_Indicator")
                .Set( Am_FILL_STYLE, Am_From_Owner (Am_FILL_STYLE) )
                .Set( Am_LEFT, 4 ) //some of these replaced with formulas below
                .Set( Am_TOP, 4 )
                .Set( Am_WIDTH, 12 )
                .Set( Am_HEIGHT, 12 )
                .Set( Am_VISIBLE, Am_From_Owner (Am_VISIBLE) )
                );
  //Visual C++ can't handle long expressions
  Scroll_Bar
     .Add_Part(Am_INTERACTOR, inter1 =
                   Am_Move_Grow_Interactor.Create("Scroll_Indicator_Inter")
                .Set( Am_START_WHEN, Am_Default_Widget_Start_Char )
                .Set( Am_START_WHERE_TEST, in_scroll_indicator )
                .Set( Am_ACTIVE, Am_Active_And_Active2 )
                .Set( Am_PRIORITY, 30.0 ) //so higher than the others
                .Set( Am_DO_METHOD, NULL ) //all work done by interim_do
                )
     .Add_Part(Am_ARROW_INTERACTOR,
                inter2 = Am_Choice_Interactor.Create("Scroll_Arrow_Inter")
                .Set( Am_START_WHEN, Am_Default_Widget_Start_Char )
                .Set( Am_START_WHERE_TEST, in_scroll_arrows )
	        .Set( Am_RUNNING_WHERE_TEST, never_go_outside )
                .Set( Am_ACTIVE, Am_Active_And_Active2 )
                .Set( Am_PRIORITY, 20.0 ) //so higher than the background inter
                )
     .Add_Part(Am_BACKGROUND_INTERACTOR,
                inter3 = Am_Choice_Interactor.Create("Scroll_Page_Inter")
               .Set( Am_START_WHEN, Am_Default_Widget_Start_Char )
               .Set( Am_ACTIVE, Am_Active_And_Active2 )
               .Set( Am_START_WHERE_TEST, Am_Inter_In )
               .Set( Am_PRIORITY, 10.0 ) // higher than normal
	       .Set( Am_RUNNING_WHERE_TEST, never_go_outside )
                )
     .Add_Part (Am_COMMAND, Am_Command.Create("Scroll_Command")
                .Set( Am_VALUE, 50 )
                .Set( Am_LABEL, "Scrollbar"))
     .Add( Am_SET_COMMAND_OLD_OWNER, Am_Set_Old_Owner_To_Me )
     ;

  // don't do any of the standard undo things
  inter1.Get_Object(Am_IMPLEMENTATION_COMMAND )
    .Set( Am_UNDO_METHOD, NULL )
    .Set( Am_REDO_METHOD, NULL )
    .Set( Am_SELECTIVE_UNDO_METHOD, NULL )
    .Set( Am_SELECTIVE_REPEAT_SAME_METHOD, NULL )
    .Set( Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL );
  // don't do any of the standard undo things
  inter2.Get_Object(Am_IMPLEMENTATION_COMMAND )
    .Set( Am_UNDO_METHOD, NULL )
    .Set( Am_REDO_METHOD, NULL )
    .Set( Am_SELECTIVE_UNDO_METHOD, NULL )
    .Set( Am_SELECTIVE_REPEAT_SAME_METHOD, NULL )
    .Set( Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL );
  // don't do any of the standard undo things
  inter3.Get_Object(Am_IMPLEMENTATION_COMMAND )
    .Set( Am_UNDO_METHOD, NULL )
    .Set( Am_REDO_METHOD, NULL )
    .Set( Am_SELECTIVE_UNDO_METHOD, NULL )
    .Set( Am_SELECTIVE_REPEAT_SAME_METHOD, NULL )
    .Set( Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL );

  inter1.Set_Part(Am_COMMAND,
          Am_Scroll_Inter_Command.Create("Command_In_Scroll_Indicator_Inter")
                  .Set( Am_START_DO_METHOD, save_pos_for_undo )
                  .Set( Am_DO_METHOD, clear_save_pos_for_undo )
                  .Set( Am_ABORT_DO_METHOD, abort_save_pos_for_undo )
                  );
  inter2.Set_Part(Am_COMMAND,
          Am_Scroll_Inter_Command.Create("Command_In_Scroll_Arrow_Inter")
                  .Set( Am_START_DO_METHOD, scroll_arrow_inter_command_start )
                  .Set( Am_ABORT_DO_METHOD, abort_save_pos_for_undo )
                  .Set( Am_DO_METHOD, scroll_arrow_inter_command_do )
                  );
  inter3.Set_Part(Am_COMMAND,
          Am_Scroll_Inter_Command.Create("Command_In_Scroll_Page_Inter")
                  .Set( Am_START_DO_METHOD, vertical_scroll_page_inter_command_start )
                  .Set( Am_ABORT_DO_METHOD, abort_save_pos_for_undo )
                  .Set( Am_DO_METHOD, scroll_arrow_inter_command_do )
                  );
  obj_adv = (Am_Object_Advanced&)Scroll_Bar;
  obj_adv.Get_Slot (Am_KEY_SELECTED )
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_ACTIVE )
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_WIDGET_LOOK )
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_FINAL_FEEDBACK_WANTED )
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_FONT )
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_FILL_STYLE )
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_VALUE_1 )
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_VALUE_2 )
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot (Am_PERCENT_VISIBLE )
    .Set_Demon_Bits (Am_STATIONARY_REDRAW | Am_EAGER_DEMON);

  // all the next slots should not be inherited
  obj_adv.Get_Slot( Am_ACTIVE ).Set_Inherit_Rule( Am_COPY );
  obj_adv.Get_Slot( Am_ACTIVE_2 ).Set_Inherit_Rule( Am_COPY );

  //obj_adv.Get_Slot(Am_VALUE).Set_Single_Constraint_Mode( false );

  /****************************************************************************
   * Am_Vertical_Scroll_Bar
   */

  Am_Vertical_Scroll_Bar = Scroll_Bar.Create("Vertical_Scroll_Bar")
    .Set( Am_WIDTH, scroll_narrow_side_size )
    .Add( Am_SCROLL_AREA_MAX, v_scroll_area_max )
    .Set( Am_LAYOUT, v_scroll_layout_formula )
    ;

  Am_Vertical_Scroll_Bar.Get_Object(Am_SCROLL_INDICATOR)
    .Set( Am_TOP, scroll_indicator_pos.Multi_Constraint() )
    .Set( Am_WIDTH, 12 ) //overridden by Am_V_Scroll_Layout_Formula
    .Set( Am_HEIGHT, scroll_indicator_size )
    .Set( Am_DRAW_METHOD, v_scroll_indicator_draw )
    ;

  Am_Vertical_Scroll_Bar.Get_Object(Am_INTERACTOR)
    .Set( Am_INTERIM_DO_METHOD, v_scroll_indicator_inter_interim_do );

  /****************************************************************************
   * Am_Vertical_Up_Down_Counter
   */
  //just the up and down arrows
  Am_Vertical_Up_Down_Counter =
    Am_Vertical_Scroll_Bar.Create("Am_Vertical_Up_Down_Counter")
    .Set(Am_HEIGHT, just_arrows_height);
  Am_Vertical_Up_Down_Counter.Remove_Part(Am_SCROLL_INDICATOR);
  Am_Vertical_Up_Down_Counter.Remove_Part(Am_INTERACTOR);
  Am_Vertical_Up_Down_Counter.Remove_Part(Am_BACKGROUND_INTERACTOR);
  

  /****************************************************************************
   * Am_Horizontal_Scroll_Bar
   */

  Am_Horizontal_Scroll_Bar = Scroll_Bar.Create("Horizontal_Scroll_Bar")
    .Set( Am_WIDTH, 200 )
    .Set( Am_HEIGHT, scroll_narrow_side_size )
    .Add( Am_SCROLL_AREA_MAX, h_scroll_area_max )
    .Set( Am_LAYOUT, h_scroll_layout_formula )
    ;

  Am_Horizontal_Scroll_Bar.Get_Object( Am_SCROLL_ARROW1 )
    .Set( Am_SCROLL_ARROW_DIRECTION, (int)Am_SCROLL_ARROW_LEFT );

  Am_Horizontal_Scroll_Bar.Get_Object( Am_SCROLL_ARROW2 )
    .Set( Am_SCROLL_ARROW_DIRECTION, (int)Am_SCROLL_ARROW_RIGHT );

  Am_Horizontal_Scroll_Bar.Get_Object( Am_SCROLL_INDICATOR )
    .Set_Name("H_Scroll_Indicator")
    .Set( Am_LEFT, scroll_indicator_pos.Multi_Constraint() )
    .Set( Am_WIDTH, scroll_indicator_size )
    .Set( Am_HEIGHT, 12 ) //overridden by Am_H_Scroll_Layout_Formula
    .Set( Am_DRAW_METHOD, h_scroll_indicator_draw )
    ;
  Am_Horizontal_Scroll_Bar.Get_Object( Am_BACKGROUND_INTERACTOR )
    .Get_Object( Am_COMMAND )
    .Set( Am_START_DO_METHOD, horizontal_scroll_page_inter_command_start );
  Am_Horizontal_Scroll_Bar.Get_Object( Am_INTERACTOR )
    .Set( Am_INTERIM_DO_METHOD, h_scroll_indicator_inter_interim_do );

  ///////////////////////////////////////////////////////////////////////////
  // Scrolling_Group
  ///////////////////////////////////////////////////////////////////////////
  Am_Object v_scroll, h_scroll;

  Am_Scrolling_Group = Am_Widget_Group.Create("Scrolling_Group")
     .Add( Am_X_OFFSET, 0 ) // x offset of the scrollable area
     .Add( Am_Y_OFFSET, 0 )
     .Set( Am_WIDTH, 150 )
     .Set( Am_HEIGHT, 150 )
     .Add( Am_ACTIVE, true )
     .Add( Am_ACTIVE_2, true ) // used by interactive tools
     .Add( Am_FILL_STYLE, Am_Amulet_Purple )
     .Add( Am_LINE_STYLE, Am_Black ) // border around scrolling area
     .Add( Am_INNER_FILL_STYLE, 0 ) // if not 0, then inside of window color
     .Add( Am_H_SCROLL_BAR, true )
     .Add( Am_V_SCROLL_BAR, true )
     .Add( Am_H_SCROLL_BAR_ON_TOP, false )
     .Add( Am_V_SCROLL_BAR_ON_LEFT, false )
     .Add( Am_H_SMALL_INCREMENT, 10 ) // in pixels
     .Add( Am_H_LARGE_INCREMENT, h_scroll_jump_page )
     .Add( Am_V_SMALL_INCREMENT, 10 ) // in pixels
     .Add( Am_V_LARGE_INCREMENT, v_scroll_jump_page )
     .Set( Am_DRAW_METHOD, scrolling_group_draw )

     .Set( Am_INVALID_METHOD, scrolling_group_invalid )
     .Set( Am_TRANSLATE_COORDINATES_METHOD, scroll_group_translate_coordinates )
     .Set( Am_POINT_IN_PART_METHOD, scroll_group_point_in_part )
     .Set( Am_POINT_IN_LEAF_METHOD, scroll_group_point_in_leaf )
     .Set( Am_POINT_IN_OBJ_METHOD,  scroll_group_point_in_obj )

     .Add( Am_INNER_WIDTH, 400 )   // ** USE THE FORMULA FOR WIDTH FROM GROUPS
     .Add( Am_INNER_HEIGHT, 400 )  // ** USE THE FORMULA FROM GROUPS
     .Add( Am_COMMAND, Am_NOT_USUALLY_UNDONE ) // whether undoable
     .Add( Am_SET_COMMAND_OLD_OWNER, Am_Set_Old_Owner_To_Me )

     // internal slots
     .Add( Am_CLIP_LEFT, scroll_clip_left )
     .Add( Am_CLIP_TOP, scroll_clip_top )
     .Add( Am_CLIP_WIDTH, scroll_clip_width )
     .Add( Am_CLIP_HEIGHT, scroll_clip_height )
     .Add_Part(Am_V_SCROLLER, v_scroll =
                Am_Vertical_Scroll_Bar.Create("V_Scroll_In_Group" )
                .Set( Am_VALUE, 0 )
		.Add( Am_Y1, set_v_scroller_value_from_y_offset )
                .Set( Am_LEFT, v_scroll_left )
                .Set( Am_TOP, v_scroll_top )
                .Set( Am_HEIGHT, v_scroll_height )
                .Set( Am_VISIBLE, v_scroll_visible )
                .Set( Am_ACTIVE, v_scroll_active )
                .Set( Am_ACTIVE_2, Am_From_Owner (Am_ACTIVE_2) )
                .Set( Am_WIDGET_LOOK, Am_From_Owner (Am_WIDGET_LOOK) )
                .Set( Am_FILL_STYLE, Am_From_Owner (Am_FILL_STYLE) )
                .Set( Am_VALUE_1, 0 )
                .Set( Am_VALUE_2, v_scroll_value_2 )
                .Set( Am_SMALL_INCREMENT, Am_From_Owner (Am_V_SMALL_INCREMENT) )
                .Set( Am_LARGE_INCREMENT, Am_From_Owner (Am_V_LARGE_INCREMENT) )
                .Set( Am_PERCENT_VISIBLE, v_scroll_percent_visible )
                );
  // Visual C++ can't handle long expressions
  Am_Scrolling_Group
     .Add_Part(Am_H_SCROLLER, h_scroll =
                Am_Horizontal_Scroll_Bar.Create("H_Scroll_In_Group" )
                .Set( Am_VALUE, 0 )
		.Add( Am_X1, set_h_scroller_value_from_x_offset )
                .Set( Am_LEFT, h_scroll_left )
                .Set( Am_TOP, h_scroll_top )
                .Set( Am_WIDTH, h_scroll_width )
                .Set( Am_VISIBLE, h_scroll_visible )
                .Set( Am_ACTIVE, h_scroll_active )
                .Set( Am_ACTIVE_2, Am_From_Owner (Am_ACTIVE_2) )
                .Set( Am_WIDGET_LOOK, Am_From_Owner (Am_WIDGET_LOOK) )
                .Set( Am_FILL_STYLE, Am_From_Owner (Am_FILL_STYLE) )
                .Set( Am_VALUE_1, 0 )
                .Set( Am_VALUE_2, h_scroll_value_2 )
                .Set( Am_SMALL_INCREMENT, Am_From_Owner(Am_H_SMALL_INCREMENT) )
                .Set( Am_LARGE_INCREMENT, Am_From_Owner(Am_H_LARGE_INCREMENT) )
                .Set( Am_PERCENT_VISIBLE, h_scroll_percent_visible )

                 )
     ;
  obj_adv = (Am_Object_Advanced&)Am_Scrolling_Group;
  obj_adv.Get_Slot( Am_LINE_STYLE )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot( Am_X_OFFSET )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot( Am_Y_OFFSET )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot( Am_H_SCROLL_BAR )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot( Am_V_SCROLL_BAR )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot( Am_H_SCROLL_BAR_ON_TOP )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot( Am_V_SCROLL_BAR_ON_LEFT )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON);
  obj_adv.Get_Slot( Am_INNER_FILL_STYLE )
    .Set_Demon_Bits( Am_STATIONARY_REDRAW | Am_EAGER_DEMON);

  //explicitly remove Am_V_SCROLLER and Am_H_SCROLLER from the
  //graphical objects list so programmers iterating through the list
  //won't be confused by them
  remove_scrollers(Am_Scrolling_Group);

  v_scroll.Get_Object(Am_COMMAND )
    .Set( Am_LABEL, "Scroll Vertical" )
    .Set( Am_DO_METHOD, scroll_group_v_do )
    .Set( Am_UNDO_METHOD, scroll_group_v_undo )
    .Set( Am_REDO_METHOD, scroll_group_v_undo )
    .Set( Am_SELECTIVE_UNDO_METHOD, scroll_group_v_selective_undo )
    .Set( Am_SELECTIVE_REPEAT_SAME_METHOD, scroll_group_v_selective_repeat )
    .Set( Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL )
    //not undoable by default
    .Set( Am_IMPLEMENTATION_PARENT, get_impl_parent_from_group )
    .Set( Am_IMPLEMENTATION_CHILD, Am_DONT_UNDO_CHILDREN )
    ;
  v_scroll.Get_Object(Am_INTERACTOR).Get_Object(Am_COMMAND )
    .Set( Am_ABORT_DO_METHOD, scroll_group_v_inter_abort_do);

  h_scroll.Get_Object(Am_COMMAND )
    .Set( Am_LABEL, "Scroll Horizontal" )
    .Set( Am_DO_METHOD, scroll_group_h_do )
    .Set( Am_IMPLEMENTATION_PARENT, get_impl_parent_from_group )
    .Set( Am_UNDO_METHOD, scroll_group_h_undo )
    .Set( Am_REDO_METHOD, scroll_group_h_undo )
    .Set( Am_SELECTIVE_UNDO_METHOD, scroll_group_h_selective_undo )
    .Set( Am_SELECTIVE_REPEAT_SAME_METHOD, scroll_group_h_selective_repeat )
    .Set( Am_SELECTIVE_REPEAT_ON_NEW_METHOD, NULL )
    .Set( Am_IMPLEMENTATION_CHILD, Am_DONT_UNDO_CHILDREN )
    ;
  h_scroll.Get_Object(Am_INTERACTOR).Get_Object(Am_COMMAND )
    .Set( Am_ABORT_DO_METHOD, scroll_group_h_inter_abort_do );
}
