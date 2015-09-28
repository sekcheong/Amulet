/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifndef OBJECT_ADVANCED_H
#define OBJECT_ADVANCED_H

#include OBJECT__H

#define Am_RETURN_ZERO_ON_ERROR_BIT 0x02

class Am_Slot;

// The demon types.
// Object Demon - connected to objects.
// Slot Demon - connected to slots.
// Part Demon - connected to objects, but used when parts are added/removed.
// Type Check - connected to slots, used for testing types.
typedef void Am_Object_Demon (Am_Object object);
typedef void Am_Slot_Demon (Am_Slot first_invalidated);
typedef void Am_Part_Demon (Am_Object owner, Am_Object old_object,
                            Am_Object new_object);
typedef const char* Am_Type_Check (const Am_Value& value);

// Opaque type for Am_Demon_Queue.
class Am_Demon_Queue_Data;

// The demon queue.
// Demons are not called immediately.  When the demon's event occurs, the
// demon procedure is stored on the demon queue.  When Am_Object::Get is
// called, each procedure on the demon queue is invoked in order.
class Am_Demon_Queue {
 public:
  // Store a new demon procedure in the queue.
  void Enqueue (Am_Slot_Demon* demon, unsigned short which_bit,
		const Am_Slot& param);

  // Remove demon procedures from the queue.
  void Delete (const Am_Object& object);
    // Delete removes slot demons demons that correspond to a given
    // object.  A slot demon corresponds to an object of which it is part.
  void Delete (const Am_Slot& slot);

  // Tests whether queue has members.
  bool Empty () const;
  // Iterates through queue invoking each demon.  This will use Prevent_Invoke
  // to prevent the queue from being invoked recursively.
  void Invoke ();

  // Prevents the queue from being invoked.  When the queue is being prevented,
  // calling Invoke is a no-op (not an error).  These methods use ref counting
  // calling Prevent_Invoke multiple times makes it neccessary to call
  // Release_Invoke the same number to release the queue.  (It does not go
  // negative, though.)
  void Prevent_Invoke ();
  void Release_Invoke ();

  // Renders the queue inoperable.  This is used during clean-up to prevent
  // the queue from hampering destroying objects.
  void Shutdown ();

  // Reinitializes the queue to its empty state.
  //  Used when invoking the inspector.
  void Reset();

  // Make a copy of a given queue.
  Am_Demon_Queue& operator= (const Am_Demon_Queue& proto);

  // Destructor.
  ~Am_Demon_Queue ();

  // Create a new, empty queue.
  Am_Demon_Queue ();
  Am_Demon_Queue (const Am_Demon_Queue& proto);
  Am_Demon_Queue (Am_Demon_Queue_Data* in_data)
  { data = in_data; }

  operator Am_Demon_Queue_Data* () const
  { return data; }

 private:
  Am_Demon_Queue_Data* data;
};

typedef unsigned short Am_Demon_Protocol;
#define Am_DEMON_PER_SLOT      0x0000
#define Am_DEMON_PER_OBJECT    0x0001
#define Am_DEMON_ON_INVALIDATE 0x0002
#define Am_DEMON_ON_CHANGE     0x0004

enum Am_Object_Demon_Type { Am_CREATE_OBJ, Am_COPY_OBJ, Am_DESTROY_OBJ };
enum Am_Part_Demon_Type { Am_ADD_PART, Am_CHANGE_OWNER };

class Am_Demon_Set_Data;

class Am_Demon_Set {
 public:
  Am_Object_Demon* Get_Object_Demon (Am_Object_Demon_Type type) const;
  void Set_Object_Demon (Am_Object_Demon_Type type,
			 Am_Object_Demon* demon);
  Am_Slot_Demon* Get_Slot_Demon (unsigned short which_bit) const;
  void Set_Slot_Demon (unsigned short which_bit, Am_Slot_Demon* method,
		       Am_Demon_Protocol protocol);
  Am_Part_Demon* Get_Part_Demon (Am_Part_Demon_Type type) const;
  void Set_Part_Demon (Am_Part_Demon_Type type, Am_Part_Demon* demon);
  Am_Type_Check* Get_Type_Check (unsigned short type) const;
  void Set_Type_Check (unsigned short type, Am_Type_Check* demon);

  Am_Demon_Set Copy () const;
  Am_Demon_Set& operator= (const Am_Demon_Set& proto);

  ~Am_Demon_Set ();

  Am_Demon_Set ();
  Am_Demon_Set (const Am_Demon_Set& proto);
  Am_Demon_Set (Am_Demon_Set_Data* in_data)
  { data = in_data; }

  operator Am_Demon_Set_Data* () const
  { return data; }

 private:
  Am_Demon_Set_Data* data;
};

class Am_Object_Context;

class Am_Constraint : public Am_Registered_Type {
 public:
  virtual bool Get (const Am_Slot& fetching_slot, Am_Value& value,
                    bool& changed) = 0;

  virtual void Invalidated (const Am_Slot& slot_invalidated,
			    Am_Constraint* invalidating_constraint,
                            const Am_Value& value) = 0;
  virtual void Changed (const Am_Slot& slot_changed,
			Am_Constraint* changing_constraint,
			const Am_Value& prev_value,
                        const Am_Value& new_value) = 0;
  virtual void Changed (const Am_Slot& slot_changed,
			Am_Constraint* changing_constraint) = 0;
  virtual void Slot_Event (Am_Object_Context* oc, const Am_Slot& slot) = 0;

  virtual Am_Constraint* Get_Prototype () = 0;
  virtual bool Is_Instance_Of (Am_Constraint* proto) = 0;

  virtual Am_Constraint* Constraint_Added (const Am_Slot& adding_slot) = 0;
  virtual Am_Constraint* Dependency_Added (const Am_Slot& adding_slot) = 0;
  virtual void Constraint_Removed (const Am_Slot& removing_slot) = 0;
  virtual void Dependency_Removed (const Am_Slot& removing_slot) = 0;

  virtual bool Is_Overridden_By (const Am_Slot& slot,
				 Am_Constraint *competing_constraint) = 0;

  virtual Am_Constraint* Create (const Am_Slot& current_slot,
				 const Am_Slot& new_slot) = 0;
  virtual Am_Constraint* Copy (const Am_Slot& current_slot,
			       const Am_Slot& new_slot) = 0;
  virtual Am_ID_Tag ID () const = 0;
  virtual const char *Get_Name() = 0;
};

// Create a class for constraint ids.
const Am_Value_Type Am_CONSTRAINT = 0x3000;

typedef void* Am_Constraint_Tag;
typedef unsigned Am_Set_Reason;

#ifdef DEBUG

#define Am_TRACE_NOT_SPECIFIED           0x00
#define Am_TRACE_SLOT_SET                0x01
#define Am_TRACE_SLOT_CHANGED            0x02
#define Am_TRACE_CONSTRAINT_FETCH        0x03
#define Am_TRACE_SLOT_DESTROY            0x04
#define Am_TRACE_INHERITANCE_PROPAGATION 0x80

typedef void Am_Slot_Set_Trace_Proc (const Am_Slot& slot,
                                     Am_Set_Reason reason);

extern Am_Slot_Set_Trace_Proc* Am_Global_Slot_Trace_Proc;

#endif

class Am_Object_Advanced;

class Am_Slot_Data;
  
class Am_Slot {
 public:
#ifdef AMULET2_INSTRUMENT
#undef Set
#endif

  void Set (const Am_Value& new_value, Am_Constraint* cause);
  void Set_Current_Constraint (Am_Constraint* constraint);

#ifdef AMULET2_INSTRUMENT
  Am_Instrumentation(Am_Slot)

#define Set Am_Instrumented(Set)
#endif

  // Get the owner of the slot.
  Am_Object_Advanced Get_Owner () const;
  // Get the name of the slot.
  Am_Slot_Key Get_Key () const;
  // Get the type of the slot.
  Am_Value_Type Get_Type () const;

  // Get the value of the slot.
  const Am_Value& Get () const;

  // Slot messages.
  void Invalidate (Am_Constraint* validating_constraint);
  void Event (Am_Object_Context* oc);
  void Validate ();
  void Change (Am_Constraint* changing_constraint);

  Am_Constraint_Tag Add_Constraint (Am_Constraint* new_constraint) const;
  void Remove_Constraint (Am_Constraint_Tag constraint_tag) const;
  Am_Constraint_Tag Add_Dependency (Am_Constraint* new_dependency) const;
  void Remove_Dependency (Am_Constraint_Tag dependency_tag) const;

  // Returns whether or not slot's value is inherited.
  bool Is_Inherited ();
  void Make_Unique ();
  bool Valid() { return data != NULL; }

  unsigned short Get_Demon_Bits () const;
  void Set_Demon_Bits (unsigned short bits);

  Am_Inherit_Rule Get_Inherit_Rule () const;
  void Set_Inherit_Rule (Am_Inherit_Rule rule);

  unsigned short Get_Type_Check () const;
  void Set_Type_Check (unsigned short type);

  bool Get_Read_Only () const;
  void Set_Read_Only (bool read_only);

  void Text_Inspect() const;

  Am_Slot ()
  { data = NULL; }
  Am_Slot (Am_Slot_Data* in_data)
  { data = in_data; }

  operator== (const Am_Slot& test) const
  { return data == test.data; }
  operator!= (const Am_Slot& test) const
  { return data != test.data; }
  operator Am_Slot_Data* () const
  { return data; }

 private:
  Am_Slot_Data* data;
};

class Am_Constraint_Iterator_Data;

class Am_Constraint_Iterator {
 public:
  Am_Constraint_Iterator ();
  Am_Constraint_Iterator (const Am_Slot& slot);
  
  Am_Constraint_Iterator& operator= (const Am_Slot& slot);
  
  unsigned short Length () const;
  void Start ();
  void Next ();
  bool Last () const;
  Am_Constraint* Get () const;
  Am_Constraint_Tag Get_Tag () const;
  
 private:
  Am_Slot context;
  Am_Constraint_Iterator_Data* current;
};

class Am_Dependency_Iterator_Data;

class Am_Dependency_Iterator {
 public:
  Am_Dependency_Iterator ();
  Am_Dependency_Iterator (const Am_Slot& slot);
  
  Am_Dependency_Iterator& operator= (const Am_Slot& slot);
  
  unsigned short Length () const;
  void Start ();
  void Next ();
  bool Last () const;
  Am_Constraint* Get () const;
  Am_Constraint_Tag Get_Tag () const;
  
 private:
  Am_Slot context;
  Am_Dependency_Iterator_Data* current;
};

class Am_Object_Advanced : public Am_Object {
 public:
  Am_Object_Advanced ();
  Am_Object_Advanced (Am_Object_Data* in_data);

  Am_Slot Get_Slot (Am_Slot_Key key) const;

  Am_Slot Get_Owner_Slot () const;
  Am_Slot Get_Part_Slot () const;

  // Returns the object in which a slot is defined.
  Am_Object_Advanced Get_Slot_Locale (Am_Slot_Key key) const;

  Am_Demon_Set Get_Demons () const;
  void Set_Demons (const Am_Demon_Set& methods);

  Am_Demon_Queue Get_Queue () const;
  void Set_Queue (const Am_Demon_Queue& queue);

  unsigned short Get_Default_Demon_Bits () const;
  void Set_Default_Demon_Bits (unsigned short bits);
  unsigned short Get_Demon_Mask () const;
  void Set_Demon_Mask (unsigned short mask);

  Am_Inherit_Rule Get_Default_Inherit_Rule () const;
  void Set_Default_Inherit_Rule (Am_Inherit_Rule rule);

  void Disinherit_Slot (Am_Slot_Key key);

  void Invoke_Demons (bool active);
  bool Demon_Invocation_Active () const;

  void Print_Name_And_Data (ostream& os) const;

  // expose your rep, darnit
  Am_Object_Data* Get_Data () const
  { return data; }

  static Am_Constraint_Context* Swap_Context (Am_Constraint_Context *new_cc) 
  {
    Am_Constraint_Context *old_cc = cc;
    cc = new_cc;
    return old_cc;
  }
  static Am_Constraint_Context* Get_Context ()
  { return cc; }
};

// Prints out an identifying name for the object to the output stream
// along with the data address 
extern ostream& operator<< (ostream& os, const Am_Object_Advanced& object);

class Am_Constraint_Context {
 public:
  virtual Am_ID_Tag ID () = 0;

  virtual const Am_Value& Get (const Am_Object_Advanced& object,
			       Am_Slot_Key key, Am_Slot_Flags flags) = 0;
  
#ifdef AMULET2_INSTRUMENT
#undef Set
#endif
  virtual void Set (const Am_Object_Advanced& object, Am_Slot_Key key,
                    const Am_Value& new_value, Am_Slot_Flags flags) = 0;
#ifdef AMULET2_INSTRUMENT
  Am_Instrumentation(Am_Constraint_Context)
#define Set Am_Instrumented(Set)
#endif

  virtual void Note_Changed (const Am_Object_Advanced& object, Am_Slot_Key key) = 0;
  virtual void Note_Unchanged (const Am_Object_Advanced& object,
                               Am_Slot_Key key) = 0;

  virtual const Am_Value& Raise_Get_Exception (const Am_Value& value,
			   const Am_Object_Advanced& object, Am_Slot_Key key,
			   Am_Slot_Flags flags, const char *msg) = 0;

  virtual Am_Wrapper* Get_Data () = 0;
  virtual void Set_Data (Am_Wrapper* data) = 0;
};

#define Am_PUSH_CC(cc) \
   { Am_Constraint_Context *old_cc = Am_Object_Advanced::Swap_Context (cc);

#define Am_POP_CC()  \
     Am_Object_Advanced::Swap_Context (old_cc); }

// An empty constraint context.  Gets and sets directly to the object.
extern Am_Constraint_Context* Am_Empty_Constraint_Context;

#define Am_PUSH_EMPTY_CC() Am_PUSH_CC(Am_Empty_Constraint_Context)
#define Am_POP_EMPTY_CC()  Am_POP_CC()

class Am_Explicit_Set : public Am_Constraint {
 public:
  bool Get (const Am_Slot& fetching_slot, Am_Value& value, bool& changed);
  
  void Invalidated (const Am_Slot& slot_invalidated,
		    Am_Constraint* invalidating_constraint,
		    const Am_Value& value);
  void Changed (const Am_Slot& slot_changed,
		Am_Constraint* changing_constraint,
		const Am_Value& prev_value, const Am_Value& new_value);
  void Changed (const Am_Slot& slot_changed,
		Am_Constraint* changing_constraint);
  void Slot_Event (Am_Object_Context* oc, const Am_Slot& slot);
  Am_Constraint* Get_Prototype ();
  bool Is_Instance_Of (Am_Constraint* proto);
  
  Am_Constraint* Constraint_Added (const Am_Slot& adding_slot);
  Am_Constraint* Dependency_Added (const Am_Slot& adding_slot);
  void Constraint_Removed (const Am_Slot& removing_slot);
  void Dependency_Removed (const Am_Slot& removing_slot);

  bool Is_Overridden_By (const Am_Slot& slot, 
			 Am_Constraint *competing_constraint);

  Am_Constraint* Create (const Am_Slot& current_slot,
			 const Am_Slot& new_slot);
  Am_Constraint* Copy (const Am_Slot& current_slot,
		       const Am_Slot& new_slot);
  Am_ID_Tag ID () const;
  const char* Get_Name();

  Am_Explicit_Set (Am_Slot_Flags in_flags) { flags = in_flags; }
  operator Am_Constraint* () { return this; }
  static bool Test (Am_Constraint* formula);
  static Am_Explicit_Set* Narrow (Am_Constraint* formula);

  static Am_ID_Tag id;

  Am_Slot_Flags flags;
};

extern void Ore_Initialize();

#endif
