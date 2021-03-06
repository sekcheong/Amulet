/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifdef GCC
#include <string.h>
#include <stdlib.h>
#else
extern "C" {
#include <string.h>
#include <stdlib.h>
}
#endif
#include "dynarray.h"

#ifdef AMULET2_INSTRUMENT
#undef Set
#undef Add
#endif

#ifndef NULL
#define NULL (void *)0
#endif

#ifdef NEED_MEMMOVE
/*
void memmove (char* dest, char* src, int length)
{
  register char *i, *j;
  
  if ((unsigned)dest < (unsigned)src)
    for (i = dest, j = src; length; ++i, ++j, --length)
      *i = *j;
  else
    for (i = dest + length - 1, j = src + length - 1; length;
	 --i, --j, --length)
      *i = *j;
}
*/
void memmove (char* dest, const char* src, int length)
{
  char *data = new char[length];

  memcpy (data, src, length);
  memcpy (dest, data, length);
  delete[] data;
}
#endif

DynArray::DynArray (unsigned in_elem_size)
{
  elem_size = in_elem_size;
  data_size = 1;
  length = 0;
  data = (char*)malloc (in_elem_size);
}

char* DynArray::Get (unsigned i)
{
  if (i < length)
    return &data[i*elem_size];
  return NULL;
}

void DynArray::Set (unsigned i, char* value)
{
  if (i < length)
    memcpy (&data[i*elem_size], value, elem_size);
}

void DynArray::Insert (unsigned i, char* value)
{
  if (i <= length) {
    if (length == data_size) {
      char* new_data;

      data_size *= 2;
      new_data = (char*)malloc (data_size*elem_size);
      memcpy (new_data, data, i*elem_size);
      memcpy (&new_data[(i+1)*elem_size], &data[i*elem_size],
              (length-i)*elem_size);
      free (data);
      data = new_data;
    } else {
#ifdef __SUNPRO_CC
      memmove ((void *) &data[(i+1)*elem_size], (void *) &data[i*elem_size],
               (length-i)*elem_size);
#else
      memmove (&data[(i+1)*elem_size], &data[i*elem_size],
               (length-i)*elem_size);
#endif
    }
    memcpy (&data[i*elem_size], value, elem_size);
    length++;
  }
}

void DynArray::Delete (unsigned i)
{
  if (i < length) {
    length--;
    if (length < (unsigned)(data_size / 3)) {
      char* new_data;

      data_size /= 2;
      new_data = (char*)malloc (data_size*elem_size);
      memcpy (new_data, data, i*elem_size);
      memcpy (&new_data[i*elem_size], &data[(i+1)*elem_size],
              (length-i)*elem_size);
      free (data);
      data = new_data;
    } else {
#ifdef __SUNPRO_CC
      memmove ((void *) &data[i*elem_size], (void *) &data[(i+1)*elem_size],
               (length-i)*elem_size);
#else
      memmove (&data[i*elem_size], &data[(i+1)*elem_size],
               (length-i)*elem_size);
#endif
    }
  }
}

void DynArray::Add (char* value)
{
  if (length == data_size) {
    char* new_data;

    data_size *= 2;
    new_data = (char*)malloc (data_size*elem_size);
      memcpy (new_data, data, length*elem_size);
    free (data);
    data = new_data;
  }
  memcpy (&data[length*elem_size], value, elem_size);
  length++;
}

DynArray* DynArray::Copy ()
{
  DynArray* new_array = new DynArray (elem_size);
  char* new_data;

  new_array->length = length;
  new_array->data_size = data_size;
  new_data = (char*)malloc (data_size*elem_size);
  free (new_array->data);
  memcpy (new_data, data, length*elem_size);
  new_array->data = new_data;

  return new_array;
}

void DynArray::Destroy ()
{
  if (data)
    free (data);
  data = NULL;
}
