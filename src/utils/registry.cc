// registry.cc
// written by Alan Ferrency
// August 1995
// registry of names for debugging Amulet wrappers and method wrappers.

// only compile this in if we're debugging.
#ifdef DEBUG

#include <am_inc.h>

#include REGISTRY__H // for registry stuff.
#include TYPES__H // for wrappers
#include UNIV_MAP__H // for hash table routines.

/* Implementation details of the registry:
   
We have a one-way hash table which hashes items (registry entries can be
wrappers, methods, or constraints) according to their registry key (the name
and type_ID of the item combined).  For registry entries, wee really want
to key on two pieces of information, the string name of the item, and its
type ID.  We need both pieces of information to determine that two keys
are equal.
*/

Am_Registry_Key::Am_Registry_Key () {
  name = NULL;
}
Am_Registry_Key::Am_Registry_Key (const char *the_name) {
  //name = new char[strlen(the_name) + 1];
  //strcpy (name, the_name);
  name = the_name;
}
bool Am_Registry_Key::operator== (const Am_Registry_Key& test_key) {
  if (test_key.name && name)
    return !strcmp(name, test_key.name);
  else return false; // no null names are equal to anything
  }
bool Am_Registry_Key::operator!= (const Am_Registry_Key& test_key) {
  return !(*this == test_key);
}
bool Am_Registry_Key::Valid() {
  return !!(name);  // if it's !=0
}
Am_Registry_Key::operator const char* () {
  return name;
}

int HashValue (Am_Registry_Key key, int size)
{
  // af1x
  // This is based on the string hash function from univ_map.cc
  // That one simply summed up the first two and last two characters, 
  // and normalized.
  // With most object names, etc, we're going to have the most useful 
  // information in the end of the string (often "..._123_127_...") and not
  // in the beginning of the string (usually "Am...") so I'll just use the
  // last four characters instead of anything from the beginning.

  unsigned base;
  const char *name = key.Name();
  unsigned len = name ? strlen(name) : 0;
  switch (len) {
    case 0: return 0;
    case 1: base = name[0]*4; break;
    case 2: base = name[0]*2 + name[1]*2; break;
    case 3: base = name[0] + name[1] + name[2]*2; break;
    default: base = name[len - 1] + name[len - 2] +
      name[len - 3] + name[len - 4];
  }
  return base * unsigned (0x10000L / 4 / 0x100)  % size;
}

int HashValue (const Am_Registered_Type* entry, int size)
{
  // just use the pointer
  // *OLD*  return (long)entry % size ;
  //new: from Yann LE BIANNIC <lebiannic@dassault-avion.fr> 26 Jun 1996 
  // the division by sizeof(entry) is intended to prevent "holes" in
  // the hash table
  return ((unsigned long)entry / sizeof(entry)) % size;
}

int KeyComp (Am_Registry_Key key1, Am_Registry_Key key2)
{
// uses Am_Registry_Key::operator!=
// returns strcmp-like compare results: 0 if equal, !=0 if unequal.
  return key1 != key2;
}

int KeyComp (const Am_Registered_Type* key1, const Am_Registered_Type* key2)
{
// uses Am_Registry_Entry::operator!=
// returns strcmp-like compare results: 0 if equal, !=0 if unequal.
  return key1 != key2;  // Am_Registered_Type* has no op== so
  // it should just compare pointers.
}

Am_Registry_Key Am_No_Registry_Key;
const Am_Registered_Type* Am_No_Registry_Entry = NULL;

// declare and implement custom hash table type

// the forward mapping (key to entry)
Am_DECL_MAP (Registry, Am_Registry_Key, const Am_Registered_Type*)

Am_IMPL_MAP (Registry, Am_Registry_Key, Am_No_Registry_Key,
	     const Am_Registered_Type*, Am_No_Registry_Entry);

// the reverse mapping (entry back to key)
Am_DECL_MAP (Registry_Reverse, const Am_Registered_Type*, Am_Registry_Key)

Am_IMPL_MAP (Registry_Reverse, const Am_Registered_Type*, Am_No_Registry_Entry,
	     Am_Registry_Key, Am_No_Registry_Key);

// the bidirectional table
Am_DECL_TABLE (Registry, Registry_Reverse, Am_Registry_Key, 
	       const Am_Registered_Type*);

Am_Table_Registry* Am_Name_Registry = NULL;

// Utility to make sure registry is initialized
inline void verify_name_registry() {
  // make the table pretty big, to hold lots of names.
  // around 1000 names registered in testwidgets (9-6-95)
  if (Am_Name_Registry == NULL)
    Am_Name_Registry = new Am_Table_Registry (2000);
}

////////
// The registry routines
// register an item/ name pair

void Am_Register_Name (const Am_Registered_Type* item, const char *name) {
  verify_name_registry();
  Am_Registry_Key key (name);
  if (key.Valid())  // only register valid keys
    Am_Name_Registry->SetAt(key, item);
}

// we unregister the names when they're deleted
void Am_Unregister_Name (const Am_Registered_Type* item) {
  verify_name_registry();
  if (item)  // only unregister valid items.
    Am_Name_Registry->DeleteKey(item);
}

const char* Am_Get_Name_Of_Item (const Am_Registered_Type* item) {
  verify_name_registry();
  if (item)
    return (Am_Name_Registry->GetAt(item));
  else return NULL;
}

const Am_Registered_Type* Am_Get_Named_Item (const char* name) {
  verify_name_registry();
  Am_Registry_Key key (name);
  if (key.Valid())
    return Am_Name_Registry->GetAt(key);
  else return Am_No_Registry_Entry;
}

Am_Map_Int2Str* Am_Type_Registry = NULL;

inline void verify_type_registry () {
  if (Am_Type_Registry == NULL) Am_Type_Registry = new Am_Map_Int2Str;
}

void Am_Register_Type_Name (Am_ID_Tag id, const char* type_name)
{
  verify_type_registry ();
  Am_Type_Registry->SetAt (id, (char*)type_name);
}

void Am_Unregister_Type_Name (Am_ID_Tag id)
{
  verify_type_registry ();
  Am_Type_Registry->DeleteKey(id);
}

const char* Am_Get_Type_Name (Am_ID_Tag id)
{
  verify_type_registry ();
  return Am_Type_Registry->GetAt (id);
}

#endif
