/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

extern "C" {
#include <stdlib.h>  // For abort ()
}

#if defined(GCC)
#include <string.h>
#else
extern "C" {
#ifdef _MSC_VER
#include <string.h>
#else
#include <strings.h>
#endif
}
#endif

#include <am_inc.h>

#include AM_IO__H

#include TYPES__H
#include REGISTRY__H
#include STDVALUE__H
#include MISC__H
#include UNIV_MAP__H
#include STR_STREAM__H

//for printout of values as a string for Am_Get_Name(Am_Value v)
#include STR_STREAM__H

void Am_Null_Method_Error_Proc(...) {
  Am_Error("** Invalid Method (with procedure ptr = 0) called.");
}

// Am_Null_Method_Error_Function is defined to 0 in nondebug mode,
// so we can't do this.
#ifdef DEBUG
Am_Any_Procedure* Am_Null_Method_Error_Function =
	&Am_Null_Method_Error_Proc;
#endif

void (*Am_Debugger) (void) = Am_Break_Into_Debugger;

#ifdef AMULET2_INSTRUMENT
// last recorded filename and line number
const char *Am_Filename;
int Am_Line_Number;
#endif

void Am_Error (const char* error_string)
{
  cerr << "** Amulet Error: " << error_string << endl;
#ifdef AMULET2_INSTRUMENT
  if (Am_Filename != 0)
    cerr << "** probably at " << Am_Filename << ":" << Am_Line_Number << endl;
#endif

#ifdef DEBUG
  if (Am_Debugger)
    Am_Debugger ();
#endif  
  cerr << "**  Program aborted." << endl;
#if defined(_WINDOWS) || defined(_MACINTOSH)
  cerr << "(press return to exit)" << endl;
  getchar();
#endif
  abort ();
}

void Am_Error ()
{
#ifdef DEBUG
  if (Am_Debugger)
    Am_Debugger ();
#endif  

  cerr << "**  Program aborted." << endl;
#if defined(_WINDOWS) || defined(_MACINTOSH)
  cerr << "(press return to exit)" << endl;
  getchar();
#endif
  abort ();
}

void Am_Standard_Print(ostream& out, unsigned long val, Am_ID_Tag type) {
#ifdef DEBUG
  out << "(" << Am_Get_Type_Name(type) << ") " << hex << val << dec;
#else
  out << "(TypeID=" << type << ") " << hex << val << dec;
#endif  
}

#ifdef DEBUG
Am_ID_Tag Am_Get_Unique_ID_Tag (const char* type_name, Am_ID_Tag in_class)
#else
Am_ID_Tag Am_Get_Unique_ID_Tag (const char* /*type_name*/, Am_ID_Tag in_class)
#endif
{
#define _FIRST_TAG 500
  static Am_ID_Tag current_tag = _FIRST_TAG;
  Am_ID_Tag tag = current_tag | in_class;
  if (Am_Type_Base (in_class))
    tag = in_class;
  else
    current_tag++;
  if (current_tag < _FIRST_TAG) // this should never happen.
    Am_Error ("*** Am_Get_Unique_ID_Tag: overflow!  Too many tags!");
#ifdef DEBUG
  if (type_name) Am_Register_Type_Name(tag, type_name);
#endif
  return tag;
}

Am_ID_Tag Am_Get_Unique_ID_Tag (const char* type_name,
				Am_Type_Support* support,
				Am_ID_Tag in_class)
{
  Am_ID_Tag tag = Am_Get_Unique_ID_Tag (type_name, in_class);
  Am_Register_Support (tag, support);
  return tag;
}

#ifdef DEBUG
Am_ID_Tag Am_Set_ID_Class (const char* type_name, Am_ID_Tag tag) {
  Am_Register_Type_Name (tag, type_name);
  return tag;
}
#else
Am_ID_Tag Am_Set_ID_Class (const char* /* type_name */, Am_ID_Tag tag) {
  return tag;
}
#endif

/*
void Am_Registered_Type::Print() const {
  Print (cout);
  cout << flush;
}
void Am_Registered_Type::Print_cout() const {
  Print (cout);
  cout << flush;
}
*/

const char * Am_Registered_Type::To_String () const {
#ifdef DEBUG
  const char *name = Am_Get_Name_Of_Item(this);
  return name;
#else
  return NULL;
#endif
}

void Am_Registered_Type::Print(ostream& out) const {
#ifdef DEBUG
  const char *name = Am_Get_Name_Of_Item(this);
  if (name) {
    out << name;
    return;
  }
#endif
  //standard print if no name or not debugging
  Am_Standard_Print(out, (unsigned long)this, ID());
}

//the default looks up the name, and if found, returns it.  Otherwise,
//returns Am_No_Object.  Ignores the item it is called on.
#ifdef DEBUG
Am_Value Am_Wrapper::From_String(const char * string) const {
  Am_Wrapper* item = (Am_Wrapper*)Am_Get_Named_Item (string);
  if (item) {
    item->Note_Reference();
    Am_Value v(item);
    return v;
  }
  return Am_No_Value;
}
#else
Am_Value Am_Wrapper::From_String(const char * /* string */) const {
  return Am_No_Value;
}
#endif

//the default looks up the name, and if found, returns it.  Otherwise,
//returns Am_No_Object.  Ignores the item it is called on.
#ifdef DEBUG
Am_Value Am_Method_Wrapper::From_String(const char * string) const {
  Am_Method_Wrapper* item = (Am_Method_Wrapper*)Am_Get_Named_Item (string);
  if (item) {
    Am_Value v(item);
    return v;
  }
  return Am_No_Value;
}
#else
Am_Value Am_Method_Wrapper::From_String(const char* /* string */) const {
  return Am_No_Value;
}
#endif

void Am_Method_Wrapper::Print(ostream& os) const {
#ifdef DEBUG
  const char *name = Am_Get_Name_Of_Item(this);
  if(name) {
    os << name;
    return;
  }
#endif
  Am_Standard_Print(os, (unsigned long)this->Call, ID());
}

static Am_Value_Type Simple    = Am_Set_ID_Class ("Simple", Am_SIMPLE_TYPE);
static Am_Value_Type EVT     = Am_Set_ID_Class ("error", Am_ERROR_VALUE_TYPE);
static Am_Value_Type WRAPPER = Am_Set_ID_Class ("wrapper",  Am_WRAPPER);
static Am_Value_Type METHOD  = Am_Set_ID_Class ("method",   Am_METHOD);
static Am_Value_Type FENUM   = Am_Set_ID_Class ("enum",     Am_ENUM);

static Am_Value_Type NONE    = Am_Set_ID_Class ("NO VALUE", 0x0000);
static Am_Value_Type ZERO    = Am_Set_ID_Class ("ZERO", Am_ZERO);
static Am_Value_Type INT     = Am_Set_ID_Class ("int",    Am_INT);
static Am_Value_Type LONG    = Am_Set_ID_Class ("long",   Am_LONG);
static Am_Value_Type BOOL    = Am_Set_ID_Class ("bool",   Am_BOOL);
static Am_Value_Type FLOAT   = Am_Set_ID_Class ("float",  Am_FLOAT);
static Am_Value_Type DOUB    = Am_Set_ID_Class ("double", Am_DOUBLE);
static Am_Value_Type STRIN   = Am_Set_ID_Class ("string", Am_STRING);
static Am_Value_Type CHAR    = Am_Set_ID_Class ("char",   Am_CHAR);
static Am_Value_Type VOID    = Am_Set_ID_Class ("void*",  Am_VOIDPTR);
static Am_Value_Type PROC    = Am_Set_ID_Class ("Am_Generic_Procedure*",
						Am_PROC);
static Am_Value_Type INVFORM = Am_Set_ID_Class ("Invalid_Formula_Error",
						Am_FORMULA_INVALID);
static Am_Value_Type MISSSLOT = Am_Set_ID_Class ("Missing_Slot_Error",
						 Am_MISSING_SLOT);
static Am_Value_Type GETONNULL = Am_Set_ID_Class ("Get_On_Null_Object_Error",
						  Am_GET_ON_NULL_OBJECT);

static Am_Value_Type TYPETYPET = Am_Set_ID_Class ("Am_Value_Type",
						  Am_TYPE_TYPE);
static Am_Value_Type SLOTKEYTYPET = Am_Set_ID_Class ("Am_Slot_Key",
						     Am_SLOT_KEY_TYPE);
  
void Am_Print_Type (ostream& os, Am_Value_Type type)
{
#ifdef DEBUG
  os << Am_Get_Type_Name (type);
#else
  os << type;
#endif  
}

/////////////////////////////////////////////////////////////////
// The Am_String procedures
/////////////////////////////////////////////////////////////////

Am_WRAPPER_IMPL (Am_String);

Am_String Am_No_String;

Am_String::Am_String (const char* string, bool copy)
{
  if (string)
    data = new Am_String_Data (string, copy);
  else data = NULL;
}

Am_String& Am_String::operator= (const char* string)
{
  if (data)
    data->Release ();
  if (string)
    data = new Am_String_Data (string);
  else data = NULL;
  return *this;
}

Am_String::operator const char* () const
{
  if (data)
    return *data;
  else
    return NULL;
}

Am_String::operator char* ()
{
  if (data) {
    data = (Am_String_Data*)data->Make_Unique ();
    return *data;
  }
  else
    return NULL;
}

bool Am_String::operator== (const Am_String& test_string) const
{
  if (data && test_string.data) {
    const char* string = *data;
    return (string == (const char*)test_string) ||
           !strcmp (string, (const char*)test_string);
  }
  else
    return data == test_string.data;
}

bool Am_String::operator== (const char* test_string) const
{
  if (data) {
    const char* string = *data;
    return (string == test_string) ||
        !strcmp (string, test_string);
  }
  else
    return test_string == NULL;
}

ostream& operator<< (ostream& os, const Am_String& string)
{
  if (string.Valid ())
    os << (const char*) string;
  return os;
}

/////////////////////////////////////////////////////////////////
// The Am_Value type procedures
/////////////////////////////////////////////////////////////////

Am_Value Am_No_Value;
Am_Value Am_Zero_Value(0, Am_ZERO);
        
Am_Value::Am_Value (Am_Wrapper* initial)
{
  value.wrapper_value = initial;
  if (initial)
    type = (Am_Value_Type)initial->ID ();
  else
    type = Am_WRAPPER_TYPE;
}

Am_Value::Am_Value (double initial)
{
  type = Am_DOUBLE;
  value.wrapper_value = new Am_Double_Data (initial);
}

Am_Value::Am_Value (const char* initial)
{
  type = Am_STRING;
  if (initial)
    value.wrapper_value = new Am_String_Data (initial);
  else
    value.wrapper_value = NULL;
}

Am_Value::Am_Value (const Am_String& initial)
{
  type = Am_STRING;
  value.wrapper_value = initial;
}

Am_Value::Am_Value (Am_Method_Wrapper* initial)
{
  value.method_value = initial;
  if (initial)
    type = (Am_Value_Type)initial->ID ();
  else
    type = Am_METHOD_TYPE;
}

Am_Value::Am_Value (const Am_Value& initial)
{
  type = initial.type;
  value = initial.value;
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Note_Reference ();
}

Am_Value::~Am_Value ()
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
}

static void type_error (const char* type_name, const Am_Value &value)
{
  cerr << "** Stored value of Am_Value is not of " << type_name
       << " type." << endl;
  cerr << "** It contains a value of type ";
  Am_Print_Type (cerr, value.type);
  cerr << "." << endl;
  Am_Error ();
}

Am_Value::operator Am_Wrapper* () const
{
  switch (Am_Type_Class (type)) {
  case Am_WRAPPER:
    if (value.wrapper_value)
      value.wrapper_value->Note_Reference ();
    return value.wrapper_value;
  case Am_SIMPLE_TYPE:
    switch (type) {
    case Am_STRING:
    case Am_DOUBLE:
      if (value.wrapper_value)
        value.wrapper_value->Note_Reference ();
      return value.wrapper_value;
    case Am_ZERO:
      return NULL;
    case Am_INT:
    case Am_LONG:
    case Am_BOOL:
    case Am_VOIDPTR:
    case Am_PROC:
      if (!value.voidptr_value)
        return NULL;
    }
  default:
    type_error ("Am_Wrapper*", *this);
    return NULL;
  }
}

Am_Value::operator Am_Ptr () const
{
  return (Am_Ptr)value.voidptr_value;
}

Am_Value::operator int () const
{
  switch (type) {
  case Am_INT:
  case Am_LONG:
    return (int)value.long_value; //use compiler's conversion
  case Am_BOOL:
    if (value.bool_value) return 1;
    else return 0;
  case Am_ZERO:
    return 0;
  default:
    type_error ("int", *this);
    return 0;
  }
}

Am_Value::operator long () const
{
  switch (type) {
  case Am_LONG:
  case Am_INT:
    return value.long_value;
  case Am_FLOAT:
    return (long)value.float_value;
  case Am_DOUBLE:
    return (long)(double)*(Am_Double_Data*)value.wrapper_value;
  case Am_BOOL:
    if (value.bool_value) return 1L;
    else return 0L;
  case Am_ZERO:
    return 0;
  default:
    type_error ("long", *this);
    return 0L;
  }
}

#ifndef NEED_BOOL
Am_Value::operator bool () const
{
  if (Am_Type_Class (type) == Am_ERROR_VALUE_TYPE ||
      type == Am_NONE) {
    type_error("bool", *this);
  }
  switch (type) {
  case Am_INT:
  case Am_LONG:
    return !!value.long_value;
  case Am_BOOL:
    return value.bool_value;
  case Am_FLOAT:
    return !!value.float_value;
  case Am_DOUBLE:
    return !!(double)*(Am_Double_Data*)value.wrapper_value;
  case Am_CHAR:
    return !!value.char_value;
  case Am_STRING:
    return !!(const char*)*(Am_String_Data*)value.wrapper_value;
  case Am_VOIDPTR:
    return !!value.voidptr_value;
  case Am_PROC:
    return !!value.proc_value;
  case Am_ZERO:
    return false;
  default:
    return !!value.voidptr_value;
  }
}
#endif

Am_Value::operator float () const
{
  switch (type) {
  case Am_FLOAT:
    return value.float_value;
  case Am_DOUBLE:
    return *(Am_Double_Data*)value.wrapper_value;
  case Am_INT:
  case Am_LONG:
    return (float)value.long_value;
  case Am_ZERO:
    return 0.0f;
  default:
    type_error ("float", *this);
    return 0.0f;
  }
}

Am_Value::operator double () const
{
  switch (type) {
  case Am_FLOAT:
    return value.float_value;
  case Am_DOUBLE:
    return *(Am_Double_Data*)value.wrapper_value;
  case Am_INT:
  case Am_LONG:
    return value.long_value;
  case Am_ZERO:
    return 0.0;
  default:
    type_error ("double", *this);
    return 0.0;
  }
}

Am_Value::operator char () const
{
  switch (type) {
  case Am_CHAR:
    return value.char_value;
  case Am_ZERO:
    return '\0';
  default:
    type_error ("char", *this);
  }
  return 0; // should never get here, but need to return something

}

Am_Value::operator Am_Generic_Procedure* () const
{
  if (Am_Type_Class (type) == Am_METHOD) {
    if (value.method_value)
      type_error ("Am_Generic_Procedure*", *this);
    return NULL;
  }
  switch (type) {
  case Am_PROC:
    return value.proc_value;
  case Am_ZERO:
    return NULL;
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    if (!value.voidptr_value)
      return NULL;
  default:
    type_error ("Am_Generic_Procedure*", *this);
    return NULL;
  }
}

Am_Value::operator Am_Method_Wrapper* () const
{
  if (Am_Type_Class (type) == Am_METHOD)
    return value.method_value;
  switch (type) {
  case Am_ZERO:
    return NULL;
  case Am_PROC:
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    if (!value.voidptr_value)
      return NULL;
  default:
    type_error ("Am_Method_Wrapper*", *this);
    return NULL;
  }
}

bool Am_Value::operator== (Am_Wrapper* test_value) const
{
  if (Am_Type_Class (type) == Am_WRAPPER)
    return (value.wrapper_value == test_value) ||
      (test_value && value.wrapper_value &&
       (*value.wrapper_value == *test_value));
  switch (type) {
  case Am_STRING:
  case Am_DOUBLE:
    return (value.wrapper_value == test_value) ||
      (test_value && value.wrapper_value &&
       (*value.wrapper_value == *test_value));
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    if (!value.voidptr_value)
      return test_value == NULL;
  default:
    return false;
  }
}

bool Am_Value::operator== (Am_Ptr test_value) const
{
  return value.voidptr_value == test_value;
}

bool Am_Value::operator== (int test_value) const
{
  switch (type) {
  case Am_INT:
  case Am_LONG:
    return value.long_value == test_value;
  case Am_FLOAT:
    return value.float_value == test_value;
  case Am_DOUBLE:
    return (double)*(Am_Double_Data*)value.wrapper_value == test_value;
  default:
    return false;
  }
}

bool Am_Value::operator== (long test_value) const
{
  switch (type) {
  case Am_LONG:
  case Am_INT:
    return value.long_value == test_value;
  case Am_FLOAT:
    return value.float_value == test_value;
  case Am_DOUBLE:
    return (double)*(Am_Double_Data*)value.wrapper_value == test_value;
  default:
    return false;
  }
}

#ifndef NEED_BOOL
bool Am_Value::operator== (bool test_value) const
{
  if (Am_Type_Class (type) != Am_NONE)
    return !!value.voidptr_value == test_value;
  switch (type) {
  case Am_INT:
  case Am_LONG:
    return !!value.long_value == test_value;
  case Am_BOOL:
    return value.bool_value == test_value;
  case Am_FLOAT:
    return !!value.float_value == test_value;
  case Am_DOUBLE:
    return !!(double)*(Am_Double_Data*)value.wrapper_value == test_value;
  case Am_CHAR:
    return !!value.char_value == test_value;
  case Am_STRING:
    return !!(const char*)*(Am_String_Data*)value.wrapper_value == test_value;
  case Am_VOIDPTR:
    return !!value.voidptr_value == test_value;
  case Am_PROC:
    return !!value.proc_value == test_value;
  default:
    return false;
  }
}
#endif

bool Am_Value::operator== (float test_value) const
{
  switch (type) {
  case Am_FLOAT:
    return value.float_value == test_value;
  case Am_DOUBLE:
    return (double)*(Am_Double_Data*)value.wrapper_value == test_value;
  case Am_INT:
  case Am_LONG:
    return value.long_value == test_value;
  default:
    return false;
  }
}

bool Am_Value::operator== (double test_value) const
{
  switch (type) {
  case Am_FLOAT:
    return value.float_value == test_value;
  case Am_DOUBLE:
    return (double)*(Am_Double_Data*)value.wrapper_value == test_value;
  case Am_INT:
  case Am_LONG:
    return value.long_value == test_value;
  default:
    return false;
  }
}

bool Am_Value::operator== (char test_value) const
{
  if (type == Am_CHAR)
    return value.char_value == test_value;
  else
    return false;
}

bool Am_Value::operator== (const char* test_value) const
{
  switch (type) {
  case Am_STRING:
    return *(Am_String_Data*)value.wrapper_value == test_value;
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    return !value.voidptr_value && !test_value;
  default:
    return false;
  }
}

bool Am_Value::operator== (const Am_String& test_value) const
{
  switch (type) {
  case Am_STRING:
    return *(Am_String_Data*)value.wrapper_value == test_value;
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    return !value.voidptr_value && !(const char*)test_value;
  default:
    return false;
  }
}

bool Am_Value::operator== (Am_Generic_Procedure* test_value) const
{
  if (Am_Type_Class (type) == Am_METHOD)
    return !value.method_value && !test_value;
  switch (type) {
  case Am_PROC:
    return value.proc_value == test_value;
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    return !value.voidptr_value && !test_value;
  default:
    return false;
  }
}

bool Am_Value::operator== (Am_Method_Wrapper* test_value) const
{
  if (Am_Type_Class (type) == Am_METHOD)
    return value.method_value == test_value;
  switch (type) {
  case Am_PROC:
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    return !value.voidptr_value && !test_value;
  default:
    return false;
  }
}

bool Am_Value::operator== (const Am_Value& test_value) const
{
  switch (Am_Type_Class (type)) {
  case Am_SIMPLE_TYPE:
    switch (type) {
    case Am_STRING:
    case Am_DOUBLE:
      return test_value == value.wrapper_value;
    case Am_VOIDPTR:
      return test_value == value.voidptr_value;
    case Am_INT:
    case Am_LONG:
      return test_value == value.long_value;
    case Am_BOOL:
      return test_value == value.bool_value;
    case Am_FLOAT:
      return test_value == value.float_value;
    case Am_CHAR:
      return test_value == value.char_value;
    case Am_PROC:
     return test_value == value.proc_value;
    case Am_NONE:
      return test_value.type == Am_NONE;
    case Am_ZERO:
      return test_value.type == Am_ZERO;
    default:
      return false;
    }
  case Am_WRAPPER:
    return test_value == value.wrapper_value;
  case Am_METHOD:
    return test_value == value.method_value;
  case Am_ENUM:
    return test_value.value.long_value == value.long_value;
  case Am_ERROR_VALUE_TYPE:
    return test_value.type == type;
  }
  return false; // should never get here, but need to return something
}

bool Am_Value::operator!= (Am_Wrapper* test_value) const
{
  if (Am_Type_Class (type) == Am_WRAPPER)
    return (value.wrapper_value != test_value) &&
      (!test_value || !value.wrapper_value ||
       !(*value.wrapper_value == *test_value));
  switch (type) {
  case Am_STRING:
  case Am_DOUBLE:
    return (value.wrapper_value != test_value) &&
      (!test_value || !value.wrapper_value ||
       !(*value.wrapper_value == *test_value));
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    if (!value.voidptr_value)
      return test_value != NULL;
  default:
    return true;
  }
}

bool Am_Value::operator!= (Am_Ptr test_value) const
{
  return value.voidptr_value != test_value;
}

bool Am_Value::operator!= (int test_value) const
{
  switch (type) {
  case Am_INT:
  case Am_LONG:
    return value.long_value != test_value;
  case Am_FLOAT:
    return value.float_value != test_value;
  case Am_DOUBLE:
    return (double)*(Am_Double_Data*)value.wrapper_value != test_value;
  default:
    return true;
  }
}

bool Am_Value::operator!= (long test_value) const
{
  switch (type) {
  case Am_LONG:
  case Am_INT:
    return value.long_value != test_value;
  case Am_FLOAT:
    return value.float_value != test_value;
  case Am_DOUBLE:
    return (double)*(Am_Double_Data*)value.wrapper_value != test_value;
  default:
    return true;
  }
}

#ifndef NEED_BOOL
bool Am_Value::operator!= (bool test_value) const
{
  if (Am_Type_Class (type) != Am_NONE)
    return !!value.voidptr_value != test_value;
  switch (type) {
  case Am_INT:
  case Am_LONG:
    return !!value.long_value != test_value;
  case Am_BOOL:
    return value.bool_value != test_value;
  case Am_FLOAT:
    return !!value.float_value != test_value;
  case Am_DOUBLE:
    return !!(double)*(Am_Double_Data*)value.wrapper_value != test_value;
  case Am_CHAR:
    return !!value.char_value != test_value;
  case Am_STRING:
    return !!(const char*)*(Am_String_Data*)value.wrapper_value != test_value;
  case Am_VOIDPTR:
    return !!value.voidptr_value != test_value;
  case Am_PROC:
    return !!value.proc_value != test_value;
  default:
    return true;
  }
}
#endif

bool Am_Value::operator!= (float test_value) const
{
  switch (type) {
  case Am_FLOAT:
    return value.float_value != test_value;
  case Am_DOUBLE:
    return (double)*(Am_Double_Data*)value.wrapper_value != test_value;
  case Am_INT:
  case Am_LONG:
    return value.long_value != test_value;
  default:
    return true;
  }
}

bool Am_Value::operator!= (double test_value) const
{
  switch (type) {
  case Am_FLOAT:
    return value.float_value != test_value;
  case Am_DOUBLE:
    return (double)*(Am_Double_Data*)value.wrapper_value != test_value;
  case Am_INT:
  case Am_LONG:
    return value.long_value != test_value;
  default:
    return true;
  }
}

bool Am_Value::operator!= (char test_value) const
{
  if (type == Am_CHAR)
    return value.char_value != test_value;
  else
    return true;
}

bool Am_Value::operator!= (const char* test_value) const
{
  switch (type) {
  case Am_STRING:
    return !(*(Am_String_Data*)value.wrapper_value == test_value);
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    if (!value.voidptr_value)
      return test_value != NULL;
  default:
    return true;
  }
}

bool Am_Value::operator!= (const Am_String& test_value) const
{
  switch (type) {
  case Am_STRING:
    return !(*(Am_String_Data*)value.wrapper_value == test_value);
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    return value.voidptr_value || (const char*)test_value;
  default:
    return true;
  }
}

bool Am_Value::operator!= (Am_Generic_Procedure* test_value) const
{
  if (Am_Type_Class (type) == Am_METHOD)
    return value.voidptr_value || test_value;
  switch (type) {
  case Am_PROC:
    return value.proc_value != test_value;
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    return value.voidptr_value || test_value;
  default:
    return true;
  }
}

bool Am_Value::operator!= (Am_Method_Wrapper* test_value) const
{
  if (Am_Type_Class (type) == Am_METHOD)
    return value.method_value != test_value;
  switch (type) {
  case Am_PROC:
  case Am_INT:
  case Am_LONG:
  case Am_VOIDPTR:
    return value.voidptr_value || test_value;
  default:
    return true;
  }
}

bool Am_Value::operator!= (const Am_Value& test_value) const
{
  return (type != test_value.type) ||
    ((Am_Type_Class (type) != Am_ERROR_VALUE_TYPE) && 
     ((Am_Type_Is_Ref_Counted (type) &&
       value.wrapper_value != test_value.value.wrapper_value &&
       value.wrapper_value && test_value.value.wrapper_value &&
       !(*value.wrapper_value == *test_value.value.wrapper_value)) ||
       (value.voidptr_value != test_value.value.voidptr_value)));
}

bool Am_Value::Valid () const
{
  switch (Am_Type_Class (type)) {
  case Am_ERROR_VALUE_TYPE: 
    return false;
  case Am_WRAPPER:
  case Am_METHOD:
  case Am_ENUM: 
    return value.voidptr_value != NULL;
  case Am_SIMPLE_TYPE:
    switch (type) {
    case Am_STRING:
    case Am_VOIDPTR:
      return value.voidptr_value != NULL;
    case Am_INT:
    case Am_LONG:
      return value.long_value != 0;
    case Am_BOOL:
      return value.bool_value != false;
    case Am_FLOAT:
      return value.float_value != 0.0;
    case Am_DOUBLE:
      return *(Am_Double_Data*)value.wrapper_value != 0.0;
    case Am_CHAR:
      return value.char_value != 0;
    case Am_PROC:
      return value.proc_value != NULL;
    default:
      return false;
    }
  default:
    return false;
  }
}

//returns true for any value, and zero value
//returns false for none and the error values
bool Am_Value::Exists () const
{
  switch (Am_Type_Class (type)) {
  case Am_ERROR_VALUE_TYPE: return false;
  case Am_WRAPPER:
  case Am_ENUM: return true;
  case Am_SIMPLE_TYPE:
    switch (type) {
    case Am_NONE:
      return false;
    default:
      return true;
    }
  default:
    return true;
  }
}

//returns true for any value, zero, and none
//returns false only for the error values
bool Am_Value::Safe () const
{
  return (Am_Type_Class (type) != Am_ERROR_VALUE_TYPE);
}

void Am_Value::Set_Empty ()
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  value.voidptr_value = NULL;
  type = Am_NONE;
}

void Am_Value::Set_Value_Type (Am_Value_Type new_type)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  value.voidptr_value = NULL;
  type = new_type;
}

Am_Value& Am_Value::operator= (Am_Wrapper* in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  value.wrapper_value = in_value;
  if (in_value)
    type = in_value->ID ();
  else
    type = Am_WRAPPER_TYPE;
  return *this;
}

Am_Value& Am_Value::operator= (Am_Ptr in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_VOIDPTR;
  value.voidptr_value = in_value;
  return *this;
}

Am_Value& Am_Value::operator= (int in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_INT;
  value.long_value = in_value;
  return *this;
}

Am_Value& Am_Value::operator= (long in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_LONG;
  value.long_value = in_value;
  return *this;
}

#ifndef NEED_BOOL
Am_Value& Am_Value::operator= (bool in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_BOOL;
  value.bool_value = in_value;
  return *this;
}
#endif

Am_Value& Am_Value::operator= (float in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_FLOAT;
  value.float_value = in_value;
  return *this;
}

Am_Value& Am_Value::operator= (double in_value)
{
  //if ((type == Am_DOUBLE) &&
  //    ((double)*(Am_Double_Data*)value.wrapper_value == in_value))
  //  return *this;
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_DOUBLE;
  value.wrapper_value = new Am_Double_Data (in_value);
  return *this;
}

Am_Value& Am_Value::operator= (char in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_CHAR;
  value.long_value = 0;  // a temporary expediency: clear value first!!!
  value.char_value = in_value;
  return *this;
}

Am_Value& Am_Value::operator= (const char* in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_STRING;
  value.wrapper_value = new Am_String_Data (in_value);
  return *this;
}

Am_Value& Am_Value::operator= (const Am_String& in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_STRING;
  value.wrapper_value = in_value;
  return *this;
}

Am_Value& Am_Value::operator= (Am_Generic_Procedure* in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = Am_PROC;
  value.proc_value = in_value;
  return *this;
}

Am_Value& Am_Value::operator= (Am_Method_Wrapper* in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  if (in_value)
    type = (Am_Value_Type)in_value->ID ();
  else
    type = Am_METHOD_TYPE;
  value.method_value = in_value;
  return *this;
}

Am_Value& Am_Value::operator= (const Am_Value& in_value)
{
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Release ();
  type = in_value.type;
  value = in_value.value;
  if (Am_Type_Is_Ref_Counted (type) && value.wrapper_value)
    value.wrapper_value->Note_Reference ();
  return *this;
}

void Am_Value::Print (ostream& os) const {
  switch (Am_Type_Class (type)) {
  case Am_SIMPLE_TYPE:
    switch (type) {
    case Am_INT:
    case Am_LONG:
      os << value.long_value;
      break;
    case Am_BOOL:
      os << value.bool_value;
      break;
    case Am_FLOAT:
      os << value.float_value;
      break;
    case Am_DOUBLE:
      os << *(Am_Double_Data*)value.wrapper_value;
      break;
    case Am_CHAR:
      os << value.char_value;
      break;
    case Am_STRING:
      if (value.wrapper_value)
        os << (const char*)*(Am_String_Data*)value.wrapper_value;
      else
        os << "(NULL)";
      break;
    case Am_VOIDPTR:
      // explicitly cast, for the case when Am_Ptr == char*
      os << "(void*) " << hex << (long)value.voidptr_value << dec;
      break;
    case Am_PROC:
      os << hex << (long)value.proc_value << dec;
      break;
    case Am_NONE:
      os << "NONE (No value)";
      break;
    case Am_ZERO:
      os << "ZERO VALUE";
      break;
    default:
      cerr << "** This value cannot be printed, type = ";
      Am_Print_Type (cerr, type);
      cerr << "." << endl;
      Am_Error ();
      break;
    }
    break;
  case Am_WRAPPER:
    if (value.wrapper_value) // complains about const with call to valid
      value.wrapper_value->Print(os);
    else
      os << "(NULL)";
    break;
  case Am_METHOD:
    if (value.method_value)
      value.method_value->Print(os);
    else
      os << "(NULL)";
    break;
  case Am_ENUM:
  {
    Am_Type_Support* support = Am_Find_Support (type);
    if (support)
      support->Print (os, *this);
    else 
      Am_Standard_Print(os, value.long_value, type);
    break;
  }
  default: //some other type (works for the error types)
    Am_Standard_Print(os, value.long_value, type);
    break;
  }
}

const char * Am_Value::To_String() const {
  const char * ret_str = NULL;
  switch (Am_Type_Class (type)) {
  case Am_WRAPPER:
    if (value.wrapper_value) // complains about const with call to valid
      ret_str = value.wrapper_value->To_String();
    break;
  case Am_METHOD:
    if (value.method_value)
      ret_str = value.method_value->To_String();
    break;
  case Am_ENUM: {
    Am_Type_Support* support = Am_Find_Support (type);
    if (support)
      ret_str = support->To_String (*this);
    break;
  }
  }
  if (ret_str)
    return ret_str;
  else { //use expensive technique
    static char line[250];
    OSTRSTREAM_CONSTR (oss,line, 250,ios::out);
    oss << *this << ends;
    OSTRSTREAM_COPY(oss,line,250);
    return line;
  }
}

//******* Should parse the standard types. ****
Am_Value Am_Value::From_String(const char * string, Am_ID_Tag type) {
  const Am_Registered_Type* item = Am_Get_Named_Item (string);
  if (item) {
    if (Am_Type_Class (item->ID()) == Am_WRAPPER) {
      Am_Wrapper* wrapper = (Am_Wrapper*)item;
      wrapper->Note_Reference();
      Am_Value v(wrapper);
      return v;
    }
    else {
      Am_Value v((Am_Method_Wrapper*)item);
      return v;
    }
  }
  //else not registered by name
  Am_Type_Support* support = Am_Find_Support (type);
  if (support)
    return support->From_String(string);
  return Am_No_Value;
}

//not very efficient, mostly for debugging
const char* Am_To_String(const Am_Value &value) {
  return value.To_String();
}



#ifdef DEBUG
Am_Method_Wrapper::Am_Method_Wrapper(Am_ID_Tag* id_ptr,
				     Am_Generic_Procedure *p,
				     const char * name)
#else
Am_Method_Wrapper::Am_Method_Wrapper(Am_ID_Tag* id_ptr,
				     Am_Generic_Procedure *p,
				     const char * /* name */)
#endif
{
  ID_Ptr = id_ptr;
  Call = p;
#ifdef DEBUG
  Am_Register_Name(this, name);
#endif
}

ostream& operator<< (ostream& os, const Am_Value& value) {
  value.Print(os);
  return os;
}

//////////////////////////////////////////////
// Implementation of Type wrapper registration

const char *Am_Enum_To_String_Helper(Am_ID_Tag type, long value) {
  Am_Type_Support* support = Am_Find_Support (type);
  if (support) {
    Am_Value v(value, type);
    return support->To_String(v);
  }
  else return NULL;
}

void Am_Enum_Print_Helper(ostream& out, Am_ID_Tag type, long value) {
  Am_Type_Support* support = Am_Find_Support (type);
  if (support) {
    Am_Value v(value, type);
    support->Print(out, v);
  }
  else Am_Standard_Print(out, (unsigned long)value, type);
}


//if you put a type as the value of a value, with the type of the
//value being Am_TYPE_TYPE.
class Am_Type_Type_Support_Class : public Am_Type_Support {
public:
  void Print (ostream& os, const Am_Value& value) const {
#ifdef DEBUG
    os << Am_Get_Type_Name((Am_ID_Tag)value.value.long_value);
#else
    os << value.value.long_value;
#endif  
  }
#ifdef DEBUG
  const char * To_String(const Am_Value &value) const {
    return Am_Get_Type_Name((Am_ID_Tag)value.value.long_value);
#else
  const char * To_String(const Am_Value & /*value*/) const {
    return NULL;
#endif  
  }
  Am_Value From_String (const char* /*string*/) const {
    cerr << "**Sorry, no mapping from type names to Am_ID_Tags\n" << flush; 
    return Am_No_Value; // not implemented since types use a 1-way hash table
  }
  ~Am_Type_Type_Support_Class() {}
};

Am_Type_Support *type_type_support = new Am_Type_Type_Support_Class();

Am_DECL_MAP (Support, Am_Value_Type, Am_Type_Support*)
Am_IMPL_MAP (Support, Am_Value_Type, Am_NONE, Am_Type_Support*, NULL)
static Am_Map_Support * Support_Table;

inline void verify_support_table()
{
  if (!Support_Table) {
    Support_Table = new Am_Map_Support;
    Support_Table->SetAt (Am_TYPE_TYPE, type_type_support);
  }
}

void Am_Register_Support (Am_Value_Type type, Am_Type_Support* support)
{
  verify_support_table();
  Support_Table->SetAt (type, support);
}

Am_Type_Support* Am_Find_Support (Am_Value_Type type)
{
  verify_support_table();
  return Support_Table->GetAt (type);
}

void Am_Type_Support::Print (ostream& out, const Am_Value& v) const {
  Am_Standard_Print(out, (unsigned long)v.value.long_value, v.type);
}
const char * Am_Type_Support::To_String(const Am_Value & /*v*/) const {
  return NULL;
}

Am_Value Am_Type_Support::From_String (const char*) const
{ return Am_No_Value; }

Am_Enum_Support::Am_Enum_Support (const char* string, Am_Value_Type in_type)
{
  type = in_type;
  int len = strlen (string);
  char* hold = new char [len + 1];
  strcpy (hold, string);
  int i;
  number = 1;
  for (i = 0; i < len; ++i) {
    if (string[i] == ' ')
      ++number;
  }
  char** array = new char* [number];
  array[0] = hold;
  int j = 1;
  for (i = 0; i < len; ++i) {
    if (hold[i] == ' ') {
      hold[i] = '\0';
      array[j] = &hold[i+1];
      ++j;
    }
  }
  value_string = hold;
  item = array;
  Am_Register_Support (type, this);
}

Am_Enum_Support::~Am_Enum_Support ()
{
  delete[] (char *) value_string;
  delete[] item;
}

void Am_Enum_Support::Print (ostream& os, const Am_Value& value) const
{
  long index = value.value.long_value;
  if (index < 0 || index >= number) {
    os << "(";
    Am_Print_Type (os, value.type);
    os << ")" << index << " (ILLEGAL VALUE)";
  }
  else os << item[index];
}

const char * Am_Enum_Support::To_String(const Am_Value &value) const {
  long index = value.value.long_value;
  if (index < 0 || index >= number) return NULL;
  else return item[index];
}

Am_Value Am_Enum_Support::From_String (const char* string) const
{
  int i;
  for (i = 0; i < number; ++i) {
    if (!strcmp (string, item[i]))
      return Am_Value (i, type);
  }
  return Am_No_Value;
}

Am_Value Am_Enum_Support::Fetch (int item)
{
  if (item < number)
    return Am_Value (item, type);
  else
    return Am_No_Value;
}


//////////////////////////////////////
// Implementation of Load/Save context

Am_Define_Method_Type_Impl (Am_Load_Method);
Am_Define_Method_Type_Impl (Am_Save_Method);

Am_Load_Save_Context Am_No_Load_Save_Context;
Am_Load_Method Am_No_Load_Method;
Am_Save_Method Am_No_Save_Method;

class Wrapper_Holder {
 public:
  Wrapper_Holder ()
  { data = NULL; }
  Wrapper_Holder (Am_Wrapper* in_data)
  { data = in_data; }
  Wrapper_Holder (const Wrapper_Holder& item)
  {
    data = item.data;
    if (data) data->Note_Reference ();
  }
  ~Wrapper_Holder ()
  { if (data) data->Release (); }
  Wrapper_Holder& operator= (const Wrapper_Holder& item)
  {
    data = item.data;
    if (data) data->Note_Reference ();
	return *this;
  }
  operator Am_Wrapper* ()
  {
    if (data) data->Note_Reference ();
    return data;
  }
  bool Valid ()
  { return data != NULL; }

  Am_Wrapper* data;
};

Wrapper_Holder No_Wrapper;

class Name_Num {
 public:
  Name_Num ()
  { name = NULL; number = -1; }
  Name_Num (const char* in_name)
  { name = in_name; number = -1; }
  Name_Num (const char* in_name, int in_number)
  { name = in_name; number = in_number; }
  Name_Num (const Name_Num& proto)
  { name = proto.name; number = proto.number; }
  Name_Num& operator= (const Name_Num& proto)
  { name = proto.name; number = proto.number; return *this; }

  const char* name;
  int number;
};

static int HashValue (const Wrapper_Holder& key, int size)
{
  return (long)key.data % size;
}

static int KeyComp (const Wrapper_Holder& key1, const Wrapper_Holder& key2)
{
  return !(key1.data == key2.data);
}

static int HashValue (const Name_Num& key, int size)
{
  int hash = (HashValue (key.name, size) + key.number + 10) % size;
  return hash;
}

static int KeyComp (const Name_Num& key1, const Name_Num& key2)
{
  return strcmp (key1.name, key2.name) || key1.number != key2.number;
}

Am_DECL_MAP (Loader, const char*, Am_Load_Method)
Am_IMPL_MAP (Loader, const char*, NULL, Am_Load_Method, Am_No_Load_Method)
Am_DECL_MAP (Saver, Am_ID_Tag, Am_Save_Method)
Am_IMPL_MAP (Saver, Am_ID_Tag, Am_NONE, Am_Save_Method, Am_No_Save_Method)
Am_DECL_MAP (Prototypes, Name_Num, Wrapper_Holder)
Am_IMPL_MAP (Prototypes, Name_Num, Name_Num (), Wrapper_Holder, No_Wrapper)
Am_DECL_MAP (Names, Wrapper_Holder, Name_Num)
Am_IMPL_MAP (Names, Wrapper_Holder, No_Wrapper, Name_Num, Name_Num ())
Am_DECL_MAP (Wrappers, int, Wrapper_Holder)
Am_IMPL_MAP (Wrappers, int, -1, Wrapper_Holder, No_Wrapper)
Am_DECL_MAP (References, Wrapper_Holder, int)
Am_IMPL_MAP (References, Wrapper_Holder, No_Wrapper, int, -1)

class Am_Load_Save_Context_Data : public Am_Wrapper {
  Am_WRAPPER_DATA_DECL (Am_Load_Save_Context)
 public:
  Am_Load_Save_Context_Data ();
  Am_Load_Save_Context_Data (Am_Load_Save_Context_Data* proto);
  ~Am_Load_Save_Context_Data ();
  bool operator== (const Am_Load_Save_Context_Data& /*test*/) const
  { return false; }

  Am_Map_Loader* loader;
  Am_Map_Saver* saver;
  Am_Map_Prototypes* prototypes;
  Am_Map_Names* names;
  int counter;
  Am_Map_Wrappers* wrappers;
  Am_Map_References* references;
};

Am_WRAPPER_DATA_IMPL (Am_Load_Save_Context, (this))

Am_Load_Save_Context_Data::Am_Load_Save_Context_Data ()
{
  loader = new Am_Map_Loader ();
  saver = new Am_Map_Saver ();
  prototypes = new Am_Map_Prototypes ();
  names = new Am_Map_Names ();
  counter = 0;
  wrappers = new Am_Map_Wrappers ();
  references = new Am_Map_References ();
}

Am_Load_Save_Context_Data::Am_Load_Save_Context_Data (
           Am_Load_Save_Context_Data* proto)
{
  loader = proto->loader->Copy ();
  saver = proto->saver->Copy ();
  prototypes = proto->prototypes->Copy ();
  names = proto->names->Copy ();
  counter = 0;
  wrappers = new Am_Map_Wrappers ();
  references = new Am_Map_References ();
}

Am_Load_Save_Context_Data::~Am_Load_Save_Context_Data ()
{
  loader->Clear ();
  delete loader;
  saver->Clear ();
  delete saver;
  prototypes->Clear ();
  delete prototypes;
  names->Clear ();
  delete names;
  wrappers->Clear ();
  delete wrappers;
  references->Clear ();
  delete references;
}

Am_WRAPPER_IMPL (Am_Load_Save_Context)

void Am_Load_Save_Context::Register_Prototype (const char* name,
					       Am_Wrapper* value)
{
  if (!data)
    data = new Am_Load_Save_Context_Data ();
  data = (Am_Load_Save_Context_Data*)data->Make_Unique();
  Wrapper_Holder holder (value);
  data->names->SetAt (holder, Name_Num (name));
  data->prototypes->SetAt (Name_Num (name), holder);
}

void Am_Load_Save_Context::Register_Prototype (const char* name, unsigned base,
					       Am_Wrapper* value)
{
  if (!data)
    data = new Am_Load_Save_Context_Data ();
  data = (Am_Load_Save_Context_Data*)data->Make_Unique();
  Wrapper_Holder holder (value);
  data->names->SetAt (holder, Name_Num (name, base));
  data->prototypes->SetAt (Name_Num (name, base), holder);
}

void Am_Load_Save_Context::Register_Loader (const char* type_name,
					    const Am_Load_Method& method)
{
  if (!data)
    data = new Am_Load_Save_Context_Data ();
  data = (Am_Load_Save_Context_Data*)data->Make_Unique();
  data->loader->SetAt (type_name, method);
}

void Am_Load_Save_Context::Register_Saver (Am_ID_Tag type,
					   const Am_Save_Method& method)
{
  if (!data)
    data = new Am_Load_Save_Context_Data ();
  data = (Am_Load_Save_Context_Data*)data->Make_Unique();
  data->saver->SetAt (type, method);
}

void Am_Load_Save_Context::Reset ()
{
  if (data) {
    data = (Am_Load_Save_Context_Data*)data->Make_Unique();
    data->counter = 0;
    data->wrappers->Clear ();
    data->references->Clear ();
  }
}

Am_Value Am_Load_Save_Context::Load (istream& is)
{
  if (!data)
    Am_Error ("Load called on a NULL load/save context");
  char ch;
  // OLD if (is.get (ch)) {
  is >> ch;  //the >> will skip whitespace
  if (ch) {
    if (ch == 'D') {
      int number;
      if (! (is >> number))
        return Am_No_Value;
      data->counter = number;
      // is.get (ch); // skip eoln
      // if (ch != '\n') return Am_No_Value;
      char type_name[100];
      if (! (is >> type_name)) // the >> will skip whitespace
        return Am_No_Value;
      // is.get (ch); // skip eoln
      // if (ch != '\n') return Am_No_Value;
      Am_Load_Method method = data->loader->GetAt (type_name);
      if (!method.Valid ())
        return Am_No_Value;
      Am_Value value = method.Call (is, *this);
      if (value.Valid () && Am_Type_Class (value.type) == Am_WRAPPER)
        data->wrappers->SetAt (number, Wrapper_Holder (value));
      return value;
    }
    if (ch == 'R') {
      if (! (is >> data->counter))
        return Am_No_Value;
      // is.get (ch); // skip eoln
      // if (ch != '\n') return Am_No_Value;
      Wrapper_Holder value = data->wrappers->GetAt (data->counter);
      if (!value.Valid ())
        return Am_No_Value;
      return Am_Value (value);
    }
    if (ch == 'N') {
      // is.get (ch); // skip eoln
      // if (ch != '\n') return Am_No_Value;
      return Am_Value (No_Wrapper);
    }
    if (ch == 'P') {
      char name[100];
      if (! (is >> name)) 
        return Am_No_Value;
      is.get (ch); // will be space or (part of) EOLN
      Wrapper_Holder value;
      if (ch == ' ') {
        int base;
        if (! (is >> base))
          return Am_No_Value;
        value = data->prototypes->GetAt (Name_Num (name, base));
        // is.get (ch); // skip eoln
        // if (ch != '\n') return Am_No_Value;
      }
      else value = data->prototypes->GetAt (Name_Num (name));
      if (!value.Valid ())
        return Am_No_Value;
      return Am_Value (value);
    }
  }
  return Am_No_Value;
}

const char* Am_Load_Save_Context::Is_Registered_Prototype (Am_Wrapper* value) {
  Wrapper_Holder holder (value);
  if (!data) return NULL;
  Name_Num name = data->names->GetAt (holder);
  return name.name;
}

void Am_Load_Save_Context::Save (ostream& os, const Am_Value& value)
{
  if (!data)
    Am_Error ("Save called on a NULL load/save context");
  if (Am_Type_Class (value.type) == Am_WRAPPER) {
    if (!value.Valid ()) {
      os << "N" << endl;
      return;
    }
    Wrapper_Holder holder (value);
    int reference = data->references->GetAt (holder);
    if (reference != -1) {
      os << "R" << reference << endl;
      return;
    }
    Name_Num name = data->names->GetAt (holder);
    if (name.name) {
      os << "P" << name.name;
      if (name.number == -1)
        os << endl;
      else
        os << " " << name.number << endl;
      return;
    }
    data->references->SetAt (holder, data->counter);
  }
  os << "D" << data->counter << endl;
  ++data->counter;
  Am_Save_Method method = data->saver->GetAt (value.type);
  if (!method.Valid()) {
    cerr << "** Don't have a method for saving values of type ";
    Am_Print_Type (cerr, value.type);
    cerr << endl;
    Am_Error();
  }
  method.Call (os, *this, value);
}

void Am_Load_Save_Context::Recursive_Load_Ahead (Am_Wrapper* value)
{
  if (!data)
    Am_Error ("Recursive_Load_Ahead called on a NULL load/save context");
  if (value)
    data->wrappers->SetAt (data->counter, Wrapper_Holder (value));
  else
    Am_Error ("Not allowed to store NULL as a load ahead");
}

void Am_Load_Save_Context::Save_Type_Name (ostream& os, const char* type_name)
{
  if (!data)
    Am_Error ("Save_Type_Name called on a NULL load/save context");
  os << type_name << endl;
}

// Used to get around this current C++ madness
void reset_ostrstream(OSTRSTREAM &oss)
{
#if __MWERKS__
   oss.rdbuf()->pubseekoff(0, ios::beg);
#else
   oss.seekp(ios::beg);
 #endif
}
