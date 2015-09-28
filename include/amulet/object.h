/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifndef OBJECT_H
#define OBJECT_H

#include <am_inc.h>

#include TYPES__H

// Am_Slot_Key is used to name the slots of objects.
typedef unsigned short Am_Slot_Key;

// object system reserves slots 0-99
#define Am_NO_SLOT        0
#define Am_NO_INHERIT     2
//do not use 1 since true==1 as slot name has a special meaning for interactors
#define Am_OWNER          10
#define Am_PROTOTYPE      11
#define Am_SOURCE_OF_COPY 12

extern void Am_Print_Key (ostream& os, Am_Slot_Key key);

// Am_Constraint represents a constraint object.  Its type is opaque in this
// context.  The file formula.h declares the standard amulet factory for
// creating constraints.  Am_Constraint itself is defined in object_advanced.h
class Am_Constraint;

// The Am_Constraint_Context is an opaque type used to represent a constraint's
// local state when Am_Object methods are called within a constraint formula.
class Am_Constraint_Context;

/////////////////////////////////////////////////////////////////////////////
// Main Object code
/////////////////////////////////////////////////////////////////////////////

enum Am_Inherit_Rule { Am_LOCAL, Am_INHERIT, Am_COPY, Am_STATIC };

typedef unsigned long Am_Slot_Flags;
#define Am_OK_IF_NOT_THERE 	0x01 //use with Set
#define Am_RETURN_ZERO_ON_ERROR 0x03 // has Am_OK_IF_NOT_THERE set also
#define Am_NO_DEPENDENCY 	0x04
#define Am_OK_IF_THERE		0x08 //use with Add
#define Am_OVERRIDE_READ_ONLY	0x10
//additional flags Am_KEEP_FORMULAS in formula.h and
//  Am_NO_ANIMATION, Am_WITH_ANIMATION and Am_DONT_TELL_ANIMATORS in inter.h

// To ease this transition, if flag is defined, all the Set
// errors will simply type out a warning and continue instead of crashing.
// Uncomment to get warnings.  Comment out to get errors
//#define AMULET2_WARNINGS true

class Am_Object_Data;

// The Am_Object class.  This is used to represent Amulet objects like
// windows, graphical objects, and interators.
class Am_Object {
  Am_WRAPPER_DECL (Am_Object)
 public:
  Am_Object ()
  { data = NULL; }

  // Creates an instance of the object.  The name of the new instance is
  // given by the parameter.  See the documentation for a description of
  // instantiation.
  Am_Object Create (const char* new_name = NULL) const;

  // Makes a copy of the object as though an instance was created of the
  // object's prototype.  The slot values, and parts of the source are copied
  // as identically as possible.
  Am_Object Copy (const char* new_name = NULL) const;

  // Like Copy except all slots that have constraints or are otherwise
  // actively determined are made into simple values.  The constraints are
  // completely stripped out.
  Am_Object Copy_Value_Only (const char* new_name = NULL) const;

  // Test whether object is same as another.
  bool operator== (const Am_Object& test_object) const;
  bool operator!= (const Am_Object& test_object) const;
  
  // Returns whether the prototype parameter is in the prototype chain of the
  // object.  Will return true if the parameter is the object itself.
  bool Is_Instance_Of (Am_Object prototype) const;

  // Answers the question is the object a part of the object given in the
  // owner parameter.  An object is considered to be part of itself.
  bool Is_Part_Of (Am_Object owner) const;

  // Get the value of a slot.  Peek is a safe Get; when an error occurs,
  // it returns an error value rather than crashing with a fatal error
  const Am_Value& Get (Am_Slot_Key key, Am_Slot_Flags flags = 0) const;
  const Am_Value& Peek (Am_Slot_Key key, Am_Slot_Flags flags = 0) const
  { return Get(key, flags | Am_OK_IF_NOT_THERE); }
  
  // Prints all the slots of the object and their values for debugging to cout
  void Text_Inspect() const;
  // Prints one slot of the object and its values for debugging to cout
  void Text_Inspect (Am_Slot_Key key) const;

  // Get and set the object's name in the global name registry
  Am_Object& Set_Name (const char* name);
  const char* Get_Name () const;

  // Returns an object's key if the object is a named part of another object.
  // Returns Am_NO_SLOT when the object is not a part or is an unnamed part.
  // Returns Am_NO_INHERIT which object is an unnamed part that doesn't
  // get inherited when owner is instantiated.
  Am_Slot_Key Get_Key () const;

  // Return the object  from which this object was instanced.
  Am_Object Get_Prototype () const;

  // Queries a slot for an object.  This causes an error if the slot containts
  // a non-object value.
  Am_Object Get_Object (Am_Slot_Key key, Am_Slot_Flags flags = 0) const;
  Am_Object Peek_Object (Am_Slot_Key key, Am_Slot_Flags flags = 0) const
  { return Get_Object (key, flags | Am_OK_IF_NOT_THERE); }

  // Returns the object's owner.  Returns NULL if the object does not have an
  // owner.
  Am_Object Get_Owner (Am_Slot_Flags flags = 0) const
  { return Get_Object (Am_OWNER, flags); }

  // Find a named part of the owner of an object.
  Am_Object Get_Sibling (Am_Slot_Key key, Am_Slot_Flags flags = 0) const
  { return Get_Owner (flags).Get_Object(key, flags); }
  Am_Object Peek_Sibling (Am_Slot_Key key, Am_Slot_Flags flags = 0) const
  { return Get_Owner(flags).Get_Object(key, flags | Am_OK_IF_NOT_THERE); }

  // Returns the type of the value stored in a slot.  If the slot does not
  // exist, Am_NONE is returned.
  Am_Value_Type Get_Slot_Type (Am_Slot_Key key, Am_Slot_Flags flags = 0) const
  { return Get(key, flags | Am_OK_IF_NOT_THERE).type; }

  // Returns true if slot is inherited from some prototype.
  bool Is_Slot_Inherited (Am_Slot_Key key) const;
  
  // Make a slot independent from all other slots.
  void Make_Unique (Am_Slot_Key key);
  
  // Returns whether value of slot is independent from all other slots.
  bool Is_Unique (Am_Slot_Key key);

#ifdef AMULET2_INSTRUMENT
#undef Set
#undef Add
#undef Set_Part
#undef Add_Part
#endif
  
  // Set the value of an existing slot.
  Am_Object& Set (Am_Slot_Key key, Am_Wrapper* value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, Am_Ptr value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, int value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, long value, Am_Slot_Flags flags = 0);
#if !defined(NEED_BOOL)
  Am_Object& Set (Am_Slot_Key key, bool value, Am_Slot_Flags flags = 0);
#endif
  Am_Object& Set (Am_Slot_Key key, float value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, double value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, char value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, const char* value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, const Am_String& value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, Am_Generic_Procedure* value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, Am_Method_Wrapper* value, Am_Slot_Flags flags = 0);
  Am_Object& Set (Am_Slot_Key key, const Am_Value& value, Am_Slot_Flags flags = 0);
  // Set a constraint into the slot.
  Am_Object& Set (Am_Slot_Key key, Am_Constraint* constraint,
		  Am_Slot_Flags flags = 0);

  // Add a new slot.
  Am_Object& Add (Am_Slot_Key key, Am_Wrapper* value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, Am_Ptr value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, int value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, long value, Am_Slot_Flags flags = 0);
#if !defined(NEED_BOOL)
  Am_Object& Add (Am_Slot_Key key, bool value, Am_Slot_Flags flags = 0);
#endif
  Am_Object& Add (Am_Slot_Key key, float value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, double value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, char value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, const char* value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, const Am_String& value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, Am_Generic_Procedure* value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, Am_Method_Wrapper* value, Am_Slot_Flags flags = 0);
  Am_Object& Add (Am_Slot_Key key, const Am_Value& value, Am_Slot_Flags flags = 0);
  // Add a new slot containing a constraint to the slot.
  Am_Object& Add (Am_Slot_Key key, Am_Constraint* constraint,
		  Am_Slot_Flags flags = 0);

  
  // Note_Changed allows one to cause a slot to act as though its value has
  // been changed even if the actual value stored has not changed.  This
  // function is especially useful for slot changes due to side effects.
  void Note_Changed (Am_Slot_Key key);

  void Note_Unchanged (Am_Slot_Key key);
  

  // Adds a part to an object.  The part either be named by providing a key or
  // unnamed.
  Am_Object& Add_Part (Am_Object new_part, bool inherit = true);
  //slot must not already exist (unless Am_OK_IF_THERE flag).
  //slot must never be a regular slot
  Am_Object& Add_Part (Am_Slot_Key key, Am_Object new_part,
		       Am_Slot_Flags set_flags = 0);
  
  //Slot must exist (unless Am_OK_IF_THERE), and must already be a part slot
  Am_Object& Set_Part (Am_Slot_Key key, Am_Object new_part,
		       Am_Slot_Flags set_flags = 0);

#ifdef AMULET2_INSTRUMENT
  Am_Instrumentation(Am_Object)

#define Set Am_Instrumented(Set)
#define Add Am_Instrumented(Add)
#define Set_Part Am_Instrumented(Set_Part)
#define Add_Part Am_Instrumented(Add_Part)
#endif

  //is that slot a part slot?  Also returns false if slot isn't there.
  bool Is_Part_Slot(Am_Slot_Key key) const;

  // Destroys the slot.  Returns the original object so Remove_Slot can be put
  // into a chain of sets and add_parts, etc.
  Am_Object& Remove_Slot (Am_Slot_Key key);
  
  // Removes a part from an object.  The part can be named either by a key or
  // by the actual part object.  Returns the original object so Remove_Slot
  // can be put into a chain of sets and add_parts, etc.
  Am_Object&  Remove_Part (Am_Slot_Key key);
  Am_Object&  Remove_Part (Am_Object part);

  // Make the object no longer be a part.
  void Remove_From_Owner ();

  // Removes all locally defined constraints from a slot.
  void Remove_Constraint (Am_Slot_Key key);

  // Destroy the object and all its parts.
  void Destroy ();

  // Get and Set "Advanced" properties of slots
  Am_Object& Set_Demon_Bits (Am_Slot_Key key, unsigned short bits);
  unsigned short Get_Demon_Bits (Am_Slot_Key key) const;

  Am_Object& Set_Inherit_Rule (Am_Slot_Key key, Am_Inherit_Rule rule);
  Am_Inherit_Rule Get_Inherit_Rule (Am_Slot_Key key) const;
  
  Am_Object& Set_Type_Check (Am_Slot_Key key, unsigned short type);
  unsigned short Get_Type_Check (Am_Slot_Key key) const;

  Am_Object& Set_Read_Only (Am_Slot_Key key, bool read_only);
  bool Get_Read_Only (Am_Slot_Key key) const;

  // Runs all demons and validates all constraints on this object
  // (and on any other objects that happen to share the same demon queue) 
  Am_Object& Validate ();

protected:
  static Am_Constraint_Context* cc;
};

// The NULL object.
extern Am_Object Am_No_Object;

// Create a Type ID for Am_Object.
const Am_Value_Type Am_OBJECT = Am_WRAPPER_TYPE | 1;

// This is a helper class used for iterating through an object's instances.
// To initialize the iterator, assign the prototype object to it, or assign
// the object when the iterator gets created.
class Am_Instance_Iterator {
 public:
  Am_Instance_Iterator ();
  Am_Instance_Iterator (Am_Object object);
  
  Am_Instance_Iterator& operator= (Am_Object object);
  
  unsigned short Length ();   // Number of instances in the list.
  void Start ();                  // Begin list at the start.
  void Next ();                   // Move to next element in list.
  bool Last ();                   // Is this the last element?
  Am_Object Get ();              // Get the current element.
 private:
  Am_Object current;
  Am_Object prototype;
};

class Am_Slot_Iterator_Data;

// This is a helper class used for iterating through the slots of an object.
// The iterator is initialized by setting it with the object whose slots are
// to be examined.  The elements of the list are actually slot keys and not
// actual slots.  This iterator will list all keys that can be accessed from
// the object including those defined in prototype objects.
class Am_Slot_Iterator {
 public:
  Am_Slot_Iterator ();
  Am_Slot_Iterator (Am_Object object);
  ~Am_Slot_Iterator ();
  
  Am_Slot_Iterator& operator= (Am_Object object);
  
  unsigned short Length ();       // Number of slots in the list.
  void Start ();                  // Begin list at the start.
  void Next ();                   // Move to next element in list.
  bool Last ();                   // Is this the last element?
  Am_Slot_Key Get ();             // Get the current element.
 private:
  Am_Slot_Iterator_Data* data;
  Am_Object context;
};

// This is a helper class used for iterating through an object's parts.
// The iterator is inialized by assigning the owner object to the iterator.
// The elements of the list will include both named and unnamed parts.
class Am_Part_Iterator {
 public:
  Am_Part_Iterator ();
  Am_Part_Iterator (Am_Object object);
  
  Am_Part_Iterator& operator= (Am_Object object);
  
  unsigned short Length ();       // Number of parts in the list.
  void Start ();                  // Begin list at the start.
  void Next ();                   // Move to next element in list.
  bool Last ();                   // Is this the last element?
  Am_Object Get ();               // Get the current element.
 private:
  Am_Object current;
  Am_Object owner;
};

// Prints out an identifying name for the object to the output stream.
extern ostream& operator<< (ostream& os, const Am_Object& object);

// The root object.  This object is instanced to create other objects.  The
// Am_Root_Object has no slots and no parts.
extern Am_Object Am_Root_Object;

// Popular method type:
Am_Define_Method_Type (Am_Object_Method, void, (Am_Object))
  
#ifdef AMULET2_GV
//
// GV macros
//

/**/
// Macros that automatically append the cc parameter for constraint Gets and
// Sets.
#define GV(s1) Get (s1)
#define GV_Object(s1) Get_Object (s1)
#define GV_Owner() Get_Owner ()
#define GV_Sibling(s1) Get_Sibling (s1)

/**/
#define GVF(s1,gvflags) Get (s1, gvflags)
#define GVF_Object(s1, gvflags) Get_Object (s1, gvflags)
#define PV(s1) Peek(s1)

#define SV(s1, val) Set (s1, val)

#endif // AMULET2_GV

//These versions pop up the inspector on the specified object and slot

extern void (*Am_Object_Debugger) (const Am_Object &obj, Am_Slot_Key slot);

extern void Am_Error (const char* error_string, const Am_Object& obj,
		      Am_Slot_Key slot = Am_NO_SLOT);
extern void Am_Error (const Am_Object& obj, Am_Slot_Key slot = Am_NO_SLOT);

#ifdef DEBUG
#define Am_ERRORO(error_string, obj, slot) 		\
{ cerr << "** Amulet_Error: " << error_string << endl;  \
  Am_Error(obj, slot);					\
}
#else
#define Am_ERRORO(error_string, obj, slot) 		\
{ cerr << "** Amulet_Error: " << error_string << endl;  \
  Am_Error();						\
}
#endif

#endif
