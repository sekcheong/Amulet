/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <am_inc.h>
#include <time.h>
#include TYPES__H
#include GEM__H

#include "gemW_time.h"

Am_WRAPPER_IMPL(Am_Time);

Am_Time::Am_Time ()
{
  data = new Am_Time_Data;
}

Am_Time::Am_Time (unsigned long milliseconds)
{
  data = new Am_Time_Data;
  data->time = milliseconds;
}

Am_Time Am_Time::Now()
{
  Am_Time t((unsigned long) timeGetTime());
  return t;
}

bool Am_Time::operator> (const Am_Time& other) const
{
  return data->time > other.data->time;
}

bool Am_Time::operator< (const Am_Time& other) const
{
  return data->time < other.data->time;
}

bool Am_Time::operator>= (const Am_Time& other) const
{
  return data->time >= other.data->time;
}

bool Am_Time::operator<= (const Am_Time& other) const
{
  return data->time <= other.data->time;
}

Am_Time Am_Time::operator+ (unsigned long milliseconds) const
{
  Am_Time rt;
  
  rt.data->time = data->time + milliseconds;
  return rt;
}

Am_Time Am_Time::operator- (unsigned long milliseconds) const
{
  Am_Time rt;
  
  rt.data->time = data->time - milliseconds;
  return rt;
}

Am_Time Am_Time::operator+ (const Am_Time& other) const
{
  Am_Time rt;

  rt.data->time = data->time + other.data->time;
  return rt;
}

Am_Time Am_Time::operator- (const Am_Time& other) const
{
  Am_Time rt;

  rt.data->time = data->time - other.data->time;
  return rt;
}

void Am_Time::operator+= (const Am_Time& other)
{
  data->time += other.data->time;
}

void Am_Time::operator-= (const Am_Time& other)
{
  data->time -= other.data->time;
}

void Am_Time::operator+= (unsigned long milliseconds)
{
  data->time += milliseconds;
}

void Am_Time::operator-= (unsigned long milliseconds)
{
  data->time -= milliseconds;
}

unsigned long Am_Time::Milliseconds () const  // return # of milliseconds
{
  return data->time;
}

bool Am_Time::Zero() const
{
  return (data->time == 0);
}

bool Am_Time::Is_Future () const
{
  return (*this > Now());
}

bool Am_Time::Is_Past () const
{
  return (*this < Now());
}

Am_WRAPPER_DATA_IMPL(Am_Time, (this));

void Am_Time_Data::Print(ostream& os) const
{
  os << time <<" milliseconds";
}

ostream& operator<< (ostream& os, const Am_Time& time) {
  time.Print(os);
  return os;
}

//returns the current time and date as a string, like
//  "Fri Jan 17 16:03:55 EST 1997\n".
Am_String Am_Get_Time_And_Date() {
  Am_String str;
  time_t time_ptr;
  (void) time(&time_ptr);
  
  char *s = ctime(&time_ptr);
  str = s;
  return str;
}
