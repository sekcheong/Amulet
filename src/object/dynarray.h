/* ************************************************************************ 
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

#ifndef DYNARRAY_H
#define DYNARRAY_H

#ifdef AMULET2_INSTRUMENT
#undef Set
#undef Add
#endif

class DynArray {
 public:
  DynArray (unsigned elem_size);

  char* Get (unsigned i);
  void Set (unsigned i, char* value);

  void Insert (unsigned i, char* value);
  void Delete (unsigned i);
  void Add (char* value);

  DynArray* Copy ();
  void Destroy ();

  unsigned elem_size;
  unsigned data_size;
  unsigned length;
  char* data;
};

#ifdef AMULET2_INSTRUMENT
#define Set Am_Instrumented(Set)
#define Add Am_Instrumented(Add)
#endif

class Dyn_Link {
 public:
  Dyn_Link* next;
};

class Dyn_Memory_Manager {
 public:
  Dyn_Memory_Manager (unsigned short size)
  {
    data_size = size;
    free_list = NULL;
    block_length = 1;
    block_position = 0;
    unsigned short first_block_size = 1;
    unsigned short min_block_size = size + sizeof (Dyn_Link);
    while (first_block_size < min_block_size)
      first_block_size <<= 1;
    block_size = first_block_size;
    block = (Dyn_Link*)new char [first_block_size];
    block->next = NULL;
  }
  ~Dyn_Memory_Manager ()
  {
    Dyn_Link* curr = block;
    Dyn_Link* next;
    while (curr) {
      next = curr->next;
      delete[] curr;
      curr = next;
    }
    block = NULL;
    free_list = NULL;
  }
  void* New ()
  {
    void* new_ptr;
    if (free_list) {
      new_ptr = free_list;
      free_list = free_list->next;
    }
    else {
      if (block_position == block_length) {
        block_position = 0;
        block_size <<= 1;
        block_length = (block_size - sizeof (Dyn_Link)) / data_size;
        Dyn_Link* new_block = (Dyn_Link*)new char [block_size];
        new_block->next = block;
        block = new_block;
      }
#ifndef __alpha
      new_ptr = (void*)(((unsigned)block) + block_position * data_size +
			sizeof (Dyn_Link));
#else
      new_ptr = (void*)(((unsigned long)block) + block_position * data_size +
			sizeof (Dyn_Link));
#endif
      block_position++;
    }
    return new_ptr;
  }
  void Delete (void* ptr)
  {
    Dyn_Link* old_ptr = (Dyn_Link*)ptr;
    old_ptr->next = free_list;
    free_list = old_ptr;
  }

 private:
  unsigned short data_size;
  unsigned long block_size;
  unsigned long block_length;
  unsigned long block_position;
  Dyn_Link* block;
  Dyn_Link* free_list;
};

#endif
