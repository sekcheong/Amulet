/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#include <am_inc.h>

#include AM_IO__H
#include GEM__H

#if defined(__MWERKS__)	|| defined (_WINDOWS)
#define TESTCLIP_D1_LEFT 400
#else
#define TESTCLIP_D1_LEFT 800
#endif

// To avoid type conversion warnings in ObjectCenter, you have to
// declare .3 and .8 to be float instead of double
const float purple_red = 0.5f;
const float purple_green = 0.3f;
const float purple_blue = 0.8f;

Am_Drawonable *d1, *d2;

Am_Style red (1.0f, 0.0f, 0.0f);
Am_Style green (0.0f, 1.0f, 0.0f);
Am_Style blue (0.0f, 0.0f, 1.0f);
Am_Style white (1.0f, 1.0f, 1.0f);
Am_Style black (0.0f, 0.0f, 0.0f);
Am_Style yellow (1.0f, 1.0f, 0.0f);
Am_Style purple (purple_red, purple_green, purple_blue);

Am_Style style_array[8];

// Utility function for reusing the style_array for iteration when
// creating lines with different line styles
//
void set_style_array (Am_Style arg0 = Am_No_Style, Am_Style arg1 = Am_No_Style,
		      Am_Style arg2 = Am_No_Style, Am_Style arg3 = Am_No_Style,
		      Am_Style arg4 = Am_No_Style, Am_Style arg5 = Am_No_Style,
		      Am_Style arg6 = Am_No_Style, Am_Style arg7 = Am_No_Style)
{
  style_array[0] = arg0;
  style_array[1] = arg1;
  style_array[2] = arg2;
  style_array[3] = arg3;
  style_array[4] = arg4;
  style_array[5] = arg5;
  style_array[6] = arg6;
  style_array[7] = arg7;
}

void clear_whole_window (Am_Drawonable *d) {
  int w, h;
  d->Get_Size (w, h);
  d->Clear_Area (0, 0, w, h);
}

void draw_4_circles(Am_Drawonable *d) {
  int x_counter, y_counter, x, y;

  set_style_array (black, red, black);
  for (x_counter=0; x_counter<=1; x_counter++) {
    for (y_counter=0; y_counter<=1; y_counter++) {
      x = 10+(75*x_counter);
      y = 10+(75*y_counter);
      d->Draw_Arc(Am_No_Style, style_array[x_counter + y_counter], x, y,
		  50, 50);
    }
  }
}

// Based on example for handling Expose events in XLIB Volume 1, p. 78.
//
// Note: "All the Expose events generated by a single action are guaranteed
// to be contiguous in the event queue."  That means that if we see an
// expose event with count 0, and it happens to be in window2, we are
// guaranteed that we are not interrupting a sequence of expose events
// from window1.  We can go ahead and install the clip mask whenever we see
// a count of 0.
void perceive_expose_event (Am_Drawonable *d, int count, int clip_left,
			    int clip_top, unsigned short clip_width,
			    unsigned short clip_height)
{
  // Since the_region is static, it will not be automatically reinitialized
  // each time the function is invoked.  It is only cleared at the end of
  // this function when count is 0.  This is why you can call this function
  // multiple times with a non-zero count to enlarge the clip region, and
  // then finally install it in the GC when count is 0.
  static Am_Region *the_region = Am_Region::Create();

  the_region->Union(clip_left, clip_top, clip_width, clip_height);

  /* If this is the last expose in a contiguous group,
     set the clip region, then clear it for next time. */
  if (count == 0) {
    d->Set_Clip(the_region);
    the_region->Destroy();
    the_region = Am_Region::Create();
  }
}

void test_single_clips() {

  ///
  ///  Draw into D1
  ///
  d1->Set_Clip(30, 30, 85, 85);
  draw_4_circles(d1);
  d1->Flush_Output();

  printf("Hit RETURN to clip giant green rect to small area:");
  getchar();

  d1->Set_Clip(75, 75, 100, 100);
  d1->Draw_Rectangle(Am_No_Style, green, 0, 0, 300, 200);
  d1->Flush_Output();

  ///
  /// Draw into D2
  ///
  d2->Set_Clip(30, 30, 85, 85);
  draw_4_circles(d2);
  d2->Set_Clip(75, 75, 100, 100);
  d2->Draw_Rectangle(Am_No_Style, green, 0, 0, 300, 200);
  d2->Flush_Output();
}

// This function tests the Union operation on Am_Regions, and simulates how
// the Expose event-hander might be implemented in Opal, based on how
// X Windows sends Exposure events.
void test_multi_clips () {
  printf("Entering test_multi_clips:\n");
  printf("  Hit RETURN to clip giant red and blue rects to multi clip rgns: ");
  getchar();

  d1->Clear_Clip();
  clear_whole_window (d1);
  perceive_expose_event(d1, 2, 20, 20, 40, 40);
  perceive_expose_event(d1, 1, 50, 50, 60, 40);
  perceive_expose_event(d1, 0, 120, 80, 40, 40);
  d1->Draw_Rectangle(Am_No_Style, red, 0, 0, 300, 200);

  perceive_expose_event(d1, 1, 200, 20, 80, 40);
  perceive_expose_event(d1, 0, 220, 80, 40, 40);
  d1->Draw_Rectangle(Am_No_Style, blue, 0, 0, 300, 200);
  d1->Flush_Output();

  // In the next set of clips and draws, you should expect the clip regions
  // in the two windows to remain distinct.  That is, drawing will only
  // occur on the left side of d1, and will only occur on the right side
  // of d2.  Note: you cannot mix non-zero expose event counts across
  // multiple windows.  "All the Expose events generated by a single action
  // are guaranteed to be contiguous in the event queue."
  printf("  Hit RETURN to set clip regions in multiple windows: ");
  getchar();
  d1->Clear_Clip();
  clear_whole_window (d1);
  d2->Clear_Clip();
  clear_whole_window (d2);

  perceive_expose_event(d1, 1, 30, 20, 60, 60);
  perceive_expose_event(d1, 0, 10, 100, 100, 50);
  // Since count was zero for d1's last exposure event, you must draw to
  // that window right now (before another exposure event comes in for that
  // window).  This will be programmed into the Exposure event handler.
  d1->Draw_Rectangle(Am_No_Style, purple, 0, 0, 300, 200);

  perceive_expose_event(d2, 1, 200, 20, 60, 60);
  perceive_expose_event(d2, 0, 180, 100, 100, 50);
  d2->Draw_Rectangle(Am_No_Style, green,  0, 0, 300, 200);
  d1->Flush_Output();
  d2->Flush_Output();
}

void test_region_points (Am_Region *rgn, int x, int y, bool val) {
  if (!(rgn->In(x, y) == val)) {
    char error[200];
    sprintf(error, "** test_region_points failed with  x = %d,  y = %d, "
	    " val = %d.\n", x, y, val);
    Am_Error (error);
  }
}

void test_region_rect (Am_Region *rgn, int x, int y,
		       unsigned int width, unsigned int height,
		       bool val1, bool val2) {
  bool total;
  if ( (!(rgn->In(x, y, width, height, total) == val1)) ||
       (!(total == val2))) {
    char error[200];
    sprintf(error, "** test_region_rect failed with x = %d, y = %d, "
	    "width = %d, height = %d\n  val1 = %d,  val2 = %d.\n", x, y,
	    width, height, val1, val2);
    Am_Error (error);
  }
}

void test_region_rgn (Am_Region *rgn1, Am_Region *rgn2,
		       bool val1, bool val2) {
  bool total;
  if ( (!(rgn1->In(rgn2, total) == val1)) ||
       (!(total == val2))) {
    char error[200];
    sprintf(error, "** test_region_rgn failed with rgn1 = %x, rgn2 = %x.\n",
	    (unsigned int) rgn1, (unsigned int) rgn2);
    Am_Error (error);
  }
}

void test_intersections () {
  printf("Testing Am_Region::Intersect_Region:\n");

  printf("  Hit RETURN to draw three giant rectangles in successively\n");
  printf("smaller regions of D2:");
  getchar();

  Am_Region *rgn = Am_Region::Create();
  rgn->Set(0, 0, 300, 200);
  d2->Set_Clip(rgn);
  d2->Draw_Rectangle(Am_No_Style, red, 0, 0, 300, 200);
  rgn->Intersect(0, 0, 200, 200);
  d2->Set_Clip(rgn);
  d2->Draw_Rectangle(Am_No_Style, white, 0, 0, 300, 200);
  rgn->Intersect(0, 0, 100, 200);
  d2->Set_Clip(rgn);
  d2->Draw_Rectangle(Am_No_Style, blue, 0, 0, 300, 200);
  d2->Flush_Output();

  d2->Clear_Clip();
  Am_Region *test_region = Am_Region::Create();
  test_region->Set(0, 0, 100, 200);
  Am_Region *inside_region = Am_Region::Create();
  inside_region->Set(25, 10, 20, 20);
  d2->Draw_Rectangle(Am_No_Style, green, 25, 20, 20, 20);
  Am_Region *overlap_region = Am_Region::Create();
  overlap_region->Set(50, 60, 100, 20);
  d2->Draw_Rectangle(Am_No_Style, green, 50, 60, 100, 20);
  Am_Region *outside_region = Am_Region::Create();
  outside_region->Set(125, 100, 20, 20);
  d2->Draw_Rectangle(Am_No_Style, green, 125, 100, 20, 20);
  d2->Flush_Output();

  test_region_points(test_region, 0, 0, true);
  test_region_points(test_region, 200, 100, false);
  test_region_rect(test_region, 25, 10, 20, 20, true, true);
  test_region_rect(test_region, 50, 60, 100, 20, true, false);
  test_region_rect(test_region, 125, 100, 20, 20, false, false);
  test_region_rgn(test_region, test_region, true, true);
  test_region_rgn(test_region, inside_region, true, true);
  test_region_rgn(test_region, overlap_region, true, false);
  test_region_rgn(test_region, outside_region, false, false);
  printf("Am_Region::In_Region tests all passed.\n");
}

void test_region_fns () {
  printf("Testing low-level Am_Region member functions:\n");

  printf("  Hit RETURN to clip giant black rect to an Am_Region:");
  getchar();

  d1->Clear_Clip();
  clear_whole_window (d1);;

  Am_Region *rgn = Am_Region::Create();
  rgn->Set(150, 50, 80, 40);
  d1->Set_Clip(rgn);
  d1->Draw_Rectangle(Am_No_Style, black, 0, 0, 300, 200);
  d1->Flush_Output();
  rgn->Destroy();

  int counter;
  printf("  Hit RETURN 6 times to nest Am_Region clipping:  ");
  set_style_array(red, green, blue, white, black, yellow);
  clear_whole_window (d1);;

  rgn = Am_Region::Create();
  for(counter=0; counter<=5; counter++) {
    getchar();
    printf("%d  ", counter + 1);
    rgn->Push(10+10*counter, 10+10*counter, 150, 150);
    d1->Set_Clip(rgn);
    d1->Draw_Rectangle(Am_No_Style, style_array[counter], 0, 0, 300, 200);
    d1->Flush_Output();
  }
  printf("\n");

  printf("  Hit RETURN 6 times to pop Am_Region clipping:  ");
  for(counter=0; counter<=5; counter++) {
    getchar();
    printf("%d  ", counter + 1);
    rgn->Pop();
    d1->Set_Clip(rgn);
    d1->Draw_Rectangle(Am_No_Style, purple, 0, 0, 300, 200);
    d1->Flush_Output();
  }
  printf("\n");

  printf("  Hit RETURN 2 times to push again onto that same Am_Region:  ");
  for(counter=0; counter<=1; counter++) {
    getchar();
    printf("%d  ", counter + 1);
    rgn->Push(50+10*counter, 50+10*counter, 200, 50);
    d1->Set_Clip(rgn);
    d1->Draw_Rectangle(Am_No_Style, style_array[counter], 0, 0, 300, 200);
    d1->Flush_Output();
  }
  printf("\n");
}

void test_drawonable_fns () {
  printf("Testing higher-level Am_Drawonable member functions:\n");

  printf("  Hit RETURN to clip black rect to drawonable's clip_region\n");
  getchar();

  d1->Clear_Clip();
  clear_whole_window (d1);;
  d1->Set_Clip(150, 50, 80, 40);
  d1->Draw_Rectangle(Am_No_Style, black, 0, 0, 300, 200);
  d1->Flush_Output();

  int counter;
  printf("  Hit RETURN 6 times to nest Am_Drawonable clipping:  ");
  set_style_array(red, green, blue, white, black, yellow);
  d1->Clear_Clip();
  clear_whole_window (d1);;

  for(counter=0; counter<=5; counter++) {
    getchar();
    printf("%d  ", counter + 1);
    d1->Push_Clip(10+10*counter, 10+10*counter, 150, 150);
    d1->Draw_Rectangle(Am_No_Style, style_array[counter], 0, 0, 300, 200);
    d1->Flush_Output();
  }
  printf("\n");

  printf("  Hit RETURN 6 times to pop Am_Drawonable clipping:  ");
  for(counter=0; counter<=5; counter++) {
    getchar();
    printf("%d  ", counter + 1);
    d1->Pop_Clip();
    d1->Draw_Rectangle(Am_No_Style, purple, 0, 0, 300, 200);
    d1->Flush_Output();
  }
  printf("\n");

  printf("  Hit RETURN 2 times to push again into that same Am_Drawonable:  ");
  for(counter=0; counter<=1; counter++) {
    getchar();
    printf("%d  ", counter + 1);
    d1->Push_Clip(50+10*counter, 50+10*counter, 200, 50);
    d1->Draw_Rectangle(Am_No_Style, style_array[counter], 0, 0, 300, 200);
    d1->Flush_Output();
  }
  printf("\n");
}

void test_clear_clipping () {
  printf("Testing clip-regions with Clear_Area():\n");

  printf("  Hit RETURN to black-out drawonable and clear an area:");
  getchar();

  d1->Clear_Clip();
  d1->Draw_Rectangle(Am_No_Style, black, 0, 0, 300, 200);
  d1->Set_Clip(10, 10, 280, 180);
  clear_whole_window (d1);;
  d1->Flush_Output();

  int counter;
  printf("  Hit RETURN 6 times to nest Am_Drawonable clipping,\n");
  printf("      while alternately drawing big rectangles and clearing:  ");
  set_style_array(red, red, blue, blue, red, red);

  for(counter=0; counter<=5; counter++) {
    getchar();
    printf("%d  ", counter + 1);
    d1->Push_Clip(0, 0, 290-20*counter, 190-20*counter);
    if ( counter % 2 )
      clear_whole_window (d1);
    else
      d1->Draw_Rectangle(Am_No_Style, style_array[counter], 0, 0, 300, 200);
    d1->Flush_Output();
  }
  printf("\n");
}

void test_more_clear_clipping () {
  printf("Testing clip-regions that do/don't intersect Clear_Area():\n");

  
  printf("  Hit RETURN to black-out window:");
  getchar();
  d1->Clear_Clip();
  d1->Draw_Rectangle(Am_No_Style, black, 0, 0, 300, 200);
  d1->Flush_Output();

  
  printf("  Hit RETURN to clip on left and clear on right, w/o intersecting:");
  getchar();
  d1->Clear_Clip();
  d1->Draw_Rectangle(Am_No_Style, black, 0, 0, 300, 200);
  d1->Set_Clip(10, 10, 80, 80);
  d1->Clear_Area(210, 110, 80, 80);
  d1->Flush_Output();

  printf("  Hit RETURN to clip on left and clear on right, with intersection:");
  getchar();
  d1->Clear_Clip();
  d1->Draw_Rectangle(Am_No_Style, green, 0, 0, 300, 200);
  d1->Set_Clip(80, 40, 80, 80);
  d1->Clear_Area(120, 80, 80, 80);
  d1->Flush_Output();

  printf("  Hit RETURN to clear on left and clip on right, with intersection:");
  getchar();
  d1->Clear_Clip();
  d1->Draw_Rectangle(Am_No_Style, blue, 0, 0, 300, 200);
  d1->Set_Clip(120, 80, 80, 80);
  d1->Clear_Area(80, 40, 80, 80);
  d1->Flush_Output();

  printf("  Hit RETURN to clear on left and clip on right, w/o intersecting:");
  getchar();
  // Put the clip-region on the right, and clear on the left, w/o intersecting
  d1->Clear_Clip();
  d1->Draw_Rectangle(Am_No_Style, red, 0, 0, 300, 200);
  d1->Set_Clip(10, 10, 80, 80);
  d1->Clear_Area(210, 110, 80, 80);
  d1->Flush_Output();
}

int main ()
{ 
  Am_Drawonable *root = Am_Drawonable::Get_Root_Drawonable();

  d1 = root->Create(TESTCLIP_D1_LEFT, 10, 300, 200);
  d1->Flush_Output ();

  d2 = root->Create(TESTCLIP_D1_LEFT, 250, 300, 200);
  d2->Flush_Output();

  test_single_clips();
  test_multi_clips();
  test_intersections();
  test_region_fns();
  test_drawonable_fns();
  test_clear_clipping();
  test_more_clear_clipping();
  
  ///
  ///  Exit
  ///
  printf("Hit RETURN to exit:");
  getchar();

  return 0;
}