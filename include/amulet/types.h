/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifndef TYPES_H
#define TYPES_H

#include <am_inc.h>

#include AM_IO__H

#ifndef NULL
#define NULL (void*)0
#endif

#ifdef NEED_BOOL
// Define a Boolean type because some c++ do not support the bool type, yet.
// Hopefully, this will someday disappear.

#ifdef _WIN32
#pragma warning( disable : 4237 )
#endif

#define true 1
#define false 0
typedef int bool;
#endif

// Am_ID_Tag is used to name classes derived from Am_Wrapper.
typedef unsigned short Am_ID_Tag;

// Am_Debugger points to a routine that is invoked whenever DEBUG is defined
// and an Am_Error occurs.
extern void (*Am_Debugger) (void);

// Call Am_Error to generate a software generated error.  Hopefully, someday
// this will be replaced by exceptions.
extern void Am_Error (const char* error_string);
// Am_Error prints out "** Program Aborted"
extern void Am_Error ();

//This version can be used with a print-out string like:
//  Am_ERROR(object << " is not valid")
#define Am_ERROR(error_string)  			\
{ cerr << "** Amulet_Error: " << error_string << endl;  \
  Am_Error();						\
}

// A helper function that generates Am_ID_Tag's for classes typically derived
// from Am_Wrapper.  Providing a base tag will OR that base onto the returned
// id.  Providing a base with an already defined subnumber will return that
 // number.
extern Am_ID_Tag Am_Get_Unique_ID_Tag (const char* type_name,
                                       Am_ID_Tag base = 0);
extern Am_ID_Tag Am_Set_ID_Class (const char* type_name, Am_ID_Tag base);

//prints val as (type_name) 0xXXXXX
extern void Am_Standard_Print(ostream& out, unsigned long val, Am_ID_Tag type);

// These helper functions strip apart the various portions of an ID tag.
// The first returns the base number of the id.
inline Am_ID_Tag Am_Type_Base (Am_ID_Tag in_id)
{
  return in_id & 0x0FFF;
}

// The second returns the class portion of the id.
inline Am_ID_Tag Am_Type_Class (Am_ID_Tag in_id)
{
  return in_id & 0x7000;
}

// The third returns true iff the type is a reference-counted pointer.
inline bool Am_Type_Is_Ref_Counted (Am_ID_Tag in_id)
{
  return (in_id & 0x8000) != 0;
}

// Am_Value_Type is an enumeration of all the distinct types that can be
// stored in a Am_Value object.
typedef Am_ID_Tag Am_Value_Type;

// Wrapper flag
const Am_Value_Type Am_REF_COUNTED = 0x8000;

// Basic supertypes. Can be 3 bits (1000 to 7000)
const Am_Value_Type Am_SIMPLE_TYPE      = 0x0000;
const Am_Value_Type Am_ERROR_VALUE_TYPE = 0x1000;
const Am_Value_Type Am_WRAPPER 		= 0x2000;
const Am_Value_Type Am_METHOD  		= 0x3000;
const Am_Value_Type Am_ENUM    		= 0x4000;

// Basic types.
const Am_Value_Type Am_NONE    =  0;
const Am_Value_Type Am_ZERO    =  1;
const Am_Value_Type Am_INT     =  2;
const Am_Value_Type Am_LONG    =  3;
const Am_Value_Type Am_BOOL    =  4;
const Am_Value_Type Am_FLOAT   =  5;
const Am_Value_Type Am_DOUBLE  =  6 | Am_REF_COUNTED;
const Am_Value_Type Am_CHAR    =  7;
const Am_Value_Type Am_STRING  =  8 | Am_REF_COUNTED;
const Am_Value_Type Am_VOIDPTR =  9;
const Am_Value_Type Am_PROC    =  10;
const Am_Value_Type Am_WRAPPER_TYPE = Am_WRAPPER | Am_REF_COUNTED;
const Am_Value_Type Am_METHOD_TYPE  = Am_METHOD;
const Am_Value_Type Am_ENUM_TYPE    = Am_ENUM;
const Am_Value_Type Am_FORMULA_INVALID     =  Am_ERROR_VALUE_TYPE | 1; //4097
const Am_Value_Type Am_MISSING_SLOT  	   =  Am_ERROR_VALUE_TYPE | 2; //4098
const Am_Value_Type Am_GET_ON_NULL_OBJECT  =  Am_ERROR_VALUE_TYPE | 3; //4099

const Am_Value_Type Am_TYPE_TYPE     = Am_ENUM | 20 ; //type of type names
const Am_Value_Type Am_SLOT_KEY_TYPE = Am_ENUM | 21 ; //type of slot keys

extern void Am_Print_Type (ostream& os, Am_Value_Type type);

#if defined(_MSC_VER) || __MWERKS__
typedef unsigned char* Am_Ptr;
#else
typedef void* Am_Ptr;
#endif

// A procedure type to use as a placeholder
typedef void Am_Generic_Procedure ();


class Am_Registered_Type; //forward reference
class Am_Wrapper; //forward reference
class Am_Method_Wrapper; //forward reference
class Am_String; // Forward reference (see below)

// This class is a union of all the distinct types that can be stored as a
// single entity.  It is used as object slots and value list items.  Certain
// Get and Set functions use the type to make it possible to set/retrieve
// the value without having to know its type explicitly.
class Am_Value {
 public:
  Am_Value_Type type;
  union {  // Storage for the value.
    Am_Wrapper*           wrapper_value;
    Am_Ptr                voidptr_value;
    long                  long_value;
    bool                  bool_value;
    float                 float_value;
    char                  char_value;  // FIX: should we get rid of this?  could put in long
    Am_Method_Wrapper*    method_value;
    Am_Generic_Procedure* proc_value; //like void* for procedures
  } value;

  // Casting operators to mitigate some syntax.
  operator Am_Wrapper* () const;
  operator Am_Ptr () const;
  operator int () const;
  operator long () const;
#ifndef NEED_BOOL
  operator bool () const;
#endif
  operator float () const;
  operator double () const;
  operator char () const;
  operator Am_Generic_Procedure* () const;
  operator Am_Method_Wrapper* () const;

  bool operator== (Am_Wrapper* test_value) const;
  bool operator== (Am_Ptr test_value) const;
  bool operator== (int test_value) const;
  bool operator== (long test_value) const;
#ifndef NEED_BOOL
  bool operator== (bool test_value) const;
#endif
  bool operator== (float test_value) const;
  bool operator== (double test_value) const;
  bool operator== (char test_value) const;
  bool operator== (const char* test_value) const;
  bool operator== (const Am_String& test_value) const;
  bool operator== (Am_Generic_Procedure* test_value) const;
  bool operator== (Am_Method_Wrapper* test_value) const;
  bool operator== (const Am_Value& test_value) const;

  bool operator!= (Am_Wrapper* test_value) const;
  bool operator!= (Am_Ptr test_value) const;
  bool operator!= (int test_value) const;
  bool operator!= (long test_value) const;
#ifndef NEED_BOOL
  bool operator!= (bool test_value) const;
#endif
  bool operator!= (float test_value) const;
  bool operator!= (double test_value) const;
  bool operator!= (char test_value) const;
  bool operator!= (const char* test_value) const;
  bool operator!= (const Am_String& test_value) const;
  bool operator!= (Am_Generic_Procedure* test_value) const;
  bool operator!= (Am_Method_Wrapper* test_value) const;
  bool operator!= (const Am_Value& test_value) const;

  // Returns true if value represents an actual slot value; false if value is an error code (like Am_MISSING_SLOT)  
  bool Safe () const;
  // Returns true if value contains a piece of typed data; false if value is an error code or an empty value (Am_NONE)
  bool Exists () const;
  // Returns true if value's binary representation is nonzero; false if not Exists() or is zero
  bool Valid () const;

  // Clears contents and sets to Am_NONE
  void Set_Empty ();

  // Clears contents and makes value be of the specified type
  void Set_Value_Type (Am_Value_Type new_type);

  // Creation operations provided to aid initialization.
  Am_Value ()
  {
    type = Am_NONE;
    value.long_value = 0;
  }
  Am_Value (Am_Wrapper* initial);
  Am_Value (Am_Ptr initial)
  {
    type = Am_VOIDPTR;
    value.voidptr_value = initial;
  }
  Am_Value (int initial)
  {
    type = Am_INT;
    value.long_value = initial;
  }
  Am_Value (long initial)
  {
    type = Am_LONG;
    value.long_value = initial;
  }
#if !defined(NEED_BOOL)
  Am_Value (bool initial)
  {
    type = Am_BOOL;
    value.bool_value = initial;
  }
#endif
  Am_Value (float initial)
  {
    type = Am_FLOAT;
    value.float_value = initial;
  }
  Am_Value (double initial);
  Am_Value (char initial)
  {
    type = Am_CHAR;
	value.long_value = 0;  // a temporary expediency: clear value first!!!
    value.char_value = initial;
  }
  Am_Value (const char* initial);
  Am_Value (const Am_String& initial);
  Am_Value (Am_Generic_Procedure* initial)
  {
    type = Am_PROC;
    value.proc_value = initial;
  }
  Am_Value (Am_Method_Wrapper* initial);
  Am_Value (long in_value, Am_Value_Type in_type)
  {
    type = in_type;
    value.long_value = in_value;
  }
  Am_Value (const Am_Value& initial);

  ~Am_Value ();

  // Assignment operators have been provided to ease some syntax juggling.
  Am_Value& operator= (Am_Wrapper* in_value);
  Am_Value& operator= (Am_Ptr in_value);
  Am_Value& operator= (int in_value);
  Am_Value& operator= (long in_value);
#ifndef NEED_BOOL
  Am_Value& operator= (bool in_value);
#endif
  Am_Value& operator= (float in_value);
  Am_Value& operator= (double in_value);
  Am_Value& operator= (char in_value);
  Am_Value& operator= (const char* in_value);
  Am_Value& operator= (const Am_String& in_value);
  Am_Value& operator= (Am_Generic_Procedure* in_value);
  Am_Value& operator= (Am_Method_Wrapper* in_value);
  Am_Value& operator= (const Am_Value& in_value);

  void Print(ostream& out) const;
  void Println() const { Print(cout); cout << endl << flush; }

  //will use strstream and Print(out) if the value doesn't have a string name.
  const char * To_String() const;

  // if the string is in the global name registry, then the type is
  // not needed.  If the name is not there, then looks for a Am_Type_Support
  // for the ID.  Will return Am_No_Value if can't be found or can't be parsed.
  static Am_Value From_String(const char * string, Am_ID_Tag type = 0);
};

extern Am_Value Am_No_Value;
extern Am_Value Am_Zero_Value;

extern ostream& operator<< (ostream& os, const Am_Value& value);

//not very efficient, mostly for use inside the debugger.  Returns a
//string for any value.
extern const char* Am_To_String(const Am_Value &value);


// The Am_Registered_Type class.  Classes derived from this type have runtime
// type ID's, and pointers to them can be registered in the name registry.

class Am_Registered_Type {
public:
  // returns unique tag for each derived type.
  virtual Am_ID_Tag ID() const = 0;

  //printing and reading for debugging
  virtual const char * To_String() const;
  virtual void Print(ostream& out) const;

  void Println() const { Print(cout); cout << endl << flush; }

  virtual ~Am_Registered_Type () { } // destructor should be virtual
};

// The Am_Wrapper type.  Classes derived from this type can be stored in an
// object slot directly.
class Am_Wrapper : public Am_Registered_Type {
 public:
  Am_Wrapper ()
  { refs = 1; }
  Am_Wrapper (const Am_Wrapper&)
  { refs = 1; }
  virtual ~Am_Wrapper ()
  { }
  void Note_Reference ()   // Note that wrapper is being stored
  { ++refs; }
  unsigned Ref_Count ()    // Return the reference count.
  { return refs; }
  void Release ()          // Indicates data is no longer being used.
  { if (!--refs) delete this; }
  bool Is_Unique ()
  { return refs == 1; }
  bool Is_Zero ()
  { return refs == 0; }
  virtual Am_Wrapper* Make_Unique () = 0; // Return a unique copy of the data.
  virtual bool operator== (Am_Wrapper& test_value) = 0; // Equality test.
  virtual Am_ID_Tag ID () const = 0; // Returns unique tag
                                     // for each derived type.
  virtual Am_Value From_String(const char * string) const;
 private:
  unsigned refs;
};

/////////////////////////////////////////////////////////////////////////////
// Wrappers for methods
/////////////////////////////////////////////////////////////////////////////

class Am_Method_Wrapper : public Am_Registered_Type {
 public:
  // constructor registers method's name in name registry.
  Am_Method_Wrapper(Am_ID_Tag* id_ptr, Am_Generic_Procedure *p,
                    const char * name = 0);
  Am_ID_Tag ID() const {return *ID_Ptr;}
  Am_Generic_Procedure *Call;
  void Print(ostream& out) const;
  virtual Am_Value From_String(const char * string) const;

protected:
  Am_ID_Tag* ID_Ptr;
};  

/////////  TOP LEVEL MACRO FOR DEFINING NEW TYPES OF METHODS
/// args must be a parenthesized list of the parameter names and types.

#define Am_Define_Method_Type_Impl(Type_name)                \
  Am_ID_Tag Type_name::Type_name##_ID =                      \
      Am_Get_Unique_ID_Tag (#Type_name, Am_METHOD_TYPE);     \
ostream& operator<< (ostream& os, const Type_name& method) { \
  method.Print(os);         				     \
  return os;                                                 \
}

//This is assigned into the Call slot when there is no procedure pointer.
//If debugging, and get a null method, it will print out an error
//message.  If not debugging, and de-reference the 0, will get a
//"Illegal instruction" error at run time.  The
//Am_Null_Method_Error_Function is defined with ... so it can take any params.
typedef void Am_Any_Procedure(...);
#ifdef DEBUG
extern Am_Any_Procedure* Am_Null_Method_Error_Function;
#else
#define Am_Null_Method_Error_Function 0
#endif

#define Am_Define_Method_Type(Type_name, Return_type, Args)                  \
/* now, a typedef for the procedure type */                                  \
typedef Return_type Type_name##_Type Args;                                   \
class Type_name {                                                            \
 public:                                                                     \
  /* Variables: */                                                           \
  /*   the wrapper I was created from  */                                    \
  Am_Method_Wrapper* from_wrapper;                                           \
  /*   a pointer to the procedure of the appropriate type */                 \
  Type_name##_Type *Call;                                                    \
  /* allocate an ID for this type       called Type_name_ID*/                \
  static Am_ID_Tag Type_name##_ID;                                           \
  bool Valid () const {                                                      \
    return Call != (Type_name##_Type*)Am_Null_Method_Error_Function;         \
  }                                                                          \
  /* method to see if a procedure is my type (and valid) */                  \
  static int Test (Am_Method_Wrapper* wrapper) {                             \
     return wrapper && wrapper->ID() == Type_name::Type_name##_ID &&         \
            wrapper->Call !=                                                 \
               (Am_Generic_Procedure*)Am_Null_Method_Error_Function; }       \
  static int Test (const Am_Value& value) {                                  \
     return value.value.method_value &&                                      \
     value.type == Type_name::Type_name##_ID &&                              \
     value.value.method_value->Call !=                                       \
        (Am_Generic_Procedure*)Am_Null_Method_Error_Function; }              \
  static Am_Value_Type Type_ID ()                                            \
  { return Type_name##_ID; }                                                 \
  /* empty constructor */                                                    \
  Type_name () {                                                             \
    Call = (Type_name##_Type*)Am_Null_Method_Error_Function;                 \
    from_wrapper = 0; }                                                      \
  /* constructor of this class from a wrapper */                             \
  Type_name (Am_Method_Wrapper* wrapper) {                                   \
    from_wrapper = wrapper;                                                  \
    if (from_wrapper)                                                        \
      Call = (Type_name##_Type*)from_wrapper->Call;                          \
    else                                                                     \
      Call = (Type_name##_Type*)Am_Null_Method_Error_Function;               \
  }                                                                          \
  /* constructor of this class from a wrapper, with the                      \
     procedure pointer passed for type-checking purposes */                  \
  Type_name (Am_Method_Wrapper* wrapper,                                     \
             Type_name##_Type*) {			                     \
    from_wrapper = wrapper;                                                  \
    if (from_wrapper)                                                        \
      Call = (Type_name##_Type*)from_wrapper->Call;                          \
    else                                                                     \
      Call = (Type_name##_Type*)Am_Null_Method_Error_Function;               \
  }                                                                          \
  Type_name (const Am_Value& value) {                                        \
    from_wrapper = value;                                                    \
    if (from_wrapper) {                                                      \
      if (value.type != Type_name::Type_name##_ID) {                         \
        cerr << "** Tried to assign a " #Type_name " method with a non "     \
                 #Type_name " wrapper: " << value << endl;                   \
	Am_Error();						             \
      }				   		                             \
      Call = (Type_name##_Type*)from_wrapper->Call;                          \
    }                                                                        \
    else                                                                     \
      Call = (Type_name##_Type*)Am_Null_Method_Error_Function;               \
  }                                                                          \
  Type_name& operator= (Am_Method_Wrapper* wrapper) {                        \
    from_wrapper = wrapper;                                                  \
    if (from_wrapper)                                                        \
      Call = (Type_name##_Type*)from_wrapper->Call;                          \
    else                                                                     \
      Call = (Type_name##_Type*)Am_Null_Method_Error_Function;               \
    return *this;                                                            \
  }                                                                          \
  Type_name& operator= (const Am_Value& value) {                             \
    from_wrapper = value;                                                    \
    if (from_wrapper) {                                                      \
      if (value.type != Type_name::Type_name##_ID) {                         \
        cerr << "** Tried to assign a " #Type_name " method with a non "     \
                 #Type_name " wrapper: " << value << endl;                   \
	Am_Error();						             \
      }							                     \
      Call = (Type_name##_Type*)from_wrapper->Call;                          \
    }                                                                        \
    else                                                                     \
      Call = (Type_name##_Type*)Am_Null_Method_Error_Function;               \
    return *this;                                                            \
  }                                                                          \
  /* convert me into a Am_Method_Wrapper so I can be stored into a slot */   \
  operator Am_Method_Wrapper* () const {return from_wrapper;}                \
  /* Printing functions */                                                   \
  const char * To_String() const { return from_wrapper->To_String(); }       \
  void Print(ostream& out) const { from_wrapper->Print(out); }               \
};                                                                           \
extern ostream& operator<< (ostream& os, const Type_name& method);

/////////  MACRO FOR DEFINING NEW METHODS OF A PRE_DEFINED TYPE 
/// return_type and args must be the same as used for defining Type_name

#define Am_Define_Method(Type_name, Return_type, Method, Args)              \
/* declare the procedure so can make a pointer to it */                     \
static Return_type Method##_proc Args;                                      \
/* allocate a wrapper object holding the procedure */                       \
Am_Method_Wrapper Method##_inst  =                                          \
      Am_Method_Wrapper(&Type_name::Type_name##_ID,                         \
            (Am_Generic_Procedure*)&Method##_proc,                          \
            #Method);                                                       \
/* Create a Type_name class object for the procedure, and call it Method    \
   (passing the wrapper object is sufficient for construction, but we       \
   also pass the procedure pointer to type-check its signature against      \
   the signature expected of Type_name methods). */                         \
Type_name Method (&Method##_inst, Method##_proc);	                    \
/* now the procedure header, so this can be followed by the code */         \
Return_type Method##_proc Args          
/* code goes here { ... }  */

/////////////////////////////////////////
// Creating and registering Enum types //
/////////////////////////////////////////
  
// This class is used to implement reading and writing functions for wrappers
// that are not derived from Am_Wrapper.  Presently, it is used for enumeration
// wrappers and for pointer wrappers.
class Am_Type_Support {
 public:
  // Prints to a stream a short debugging statement.  Used for displaying for
  // a user, not for permanent storage.
  virtual void Print (ostream& os, const Am_Value& value) const;

  void Println(const Am_Value& value) const
     { Print(cout, value); cout << endl << flush; }

  //Returns the string if easy and doesn't require any allocation.
  //Otherwise, returns NULL.
  virtual const char * To_String(const Am_Value &v) const;
  
  // Reads a string, potentially provided by a user and converts to its own
  // type.  Returns Am_No_Value when there is an error.
  virtual Am_Value From_String (const char* string) const;

  virtual ~Am_Type_Support () { } // virtual destructor
};

// Simple wrapper types can be registered with the system so that their values
// may be printed in the debugger and other places.
extern void Am_Register_Support (Am_Value_Type type,
				 Am_Type_Support* support);
extern Am_Type_Support* Am_Find_Support (Am_Value_Type type);

// Special ID generator useful for registering the support class.
extern Am_ID_Tag Am_Get_Unique_ID_Tag (const char* type_name,
				       Am_Type_Support* support,
                                       Am_ID_Tag base);

extern const char *Am_Enum_To_String_Helper(Am_ID_Tag type, long value);
extern void Am_Enum_Print_Helper(ostream& out, Am_ID_Tag type, long value);

#define Am_Define_Enum_Type(Type_name, Enum_name)                \
class Type_name {                                                \
 public:                                                         \
  Type_name (const Am_Value& in_value)                           \
  {                                                              \
    if (in_value.type != Am_ZERO && in_value.type != Type_name##_ID) {  \
        cerr << "** Tried to assign a " #Type_name " enum with a non "  \
                 #Type_name " wrapper: " << in_value << endl;    \
	Am_Error();						 \
      }							         \
    value = (Enum_name)in_value.value.long_value;                \
  }                                                              \
  Type_name (Enum_name in_value)                                 \
  { value = in_value; }                                          \
  Type_name ()                                                   \
  { value = (Enum_name)0; }                                      \
  Type_name& operator= (Enum_name in_value)                      \
  { value = in_value; return *this; }                            \
  Type_name& operator= (const Am_Value& in_value)                \
  {                                                              \
    if (in_value.type != Am_ZERO && in_value.type != Type_name##_ID) {  \
        cerr << "** Tried to set a " #Type_name " enum with a non "     \
                 #Type_name " wrapper: " << in_value << endl;    \
	Am_Error();						 \
      }							         \
    value = (Enum_name)in_value.value.long_value;                \
    return *this;                                                \
  }                                                              \
  operator Am_Value () const                                     \
  { return Am_Value ((long)value, Type_name##_ID); }             \
  bool operator== (Enum_name test_value) const                   \
  { return value == test_value; }                                \
  bool operator== (Type_name test_value) const                   \
  { return value == test_value.value; }                          \
  bool operator!= (Enum_name test_value) const                   \
  { return value != test_value; }                                \
  bool operator!= (Type_name test_value) const                   \
  { return value != test_value.value; }                          \
  static bool Test (const Am_Value& value)                       \
  { return value.type == Type_name##_ID; }                       \
  static Am_Value_Type Type_ID ()                                \
  { return Type_name##_ID; }                                     \
  /* Printing functions */                                       \
  const char * To_String() const {                               \
    return Am_Enum_To_String_Helper(Type_name##_ID, (long)value);       \
  }                                                              \
  void Print(ostream& out) const {				 \
    Am_Enum_Print_Helper(out, Type_name##_ID, (long)value);      \
  }                                                              \
  void Println() const {					 \
    Print(cout); cout << endl << flush;			         \
  }                                                              \
  Enum_name value;                                               \
 private:                                                        \
  static Am_ID_Tag Type_name##_ID;                               \
};                                                               \
extern ostream& operator<< (ostream& os, const Type_name& value);

#define Am_Define_Enum_Long_Type(Type_name)                      \
class Type_name {                                                \
 public:                                                         \
  Type_name (const Am_Value& in_value)                           \
  {                                                              \
    if (in_value.type != Am_ZERO && in_value.type != Type_name##_ID) {  \
        cerr << "** Tried to set a " #Type_name " enum with a non "    \
                 #Type_name " wrapper: " << in_value << endl;    \
	Am_Error();						 \
      }							         \
    value = (long)in_value.value.long_value;                     \
  }                                                              \
  Type_name (long in_value = 0)                                  \
  { value = in_value; }                                          \
  Type_name& operator= (long in_value)                           \
  { value = in_value; return *this; }                            \
  Type_name& operator= (const Am_Value& in_value)                \
  {                                                              \
    if (in_value.type != Am_ZERO && in_value.type != Type_name##_ID) {  \
        cerr << "** Tried to set a " #Type_name " enum with a non "    \
                 #Type_name " wrapper: " << in_value << endl;    \
	Am_Error();						 \
      }							         \
    value = (long)in_value.value.long_value;                     \
    return *this;                                                \
  }                                                              \
  operator Am_Value () const                                     \
  { return Am_Value ((long)value, Type_name##_ID); }             \
  bool operator== (long test_value) const                        \
  { return value == test_value; }                                \
  bool operator== (Type_name test_value) const                   \
  { return value == test_value.value; }                          \
  bool operator!= (long test_value) const                        \
  { return value != test_value; }                                \
  bool operator!= (Type_name test_value) const                   \
  { return value != test_value.value; }                          \
  static bool Test (const Am_Value& value)                       \
  { return value.type == Type_name##_ID; }                       \
  static Am_Value_Type Type_ID ()                                \
  { return Type_name##_ID; }                                     \
  /* Printing functions */                                       \
  const char * To_String() const {                               \
    return Am_Enum_To_String_Helper(Type_name##_ID, (long)value);       \
  }                                                              \
  void Print(ostream& out) const {				 \
    Am_Enum_Print_Helper(out, Type_name##_ID, (long)value);      \
  }                                                              \
  void Println() const {					 \
    Print(cout); cout << endl << flush;			         \
  }                                                              \
  long value;                                                    \
 private:                                                        \
  static Am_ID_Tag Type_name##_ID;                               \
};                                                               \
extern ostream& operator<< (ostream& os, const Type_name& value);

#define Am_Define_Enum_Type_Impl(Type_name, Type_Support_Ptr)            \
  Am_ID_Tag Type_name::Type_name##_ID =                                  \
      Am_Get_Unique_ID_Tag (#Type_name, Type_Support_Ptr, Am_ENUM_TYPE); \
ostream& operator<< (ostream& os, const Type_name& item) {               \
  item.Print(os);                                                        \
  return os;                                                             \
}

// The special macro Am_Define_Enum_Support permits an enumeration wrapper to
// be automatically defined with a printer and reader support class.  The
// second parameter is a string which contains a list of all the constants
// in the enumeration.  The string must be in order and each constant must be
// separated by a single space (the last constant is followed by \0).  The
// enumeration is assumed to be zero-based with no skipped numbers.
class Am_Enum_Support : public Am_Type_Support {
 protected:
  const char* value_string;
  char** item;
  int number;
  Am_Value_Type type;
 public:
  Am_Enum_Support (const char* value_string, Am_Value_Type type);
  ~Am_Enum_Support ();
  void Set_Type (Am_Value_Type in_type)
  { type = in_type; }

  void Print (ostream& os, const Am_Value& value) const;
  const char * To_String(const Am_Value &v) const;
  virtual Am_Value From_String (const char* string) const;

  int Number ()
  { return number; }
  Am_Value Fetch (int item);
};

#define Am_Define_Enum_Support(Type_name, Value_string)                    \
Am_ID_Tag Type_name::Type_name##_ID =                                      \
      Am_Get_Unique_ID_Tag (#Type_name, Am_ENUM_TYPE);                     \
Am_Enum_Support Type_name##_Support (Value_string, Type_name::Type_ID ()); \
ostream& operator<< (ostream& os, const Type_name& item) {                 \
  item.Print(os);                                                          \
  return os;                                                               \
}

//////////////////////////////////////
// Pointer Wrapper Macros -- for easily putting pointers to external objects
// into Amulet objects, with no reference counting or memory management.
//  If you need memory management, use "real" wrappers, below.
//  Note: Type_name is the class itself, not the pointer to the class
//////////////////////////////////////

#define Am_Define_Pointer_Wrapper(Type_name) 			             \
class Am_##Type_name {  					             \
public:  							             \
  Am_##Type_name (const Am_Value& in_value)                                  \
  {                                                                          \
    if (in_value.type != Am_ZERO && in_value.type != Am_##Type_name##_ID) {  \
        cerr << "** Tried to set a Am_" #Type_name " pointer wrapper "       \
	  "with a non Am_" #Type_name " wrapper: " << in_value << endl;      \
	Am_Error();						             \
      }							                     \
    value = (Type_name*)in_value.value.voidptr_value;                        \
  }                                                                          \
  Am_##Type_name (Type_name* in_value = NULL)                                \
  { value = in_value; }                                                      \
  Am_##Type_name& operator= (Type_name* in_value)                            \
  { value = in_value; return *this; }                                        \
  Am_##Type_name& operator= (const Am_Value& in_value)                       \
  {                                                                          \
    if (in_value.type != Am_ZERO && in_value.type != Am_##Type_name##_ID) {  \
        cerr << "** Tried to set a Am_" #Type_name " pointer wrapper "       \
	  "with a non Am_" #Type_name " wrapper: " << in_value << endl;      \
	Am_Error();						             \
      }							                     \
    value = (Type_name*)in_value.value.voidptr_value;                        \
    return *this;                                                            \
  }                                                                          \
  operator Am_Value () const                                                 \
  { return Am_Value ((long)value, Am_##Type_name##_ID); }                    \
  bool operator== (Type_name* test_value) const                              \
  { return value == test_value; }                                            \
  bool operator== (Am_##Type_name test_value) const                          \
  { return value == (Type_name*)test_value.value; }                          \
  bool operator!= (Type_name* test_value) const                              \
  { return value != test_value; }                                            \
  bool operator!= (Am_##Type_name test_value) const                          \
  { return value != (Type_name*)test_value.value; }                          \
  static bool Test (const Am_Value& value)                                   \
  { return value.type == Am_##Type_name##_ID; }                              \
  static Am_Value_Type Type_ID ()                                            \
  { return Am_##Type_name##_ID; }                                            \
  /* Printing functions */                                                   \
  const char * To_String() const {                                           \
    return Am_Enum_To_String_Helper(Am_##Type_name##_ID, (long)value);       \
  }                                                                          \
  void Print(ostream& out) const {				             \
    Am_Enum_Print_Helper(out, Am_##Type_name##_ID, (long)value);             \
  }                                                                          \
  void Println() const {					             \
    Print(cout); cout << endl << flush;			                     \
  }                                                                          \
  Type_name* value;                                                          \
 private:                                                                    \
  static Am_ID_Tag Am_##Type_name##_ID;                                      \
};                                                                           \
extern ostream& operator<< (ostream& os, const Am_##Type_name& value);

#define Am_Define_Pointer_Wrapper_Impl(Type_name, Type_Support_Ptr)   \
  Am_ID_Tag Am_##Type_name::Am_##Type_name##_ID =                  \
      Am_Get_Unique_ID_Tag ("Am_" #Type_name, Type_Support_Ptr,    \
			    Am_ENUM_TYPE); 			   \
ostream& operator<< (ostream& os, const Am_##Type_name& item) {    \
  item.Print(os);                                                  \
  return os;                                                       \
}

//////////////////////////////////////
// Definition of the Wrapper Macros //
//////////////////////////////////////

#define Am_WRAPPER_DATA_DECL(Type_name)                  \
   public:                                               \
    Am_Wrapper* Make_Unique ();                          \
    bool operator== (Am_Wrapper& test_value);            \
    Am_ID_Tag ID () const                                \
    { return id; }                                       \
    static Type_name##_Data* Narrow (Am_Wrapper* value); \
    static Am_ID_Tag Type_name##_Data_ID ()              \
    { return id; }                                       \
   private:                                              \
    static Am_ID_Tag id;                           

#define Am_WRAPPER_DATA_IMPL_NO_ID(Type_name, create_args)     \
Type_name##_Data* Type_name##_Data::Narrow (Am_Wrapper* value) \
{                                                              \
  if (value && (value->ID () == id))                           \
    return (Type_name##_Data*)value;                           \
  else                                                         \
    return NULL;                                               \
}                                                              \
bool Type_name##_Data::operator== (Am_Wrapper& test_value)     \
{                                                              \
  if (id == test_value.ID ())                                  \
    return (&test_value == this) ||                            \
           (this->operator== ((Type_name##_Data&)test_value)); \
  else                                                         \
    return false;                                              \
}                                                              \
Am_Wrapper* Type_name##_Data::Make_Unique ()                   \
{                                                              \
  if (Is_Unique ())                                            \
    return this;                                               \
  else {                                                       \
    Release ();                                                \
    return new Type_name##_Data create_args;                   \
  }                                                            \
}

#define Am_WRAPPER_DATA_IMPL(Type_name, create_args) \
Am_WRAPPER_DATA_IMPL_NO_ID (Type_name, create_args)  \
Am_ID_Tag Type_name##_Data::id = Am_Get_Unique_ID_Tag (#Type_name, \
                                              Am_WRAPPER_TYPE);

#define Am_WRAPPER_DATA_IMPL_ID(Type_name, create_args, in_id) \
Am_WRAPPER_DATA_IMPL_NO_ID (Type_name, create_args)            \
Am_ID_Tag Type_name##_Data::id = Am_Get_Unique_ID_Tag (#Type_name, in_id);

#define Am_WRAPPER_DECL(Type_name)                    \
 public:                                              \
  Type_name (const Type_name&);                       \
  Type_name (const Am_Value&);                        \
  Type_name (Type_name##_Data* in_data)               \
  { data = in_data; }                                 \
  ~Type_name ();                                      \
  Type_name& operator= (const Type_name&);            \
  Type_name& operator= (const Am_Value&);             \
  Type_name& operator= (Type_name##_Data* in_data);   \
  operator Am_Wrapper* () const;                      \
  bool Valid () const;                                \
  static Type_name Narrow (Am_Wrapper*);              \
  static bool Test (const Am_Wrapper*);               \
  static bool Test (const Am_Value& in_value);        \
  static Am_Value_Type Type_ID ();                    \
  const char * To_String() const;                     \
  Am_Value From_String (const char* string);          \
  void Print(ostream& out) const;                     \
  void Println() const;				      \
 protected:                                           \
  Type_name##_Data* data;

#define Am_WRAPPER_IMPL(Type_name)                                  \
Type_name::Type_name (const Type_name& prev)                        \
{                                                                   \
  data = prev.data;                                                 \
  if (data)                                                         \
    data->Note_Reference ();                                        \
}                                                                   \
Type_name::Type_name (const Am_Value& in_value)                     \
{                                                                   \
  data = (Type_name##_Data*)in_value.value.wrapper_value;           \
  if (data) {                                                       \
    if (Type_name##_Data::Type_name##_Data_ID () != in_value.type) {  \
        cerr << "** Tried to set a " #Type_name " with a non "      \
                 #Type_name " wrapper: " << in_value << endl;       \
	Am_Error();						    \
      }							            \
    data->Note_Reference ();                                        \
  }                                                                 \
}                                                                   \
Type_name::~Type_name ()                                            \
{                                                                   \
  if (data) {                                                       \
    if (data->Is_Zero ())                                           \
      Am_Error ("** Tried to delete a " #Type_name " twice.");                                                  \
    data->Release ();                                               \
  }                                                                 \
  data = NULL;                                                      \
}                                                                   \
Type_name& Type_name::operator= (const Type_name& prev)             \
{                                                                   \
  Type_name##_Data* old_data = data;                                \
  data = prev.data;                                                 \
  if (data)                                                         \
    data->Note_Reference ();                                        \
  if (old_data)                                                     \
    old_data->Release ();                                           \
  return *this;                                                     \
}                                                                   \
Type_name& Type_name::operator= (const Am_Value& in_value)          \
{                                                                   \
  Type_name##_Data* old_data = data;                                \
  data = (Type_name##_Data*)in_value.value.wrapper_value;           \
  if (data) {                                                       \
    if (in_value.type != Type_name##_Data::Type_name##_Data_ID () &&\
        in_value.type != Am_ZERO) {  				    \
        cerr << "** Tried to set a " #Type_name " with a non "      \
                 #Type_name " wrapper: " << in_value << endl;       \
	Am_Error();						    \
      }							            \
    data->Note_Reference ();                                        \
  }                                                                 \
  if (old_data)                                                     \
    old_data->Release ();                                           \
  return *this;                                                     \
}                                                                   \
Type_name& Type_name::operator= (Type_name##_Data* in_data)         \
{                                                                   \
  if (data) data->Release ();                                       \
  data = in_data;                                                   \
  return *this;                                                     \
}                                                                   \
Type_name::operator Am_Wrapper* () const                            \
{                                                                   \
  if (data)                                                         \
    data->Note_Reference ();                                        \
  return data;                                                      \
}                                                                   \
bool Type_name::Valid () const                                      \
{                                                                   \
  return data != NULL;                                              \
}                                                                   \
Type_name Type_name::Narrow (Am_Wrapper* in_data)                   \
{                                                                   \
  if (in_data) {                                                    \
    if (Type_name##_Data::Type_name##_Data_ID () == in_data->ID ()) \
      return (Type_name##_Data*)in_data;                            \
    else                                                            \
      Am_Error ("** Tried to set a " #Type_name " with a non "      \
                #Type_name " wrapper.");                            \
  }                                                                 \
  return (Type_name##_Data*)NULL;                                   \
}                                                                   \
bool Type_name::Test (const Am_Wrapper* in_data)                    \
{                                                                   \
  return (in_data &&                                                \
          (in_data->ID () ==                                        \
           Type_name##_Data::Type_name##_Data_ID ()));              \
}								    \
bool Type_name::Test (const Am_Value& in_value)                     \
{                                                                   \
  return (in_value.value.wrapper_value &&                           \
          (in_value.type ==                                         \
           Type_name##_Data::Type_name##_Data_ID ()));              \
}								    \
Am_Value_Type Type_name::Type_ID ()                                 \
{								    \
  return Type_name##_Data::Type_name##_Data_ID ();                  \
}                                                                   \
const char * Type_name::To_String() const                           \
{                                                                   \
  if (data) return data->To_String();                               \
  else return NULL;                                                 \
}                                                                   \
Am_Value Type_name::From_String (const char* string)                \
{                                                                   \
  if (data) return data->From_String(string);                       \
  else return Am_No_Value;                                          \
}                                                                   \
void Type_name::Print(ostream& out) const                           \
{                                                                   \
  if (data) data->Print(out);                                       \
  else out << "("  #Type_name ")NULL";       	   		    \
}                                                                   \
void Type_name::Println() const              		            \
{                                                                   \
   Print(cout);                                                     \
   cout << endl << flush;                                           \
 }

// A simple string class Used to store retrieve strings from Am_Values and
// similar places.  Uses wrapper reference counting for automatic deallocation.
class Am_String_Data;

class Am_String {
  Am_WRAPPER_DECL (Am_String)
 public:
  Am_String ()
  { data = NULL; }
  Am_String (const char* string, bool copy = true);

  Am_String& operator= (const char* string);

  operator const char* () const;
  operator char* ();

  bool operator== (const Am_String& test_string) const;
  bool operator!= (const Am_String& test_string) const
     { return !operator==(test_string); }
  bool operator== (const char* test_string) const;
  bool operator!= (const char* test_string) const
     { return !operator==(test_string); }
#if defined(_WINDOWS)
  bool operator== (char* test_string) const
     { return operator== ((const char*)test_string); }
  bool operator!= (char* test_string) const
     { return !operator==(test_string); }
#endif
};

// The NULL string.
extern Am_String Am_No_String;

extern ostream& operator<< (ostream& os, const Am_String& string);

/* for iterators */
enum Am_Insert_Position { Am_BEFORE, Am_AFTER };
enum Am_Add_Position { Am_HEAD, Am_TAIL };

///////////////////////////////
// Loading and Saving Values //
///////////////////////////////

class Am_Load_Save_Context; // forward reference

// Method used to load a value from a stream.  Use the context parameter to
// load nested items from the same stream.
Am_Define_Method_Type (Am_Load_Method, Am_Value,
		       (istream& is, Am_Load_Save_Context& context))
// Method used to save a value into a stream.  Use the context parameter to
// save nested items from the same stream.
Am_Define_Method_Type (Am_Save_Method, void,
		       (ostream& os, Am_Load_Save_Context& context,
			const Am_Value& value))

extern Am_Load_Method Am_No_Load_Method;
extern Am_Save_Method Am_No_Save_Method;

class Am_Load_Save_Context_Data; // private class

// This class holds the state of load or save in process.  This will keep
// track of the names of items loaded and will handle references to objects
// previously loaded or saved.
class Am_Load_Save_Context {
  Am_WRAPPER_DECL (Am_Load_Save_Context)
 public:
  Am_Load_Save_Context ()
  { data = NULL; }

  // This method is used to record items that are referred to by the objects
  // being loaded or saved, but the items themselves are permanent parts of
  // the application hence they shouldn't (or possibly can't) be saved as
  // well.  By providing a base number one can use the same name over and over.
  void Register_Prototype (const char* name, Am_Wrapper* value);
  void Register_Prototype (const char* name, unsigned base, Am_Wrapper* value);
  //returns the name if the value is registered as a prototype.  If
  //not registered, returns NULL
  const char* Is_Registered_Prototype (Am_Wrapper* value);
  // Load methods are registered based on a string type name.  The type name
  // must be a single alphanumeric word (no spaces).  The loader is responsible
  // for returning a value for anything stream that is declared with that type
  // name.  If two or more methods are registered on the same name, the last
  // one defined will be used.
  void Register_Loader (const char* type_name, const Am_Load_Method& method);
  // Save methods are registered based on a value type.  If a single type can
  // be saved in multiple ways, it is up to the save method to delagate the
  // save to the proper method.  If two or more methods are registered on the
  // same type, the last one defined will be used.
  void Register_Saver (Am_ID_Tag type, const Am_Save_Method& method);

  // Reset internal counters and references.  Used to start a new load or save
  // session.  Should be called once per new stream before the first call to
  // Load or Save.  Will not remove registered items such as loaders, savers,
  // and prototypes.
  void Reset ();

  // Load value from stream.  Call this once for each value saved.  Returns
  // Am_No_Value when stream is empty.
  Am_Value Load (istream& is);
  // Save value to stream.  Call once for each value saved.
  void Save (ostream& os, const Am_Value& value);

  // Structures that are recursive must call this function before Load is
  // called recusively.  The value is the pointer to the structure being
  // loaded.  This value will be used by internal values that refer to the
  // parent structure.
  void Recursive_Load_Ahead (Am_Wrapper* value);
  // This procedure must be called by each save method as the first thing it
  // writes to the stream.  The procedure will put the name into the stream
  // with the proper format.  After that, the save method can do whatever it
  // needs.
  void Save_Type_Name (ostream& os, const char* type_name);
};

extern Am_Load_Save_Context Am_No_Load_Save_Context;

#endif
