/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact garnet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/* This file contains the implementation of the X version of Am_Region
   
   Designed and implemented by Andrew Mickish
*/

extern "C" {
}

#include <am_inc.h>

#include GEM__H
#include GEMX__H

// // // // // // // // // // // // // // // // // // // // // // // // // //
//
// Am_Region creator and destructor functions
//
// // // // // // // // // // // // // // // // // // // // // // // // // //

Am_Region* Am_Region::Create () {
  Am_Region_Impl* the_region = new Am_Region_Impl ();
  return the_region;
}

Am_Region_Impl::Am_Region_Impl () {
  size = 5;
  index = 0;
  max_index = size - 1;

  x_rgns    = new Region[size];
  for(int i=0;i<size;i++)
    x_rgns[i] = NULL;
}

void Am_Region_Impl::Destroy () {
  index = 0;
  if (x_rgns[0])
    XDestroyRegion(x_rgns[0]);
  x_rgns[index] = NULL;
  delete [] x_rgns;
  delete this;
}

// // // // // // // // // // // // // // // // // // // // // // // // // //
//
// Am_Region manipulator functions
//
// // // // // // // // // // // // // // // // // // // // // // // // // //

bool Am_Region_Impl::all_rgns_used () {
  return (index == max_index);
}

void Am_Region_Impl::add_more_rgns() {
  size += 5;
  max_index += 5;
  Region *new_rgns = new Region[size];
  memcpy(new_rgns, x_rgns, sizeof(Region) * (size - 5));
  for(int i=size-5;i<size;i++)
    new_rgns[i] = NULL;
  delete [] x_rgns;
  x_rgns = new_rgns;
}

void Am_Region_Impl::Clear () {
  index = 0;
  if (x_rgns[index])
    XDestroyRegion(x_rgns[index]);
  x_rgns[index] = NULL;
}

     
void Am_Region_Impl::Set (int the_left, int the_top,
			  unsigned int the_width,
			  unsigned int the_height) {
  static XRectangle x_rect;
  x_rect.x = the_left;
  x_rect.y = the_top;
  x_rect.width = the_width;
  x_rect.height = the_height;

  Clear ();
  x_rgns[0] = XCreateRegion ();
  XUnionRectWithRegion(&x_rect, x_rgns[0], x_rgns[0]);
}

// You can call this function with an uninitialized Am_Region (i.e., you
// don't have to call Set_Region first.
//
void Am_Region_Impl::Push (Am_Region *the_region) {
  // Since we want to install a copy of the_region anyway, pick it apart
  // and send the pieces to the rect version of this function.
  static XRectangle x_rect;
  XClipBox(((Am_Region_Impl *)the_region)->region_to_use(), &x_rect);
  Push(x_rect.x, x_rect.y, x_rect.width, x_rect.height);
}

// You can call this function with an uninitialized Am_Region (i.e., you
// don't have to call Set_Region first.
//
void Am_Region_Impl::Push (int the_left, int the_top,
			   unsigned int the_width,
			   unsigned int the_height) {
  if (all_rgns_used())
    add_more_rgns();

  // If this is an uninitialized Am_Region, then we want the index to stay 0
  // so we can initialize the first element in the x_rects array.  If this is
  // an old Am_Region, then initialize the next element.
  if (x_rgns[0])
    index = index + 1;

  static XRectangle x_rect;
  x_rect.x = the_left;
  x_rect.y = the_top;
  x_rect.width = the_width;
  x_rect.height = the_height;

  if (x_rgns[index])
    XDestroyRegion(x_rgns[index]);
  x_rgns[index] = XCreateRegion();
  XUnionRectWithRegion(&x_rect, x_rgns[index], x_rgns[index]);

  // If this region has already had rects pushed onto it, then intersect
  // this new region with all the ones that have come before it (it is
  // sufficient to intersect it with just the previous region).
  if (index)
    XIntersectRegion(x_rgns[index-1], x_rgns[index], x_rgns[index]);

}

void Am_Region_Impl::Pop () {
  if (index) {
    XDestroyRegion(x_rgns[index]);
    x_rgns[index] = NULL;
    index--;
  }
  else
    // index == 0, so designate this an "uninitialized" region
    Clear ();
}

// Unions are performed on the most recently modified region.  That is, if
// a sequence of pushes have generated a particualr region, the union
// operation will be performed on the region resulting from the last push.
//
// You can call this function with an uninitialized Am_Region (i.e., you
// don't have to call Set_Region first.
//
void Am_Region_Impl::Union (int the_left, int the_top,
			    unsigned int the_width,
			    unsigned int the_height) {

  static XRectangle x_rect;
  x_rect.x = the_left;
  x_rect.y = the_top;
  x_rect.width = the_width;
  x_rect.height = the_height;

  if (!(x_rgns[index]))
    x_rgns[index] = XCreateRegion();
  XUnionRectWithRegion(&x_rect, x_rgns[index], x_rgns[index]);
}

void Am_Region_Impl::Intersect (int the_left, int the_top,
			        unsigned int the_width,
			        unsigned int the_height) {
  Region x_region = XCreateRegion();
  static XRectangle x_rect;
  x_rect.x = the_left;
  x_rect.y = the_top;
  x_rect.width = the_width;
  x_rect.height = the_height;
  XUnionRectWithRegion(&x_rect, x_region, x_region);

  if (!(x_rgns[index]))
    x_rgns[index] = XCreateRegion();
  XIntersectRegion(x_region, x_rgns[index], x_rgns[index]);
  XDestroyRegion(x_region);
}

// Returns true if the point is inside the region.  A point exactly on the
// boundary of the region is considered inside the region.
bool Am_Region_Impl::In (int x, int y) {
  return XPointInRegion(x_rgns[index], x, y);
}

// Returns true if the rectangle is completely inside or intersects the region.
// The total parameter is set to true if the rectangle is completely inside
// the region.
bool Am_Region_Impl::In (int x, int y, unsigned int width,
		         unsigned int height, bool& total) {
  int x_result = XRectInRegion (x_rgns[index], x, y, width, height);

  if (x_result == RectangleIn)
    total = true;
  else
    total = false;

  if (x_result == RectangleOut)
    return false;
  else
    return true;
}

// Returns true if the rectangle is completely inside or intersects the region.
// The total parameter is set to true if the rectangle is completely inside
// the region.
bool Am_Region_Impl::In (Am_Region *rgn, bool& total) {
  static XRectangle x_rect;
  XClipBox (((Am_Region_Impl *)rgn)->region_to_use(), &x_rect);
  return In (x_rect.x, x_rect.y, x_rect.width, x_rect.height, total);
}
