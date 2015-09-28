# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

!IF "$(CFG)" == ""
CFG=amulet - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to amulet - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "amulet - Win32 Release" && "$(CFG)" != "amulet - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "amulet.mak" CFG="amulet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "amulet - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "amulet - Win32 Debug" (based on "Win32 (x86) External Target")
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
# PROP Target_Last_Scanned "amulet - Win32 Debug"

!IF  "$(CFG)" == "amulet - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f amulet.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "amulet.exe"
# PROP BASE Bsc_Name "amulet.bsc"
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE /f Makefile OP="$(AM_RELEASE)" LIB_MODIFIER=-release amulet tests samples"
# PROP Rebuild_Opt "/a"
# PROP Target_File "amulet tests samples"
# PROP Bsc_Name ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : 

CLEAN : 
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

!ELSEIF  "$(CFG)" == "amulet - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP BASE Cmd_Line "NMAKE /f amulet.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "amulet.exe"
# PROP BASE Bsc_Name "amulet.bsc"
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# PROP Cmd_Line "NMAKE /f Makefile all"
# PROP Rebuild_Opt "/a"
# PROP Target_File "all"
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

# Name "amulet - Win32 Release"
# Name "amulet - Win32 Debug"

!IF  "$(CFG)" == "amulet - Win32 Release"

"$(OUTDIR)\amulet tests samples" : 
   CD C:\amulet\bin
   NMAKE /f Makefile OP="$(AM_RELEASE)" LIB_MODIFIER=-release amulet tests\
 samples

!ELSEIF  "$(CFG)" == "amulet - Win32 Debug"

"$(OUTDIR)\all" : 
   CD C:\amulet\bin
   NMAKE /f Makefile all

!ENDIF 

# End Target
# End Project
################################################################################
