/* ************************************************************************
 *         The Amulet User Interface Development Environment              *
 * ************************************************************************
 * This code was written as part of the Amulet project at                 *
 * Carnegie Mellon University, and has been placed in the public          *
 * domain.  If you are using this code or any part of Amulet,             *
 * please contact amulet@cs.cmu.edu to be put on the mailing list.        *
 * ************************************************************************/

/*  
  I have made this file so that it automatically compiles certain options
  based on the version and the platform.
  
  _MACINTOSH is defined for Mac based apps in the precompiled header
  
  For CW10 and CW11, the older "obsolete" standard library is envoked and
  bool support is off
  For CW Pro 1 (CW12) and later, bool must be on in the project preferences
  and the new MSL is automatically used
  
  DEBUG is defined as 1 in Am_DebugPrefix.h and undefined in Am_Prefix.h
  MEMORY is always on since it uses the built in Amulet memory manager
  that will require far fewer pointers
  AMULET2_CONVERSION is undefined but can be defined to 1 by the developer
  
  bdk 6/19/97
*/

#if __MWERKS__
#  if macintosh
#    ifdef powerc
#      include <AmuletHeadersPPC>
#    else
#      include <AmuletHeaders68K>
#    endif
#  else
#    error "This file is being used for an unsupported platform"
#  endif /* macintosh */
#else
#  error "This file is being used for an unsupported compiler"
#endif /* __MWERKS__ */

// Amulet Definitions
#define MEMORY              1
#undef  DEBUG

// ------------------- AMULET2_CONVERSION --------------------
// Uncomment AMULET2_CONVERSION only for new slots and Set only for 
// existing slots in V3, where in V2 Set was used for both.  With 
// AMULET2_CONVERSION defined, mis-uses of Add and Set are flagged 
// as warnings, not a errors.  It is intended to be used in 
// converting V2 code to V3.

// #define AMULET2_CONVERSION  1

// ---------------------- MSL SUPPORT -----------------------
#if (__MWERKS__ >= 0x1800) /* do MSL if CW Pro 1 (CW12) or later */
#define AM_USES_MSL 1
#endif  

#ifdef AM_USES_MSL
#  if __option(bool)
#    undef NEED_BOOL
#  else
#    error "MSL Support wants to Bool option turned on in project settings"
#  endif
#else
// CodeWarrior 10 has native bool support. Unfortunately the compiler
// implicitly converts any pointers to bool, instead of void*. This is a
// major problem so the NEED_BOOL macro remains...
#  if __option(bool)
#    define NEED_BOOL 1
#  else
#    error "Bool option must be off when you use the obsolete standard library"
#  endif
#endif /* AM_USES_MSL */
