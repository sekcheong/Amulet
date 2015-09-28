#ifndef AMULET2_H
#define AMULET2_H

#ifdef AMULET2_INSTRUMENT
//
// Automatically converting Sets to Adds
//

// last recorded filename and line number
extern const char *Am_Filename;
extern int Am_Line_Number;

#define Am_Instrumented(method)   Record_FileLine (__FILE__, __LINE__).method

#define Am_Instrumentation(type) \
  type& Record_FileLine (const char *f, int l) \
  { Am_Filename = f; Am_Line_Number = l; return *this; }

#endif // AMULET2_INSTRUMENT

#endif // AMULET2_H
