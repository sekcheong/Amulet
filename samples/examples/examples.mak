# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

!IF "$(CFG)" == ""
CFG=examples - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to examples - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "examples - Win32 Release" && "$(CFG)" !=\
 "examples - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "examples.mak" CFG="examples - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "examples - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "examples - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "examples - Win32 Debug"

!IF  "$(CFG)" == "examples - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f examples.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "examples.exe"
# PROP BASE Bsc_Name "examples.bsc"
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE /k /f Makefile OP="$(AM_RELEASE)" LIB_MODIFIER=-release"
# PROP Rebuild_Opt "/a"
# PROP Target_File "examples"
# PROP Bsc_Name ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "examples - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f examples.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "examples.exe"
# PROP BASE Bsc_Name "examples.bsc"
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE /k /f Makefile"
# PROP Rebuild_Opt "/a"
# PROP Target_File "examples"
# PROP Bsc_Name ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ENDIF 

################################################################################
# Begin Target

# Name "examples - Win32 Release"
# Name "examples - Win32 Debug"

!IF  "$(CFG)" == "examples - Win32 Release"

"$(OUTDIR)\examples" : 
   CD C:\amulet\samples\examples
   NMAKE /k /f Makefile OP="$(AM_RELEASE)" LIB_MODIFIER=-release

!ELSEIF  "$(CFG)" == "examples - Win32 Debug"

"$(OUTDIR)\examples" : 
   CD C:\amulet\samples\examples
   NMAKE /k /f Makefile

!ENDIF 

# End Target
# End Project
################################################################################
