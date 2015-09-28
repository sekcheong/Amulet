/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifndef IMPL_H
#define IMPL_H

#include <am_inc.h>

#include OBJECT_ADVANCED__H

#include "dynarray.h"

class CItem;

class QItem {
#ifdef MEMORY
 public:
  void* operator new (size_t)
  {
     return memory.New ();
  }
  void operator delete (void* ptr, size_t)
  {
    memory.Delete (ptr);
  }
  static Dyn_Memory_Manager memory;
#endif
 public:
  QItem (Am_Slot_Demon* demon, unsigned short which_bit,
         const Am_Slot& param);
  ~QItem ();

  Am_Slot_Demon* demon;
  Am_Slot_Data* param;
  unsigned short which_bit;
  QItem* next;
};

#define DEMONS_ACTIVE 0x8000

class Am_Demon_Queue_Data {
 public:
  Am_Demon_Queue_Data ()
  {
    refs = 1;
    head = NULL;
    tail = NULL;
    invoke_stack = 0;
    active = true;
  }
  ~Am_Demon_Queue_Data ()
  {
    QItem* curr;
    QItem* next;
    for (curr = head; curr; curr = next) {
      next = curr->next;
      delete curr;
    }
  }
  void Note_Reference ()
  { ++ refs; }
  void Release ()
  {
    if (!--refs)
      delete this;
  }
  void Enqueue (QItem* item)
  {
    item->next = NULL;
    if (tail)
      tail->next = item;
    else
      head = item;
    tail = item;
  }

  int refs;
  QItem* head;
  QItem* tail;
  unsigned short invoke_stack;
  bool active;
};

class Slot_Demon_Holder {
 public:
  Am_Slot_Demon* demon;
  Am_Demon_Protocol protocol;
  unsigned short which_bit;
};

class Type_Holder {
 public:
  Am_Type_Check* func;
};

class Am_Demon_Set_Data {
 public:
  Am_Demon_Set_Data ();
  Am_Demon_Set_Data (Am_Demon_Set_Data* proto)
  {
    refs = 1;
    create_demon = proto->create_demon;
    copy_demon = proto->copy_demon;
    destroy_demon = proto->destroy_demon;
    change_owner_demon = proto->change_owner_demon;
    add_part_demon = proto->add_part_demon;
    change_length = proto->change_length;
    invalid_length = proto->invalid_length;
    int i;
    for (i = 0; i < change_length; ++i)
      change_demons[i] = proto->change_demons[i];
    for (i = 0; i < invalid_length; ++i)
      invalid_demons[i] = proto->invalid_demons[i];
    max_type_check = proto->max_type_check;
    if (max_type_check) {
      type_check_list = new Type_Holder[max_type_check];
      for (i = 0; i < max_type_check; ++i)
        type_check_list[i] = proto->type_check_list[i];
    }
    else
      type_check_list = NULL;
  }
  ~Am_Demon_Set_Data ()
  {
    delete[] type_check_list;
  }
  void Note_Reference ()
  { ++ refs; }
  void Release ()
  {
    if (!--refs)
      delete this;
  }

  int refs;
  Am_Object_Demon* create_demon;
  Am_Object_Demon* copy_demon;
  Am_Object_Demon* destroy_demon;
  Am_Part_Demon* change_owner_demon;
  Am_Part_Demon* add_part_demon;
  unsigned short change_length;
  unsigned short invalid_length;
  Slot_Demon_Holder change_demons[5];
  Slot_Demon_Holder invalid_demons[5];
  unsigned short max_type_check;
  Type_Holder *type_check_list;
};

class Am_Object_Context {
 public:
  Am_Object_Context (bool in_inherited)
  { is_inherited = in_inherited; }
  bool is_inherited;
};

class am_CList {
 public:
  am_CList ();

  CItem* Add_Dep (Am_Constraint* item);
  Am_Constraint* Remove_Dep (CItem* item);
  CItem* Add_Con (Am_Constraint* item);
  Am_Constraint* Remove_Con (CItem* item);
  void Add_Inv (CItem* item);
  void Add_Update (CItem* item);
  void Remove_Inv (CItem* item);
  void Clear () { head = NULL; }
  CItem* Find (Am_Constraint* item);
  CItem* Pop ();
  void Validate (const Am_Slot& validating_slot);
  void Invalidate (const Am_Slot& slot_invalidated,
		   Am_Constraint* invalidating_constraint,
		   const Am_Value& value);
  void Change (const Am_Slot& slot_invalidated,
	       Am_Constraint* invalidating_constraint,
	       const Am_Value& prev_value, const Am_Value& new_value);
  void Change (const Am_Slot& slot_invalidated,
	       Am_Constraint* invalidating_constraint);
  void Slot_Event (Am_Object_Context* oc, const Am_Slot& slot);
  bool Empty ()
  { return !head; }

  void Remove_Any_Overridden_By (const Am_Slot& slot,
				 Am_Constraint* competing_constraint);

  void destroy (const Am_Slot& slot, bool constraint);

  CItem* head;
};

// "Is Inherited" means one or more instances of the slot may inherit this 
// slots value. "One of my instances might inherit my value."
#define BIT_IS_INHERITED      0x0001
// "Inherits" means the value of the slot is inherited from its 
// prototype.  "I inherit my prototype's value."
#define BIT_INHERITS          0x0002
#define BIT_IS_INVALID        0x0004
#define BIT_VALIDATING_NOW    0x0008
#define BIT_INVALIDATING_NOW  0x0010
#define BIT_IS_PART           0x0020
#define BIT_READ_ONLY         0x0040
#define BIT_SINGLE_CONSTRAINT 0x0080
#define DATA_BITS             0x00e0	// masks PART, READ_ONLY, SINGLE_CONSTRAINT

#ifdef AMULET2_INSTRUMENT
#undef Set
#endif

class Am_Slot_Data : public Am_Value {
#ifdef MEMORY
 public:
  void* operator new (size_t)
  {
     return memory.New ();
  }
  void operator delete (void* ptr, size_t)
  {
    memory.Delete (ptr);
  }
  static Dyn_Memory_Manager memory;
#endif
 public:
  Am_Slot_Data  (Am_Object_Data*, Am_Slot_Key);
  Am_Slot_Data  (Am_Object_Data*, Am_Slot_Key, Am_Value_Type);

  void Set (const Am_Value& new_value, Am_Slot_Flags set_flags);
  void Destroy ();

 public:
  Am_Object_Data* context;
  am_CList constraints;
  am_CList dependencies;
  am_CList invalid_constraints;
  Am_Slot_Key key;
  unsigned short enabled_demons;
  unsigned short queued_demons;
  unsigned short type_check;
#ifdef GCC
  unsigned short flags : 8;
  Am_Inherit_Rule rule : 2;
#else
  unsigned short flags;
  Am_Inherit_Rule rule;
#endif
};

class Am_Object_Data : public Am_Wrapper {
  Am_WRAPPER_DATA_DECL (Am_Object)
 public:
  void Print(ostream& out) const;
  Am_Object_Data ();
  Am_Object_Data (const char* schema_name, Am_Object_Data* in_prototype);
  ~Am_Object_Data ();

  operator== (Am_Object_Data&)
  { return false; }

  Am_Object_Data* create_object (const char* new_name);
  Am_Object_Data* copy_object (const char* new_name);
  Am_Object_Data* copy_object_value_only (const char* new_name);
  void invoke_create_demons ();
  void invoke_copy_demons ();
  void destroy_object ();
  void validate_object ();
  void note_parts ();
  void demon_removal ();

  Am_Slot_Data* find_slot (Am_Slot_Key key);
  Am_Slot_Data* find_prototype (Am_Slot_Key key);
  void find_slot_and_position (Am_Slot_Key key, Am_Slot_Data*& slot,
			       unsigned& i);
  void set_slot (Am_Slot_Key key, const Am_Value& value,
		 Am_Slot_Flags set_flags);
  void set_slot (Am_Slot_Key key, Am_Constraint* constraint,
		 Am_Slot_Flags set_flags);
  bool notify_change (Am_Slot_Key key, Am_Constraint* cause,
		      const Am_Value& old_value, const Am_Value& new_value);
  bool propagate_change (Am_Slot_Key key, Am_Constraint* cause,
			 const Am_Value& old_value, const Am_Value& new_value);
  bool notify_change (Am_Slot_Key key, Am_Constraint* cause);
  bool propagate_change (Am_Slot_Key key, Am_Constraint* cause);
  bool notify_unique (Am_Slot_Key key, Am_Wrapper* new_value);
  bool propagate_unique (Am_Slot_Key key, Am_Wrapper* new_value);
  void sever_slot (Am_Slot_Data* proto);
  void sever_copies (Am_Slot_Data* proto);
  void remove_temporary_slot (Am_Slot_Key key);
  void remove_temporaries (Am_Slot_Key key);
  void convert_temporary_slot (Am_Slot_Key key);
  void convert_temporaries (Am_Slot_Key key);
  void remove_subconstraint (Am_Slot_Key key, Am_Constraint* proto);
  void prop_remove_constraint (Am_Slot_Key key, Am_Constraint* proto);
  void set_constraint (Am_Slot_Key key, Am_Constraint* constraint,
                       Am_Slot_Data* proto);
  void propagate_constraint (Am_Slot_Key key, Am_Constraint* constraint,
                             Am_Slot_Data* proto);
  void delete_slot (Am_Slot_Data* slot, Am_Slot_Data* proto_slot);
  void remove_part ();
  void enqueue_change (Am_Slot_Data* slot);
  void enqueue_invalid (Am_Slot_Data* slot);
  void print_slot_name_and_value (Am_Slot_Key key, Am_Slot_Data* value) const;

  Am_Object_Data* prototype;
  Am_Object_Data* first_instance;
  Am_Object_Data* next_instance;
  Am_Object_Data* first_part;
  Am_Object_Data* next_part;
  Am_Slot_Data owner_slot;
  Am_Slot_Data part_slot;
  Am_Demon_Set_Data* demon_set;
  Am_Demon_Queue demon_queue;
  unsigned short default_bits;
  unsigned short bits_mask;
  unsigned short demons_active;
  Am_Inherit_Rule default_rule;
  DynArray data;
};

#endif
