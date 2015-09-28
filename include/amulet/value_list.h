/* ****************************** -*-c++-*- *******************************/
/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifndef VALUE_LIST_H
#define VALUE_LIST_H

#include <am_inc.h>

#include TYPES__H

class Am_Value_List_Data;

class Am_List_Item;

class Am_Value_List {
  Am_WRAPPER_DECL (Am_Value_List)
 public:
  Am_Value_List ();

  bool operator== (const Am_Value_List& test_list) const;
  bool operator!= (const Am_Value_List& test_list) const;

  // Returns the number of elements in the list.
  unsigned short Length () const;

  // Returns whether list is empty or not.
  bool Empty () const;

  void Start (); // Make first element be current.
  void End ();   // Make last element be current.

  void Prev ();  // Make previous element be current.
  void Next ();  // Make next element be current.

  // Returns TRUE when current element passes the first element.
  bool First () const;

  // Returns TRUE when current element passes the last element.
  bool Last () const;

  // Retrieve the value of the current element.  Error if no element is
  // current.
  Am_Value& Get () const;

  // Retrieve the type of the current element.  Error if no element is
  // current.
  Am_Value_Type Get_Type () const;

  // Retrieves the value of the index'th item of the list, starting from
  // the front, with 0 being the first item.  Raises error if list is not
  // at least index+1 long.  Does not use or affect the current pointer.
  Am_Value& Get_Nth (int index) const;

  // Moves current pointer to the index'th item.  Raises error if list is not
  // at least index+1 long.
  void Move_Nth (int index);

  // Returns TRUE is given value is a member of the list.  The search begins
  // at whatever element is current.  If the search is successful, the current
  // element will point to the successful member.  If the search is not
  // successful the current element pointer will be left stationary.
  bool Member (Am_Wrapper*     value);
  bool Member (Am_Ptr          value);
  bool Member (int             value);
  bool Member (long            value);
#if !defined(NEED_BOOL)
  bool Member (bool            value);
#endif
  bool Member (float           value);
  bool Member (double          value);
  bool Member (char            value);
  bool Member (const char*     value);
  bool Member (const Am_String& value);
  bool Member (Am_Method_Wrapper* value);
  bool Member (Am_Generic_Procedure* value);
  bool Member (const Am_Value& value);

#ifdef AMULET2_INSTRUMENT
#undef Set
#undef Add
#endif

  // Add puts the new element at the head or tail of the list.
  Am_Value_List& Add (Am_Wrapper* value, Am_Add_Position position = Am_TAIL,
                      bool unique = true);
  Am_Value_List& Add (Am_Ptr value, Am_Add_Position position = Am_TAIL,
                      bool unique = true);
  Am_Value_List& Add (int value, Am_Add_Position position = Am_TAIL,
                      bool unique = true);
  Am_Value_List& Add (long value, Am_Add_Position position = Am_TAIL,
                      bool unique = true);
#if !defined(NEED_BOOL)
  Am_Value_List& Add (bool value, Am_Add_Position position = Am_TAIL,
                      bool unique = true);
#endif
  Am_Value_List& Add (float value, Am_Add_Position position = Am_TAIL,
                      bool unique = true);
  Am_Value_List& Add (double value, Am_Add_Position position = Am_TAIL,
                      bool unique = true);
  Am_Value_List& Add (char value, Am_Add_Position position = Am_TAIL,
                      bool unique = true);
  Am_Value_List& Add (const char* value, Am_Add_Position position = Am_TAIL,
                      bool unique = true);
  Am_Value_List& Add (const Am_String& value,
                      Am_Add_Position position = Am_TAIL, bool unique = true);
  Am_Value_List& Add (Am_Method_Wrapper* value,
                      Am_Add_Position position = Am_TAIL, bool unique = true);
  Am_Value_List& Add (Am_Generic_Procedure* value,
                      Am_Add_Position position = Am_TAIL, bool unique = true);
  Am_Value_List& Add (const Am_Value& value,
                      Am_Add_Position position = Am_TAIL, bool unique = true);

  // Insert puts the new element before or after the current position
  // in the list.  The current position is set by using the Start, End, Next,
  // and Prev methods.
  void Insert (Am_Wrapper*     value, Am_Insert_Position position,
               bool unique = true);
  void Insert (Am_Ptr          value, Am_Insert_Position position,
               bool unique = true);
  void Insert (int             value, Am_Insert_Position position,
               bool unique = true);
  void Insert (long            value, Am_Insert_Position position,
               bool unique = true);
#if !defined(NEED_BOOL)
  void Insert (bool            value, Am_Insert_Position position,
               bool unique = true);
#endif
  void Insert (float           value, Am_Insert_Position position,
               bool unique = true);
  void Insert (double          value, Am_Insert_Position position,
               bool unique = true);
  void Insert (char            value, Am_Insert_Position position,
               bool unique = true);
  void Insert (const char*     value, Am_Insert_Position position,
               bool unique = true);
  void Insert (const Am_String& value, Am_Insert_Position position,
               bool unique = true);
  void Insert (Am_Method_Wrapper* value, Am_Insert_Position position,
               bool unique = true);
  void Insert (Am_Generic_Procedure* value, Am_Insert_Position position,
               bool unique = true);
  void Insert (const Am_Value& value, Am_Insert_Position position,
               bool unique = true);

  // Change the value of the current element.  Error if no element is current.
  void Set (Am_Wrapper*     value, bool unique = true);
  void Set (Am_Ptr          value, bool unique = true);
  void Set (int             value, bool unique = true);
  void Set (long            value, bool unique = true);
#if !defined(NEED_BOOL)
  void Set (bool            value, bool unique = true);
#endif
  void Set (float           value, bool unique = true);
  void Set (double          value, bool unique = true);
  void Set (char            value, bool unique = true);
  void Set (const char*     value, bool unique = true);
  void Set (const Am_String& value, bool unique = true);
  void Set (Am_Method_Wrapper* value, bool unique = true);
  void Set (Am_Generic_Procedure* value, bool unique = true);
  void Set (const Am_Value&       value, bool unique = true);

#ifdef AMULET2_INSTRUMENT
  Am_Instrumentation(Am_Value_List)
#define Set Am_Instrumented(Set)
#define Add Am_Instrumented(Add)
#endif

  //adds items in other_list to my end.  Returns me (this) (so can be cascaded)
  Am_Value_List& Append (Am_Value_List other_list, bool unique = true);

  // Delete the current element.  Error if no element is current.  The current
  // position is shifted to the element previous to the deleted.
  void Delete (bool unique = true);

  // Creates an actual copy of the list contents.  Useful for making a list
  // unique when the program does not know how it will become unique.
  Am_Value_List Copy ();

  // Delete the entire list.  All elements are deleted. The current position
  void Make_Empty ();  // becomes undefined.

  // Creates an empty list that is not NULL.
  static Am_Value_List Empty_List ();

 private:
  Am_List_Item* item;
};

// Create a Type ID for Am_Value_List.
const Am_Value_Type Am_VALUE_LIST = Am_WRAPPER_TYPE | 2;

extern Am_Value_List Am_No_Value_List;

ostream& operator<< (ostream& os, const Am_Value_List& list);
     
// Example:
//  #include <amulet/valuelist.h>
//  main ()
//  {
//    Am_Value_List my_list;
//    my_list.Add (0).Add (1).Add (2).Add (3);
//
//    // Iterating through a list.
//    int val;
//    for (my_list.Start (); !my_list.Last (); my_list.Next ()) {
//      val = my_list.Get ();
//      cout << val;
//    }
//    cout << endl;
//
//    // Testing for membership.
//    my_list.Start ();
//    if (my_list.Member (2))
//      cout << "search successful" << endl;
//    else
//      cout << "search not successful" << endl;
//  }

#endif
